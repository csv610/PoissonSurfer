#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <string>

#include "SparseMatrix.h"
#include "Vector.h"
#include "MyTime.h"
#include "MarchingCubes.h"
#include "Octree.h"
#include "PPolynomial.h"
#include "ply.h"
#include "MemoryUsage.h"

#ifdef _OPENMP
#include "omp.h"
#endif

// Stubs for functions used in MultiGridOctreeData templates
void DumpOutput( const char* format , ... ) {
    va_list args;
    va_start( args , format );
    vprintf( format , args );
    va_end( args );
}
void DumpOutput2( char* str , const char* format , ... ) {
    va_list args;
    va_start( args , format );
    vprintf( format , args );
    va_end( args );
    va_start( args , format );
    vsnprintf( str , 1024 , format , args );
    va_end( args );
}

#include "MultiGridOctreeData.h"
#include "PointStream.h"
#include "Geometry.h"

// Missing overload for GetMCIsoSurface when using PlyOrientedVertex
template< class Real > void SetIsoVertexValue( PlyOrientedVertex< float >& vertex , Real value ){ ; }

// Define a simple vertex structure if needed, or use PlyOrientedVertex
// The Poisson Recon expects PointStream to provide (point, normal)

template<class Real>
int Remesh(const std::vector<std::pair<Point3D<Real>, Point3D<Real>>>& pointCloud, int depth, const std::string& outPath) {
    Octree<Real> tree;
    tree.threads = 1; // Default to 1
    
    int minDepth = 0;
    int fullDepth = 5;
    int splatDepth = depth - 2;
    Real samplesPerNode = 1.0;
    Real scaleFactor = 1.1; 
    bool useConfidence = false;
    bool useNormalWeight = false;
    Real constraintWeight = 4.0;
    int adaptiveExponent = 1;
    int boundaryType = 1; // NEUMANN
    
    typename Octree<Real>::PointInfo pointInfo;
    typename Octree<Real>::NormalInfo normalInfo;
    std::vector<Real> kernelDensityWeights;
    std::vector<Real> centerWeights;
    
    MemoryPointStream<Real> stream(pointCloud.size(), pointCloud.data());
    
    XForm4x4<Real> xForm = XForm4x4<Real>::Identity();
    
    int pointCount = tree.template SetTree<Real>(
        &stream, minDepth, depth, fullDepth, splatDepth, samplesPerNode,
        scaleFactor, useConfidence, useNormalWeight, constraintWeight, adaptiveExponent,
        pointInfo, normalInfo, kernelDensityWeights, centerWeights, boundaryType, xForm
    );
    
    if (pointCount <= 0) return 0;
    
    Pointer(Real) constraints = tree.SetLaplacianConstraints(normalInfo);
    Pointer(Real) solution = tree.SolveSystem(pointInfo, constraints, false, 8, depth, 0, 1e-3);
    
    FreePointer(constraints);
    
    Real isoValue = tree.GetIsoValue(solution, centerWeights);
    
    CoredVectorMeshData<PlyOrientedVertex<float>> mesh;
    tree.template GetMCIsoSurface<PlyOrientedVertex<float>>(NULL, solution, isoValue, mesh);
    
    DeletePointer(solution);
    
    // Write out the mesh
    XForm4x4<Real> iXForm = xForm.inverse();
    std::vector<char> outBuf(outPath.begin(), outPath.end());
    outBuf.push_back(0);
    PlyWritePolygons(outBuf.data(), &mesh, PLY_BINARY_NATIVE, NULL, 0, iXForm);

    return (int)(mesh.inCorePoints.size() + mesh.outOfCorePointCount());
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " -i <input_mesh> -o <output_mesh> [-d <depth>]" << std::endl;
        return 1;
    }

    std::string inPath, outPath;
    int depth = 8;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-i" && i + 1 < argc) inPath = argv[++i];
        else if (std::string(argv[i]) == "-o" && i + 1 < argc) outPath = argv[++i];
        else if (std::string(argv[i]) == "-d" && i + 1 < argc) depth = std::stoi(argv[++i]);
    }

    if (inPath.empty() || outPath.empty()) {
        std::cerr << "Error: Input and output paths are required." << std::endl;
        std::cerr << "Usage: " << argv[0] << " -i <input_mesh> -o <output_mesh> [-d <depth>]" << std::endl;
        return 1;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(inPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error loading mesh: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    std::vector<std::pair<Point3D<float>, Point3D<float>>> pointCloud;
    float epsilon = 0.001f;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3) continue;

            aiVector3D v0 = mesh->mVertices[face.mIndices[0]];
            aiVector3D v1 = mesh->mVertices[face.mIndices[1]];
            aiVector3D v2 = mesh->mVertices[face.mIndices[2]];

            Point3D<float> p0(v0.x, v0.y, v0.z);
            Point3D<float> p1(v1.x, v1.y, v1.z);
            Point3D<float> p2(v2.x, v2.y, v2.z);

            Point3D<float> edge1 = p1 - p0;
            Point3D<float> edge2 = p2 - p0;
            Point3D<float> normal;
            CrossProduct(edge1, edge2, normal);
            float len = sqrt(SquareLength(normal));
            if (len > 0) normal /= len;

            Point3D<float> center = (p0 + p1 + p2) / 3.0f;

            // 7 points
            // Corners (slightly pushed in)
            pointCloud.push_back({p0 * (1.0f - 2.0f * epsilon) + p1 * epsilon + p2 * epsilon, normal});
            pointCloud.push_back({p1 * (1.0f - 2.0f * epsilon) + p0 * epsilon + p2 * epsilon, normal});
            pointCloud.push_back({p2 * (1.0f - 2.0f * epsilon) + p0 * epsilon + p1 * epsilon, normal});

            // Edges (slightly pushed in)
            pointCloud.push_back({(p0 + p1) * 0.5f * (1.0f - epsilon) + center * epsilon, normal});
            pointCloud.push_back({(p1 + p2) * 0.5f * (1.0f - epsilon) + center * epsilon, normal});
            pointCloud.push_back({(p2 + p0) * 0.5f * (1.0f - epsilon) + center * epsilon, normal});

            // Center
            pointCloud.push_back({center, normal});
        }
    }

    std::cout << "Generated " << pointCloud.size() << " points." << std::endl;
    
    OctNode<TreeNodeData>::SetAllocator(1024 * 1024);
    int vertexCount = Remesh<float>(pointCloud, depth, outPath);
    std::cout << "Remeshed mesh has " << vertexCount << " vertices." << std::endl;

    return 0;
}
