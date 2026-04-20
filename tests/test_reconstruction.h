#pragma once
#include <omp.h>
#include "../src/MyTime.h"
#include "../src/Octree.h"
#include "../src/MultiGridOctreeData.h"
#include "../src/PointStream.h"
#include "../src/Geometry.h"
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Missing overload for GetMCIsoSurface when using PlyOrientedVertex
template< class Real > void SetIsoVertexValue( PlyOrientedVertex< float >& vertex , Real value ){ ; }

namespace {

template<class Real>
int Reconstruct(PointStream<float>* pointStream, int depth) {
    Octree<Real> tree;
    tree.threads = 1;
    
    int minDepth = 0;
    int fullDepth = 5;
    int splatDepth = depth - 2;
    Real samplesPerNode = 1.0;
    Real scaleFactor = 1.25;
    bool useConfidence = false;
    bool useNormalWeight = false;
    Real constraintWeight = 4.0;
    int adaptiveExponent = 1;
    int boundaryType = 1; // NEUMANN
    
    typename Octree<Real>::PointInfo pointInfo;
    typename Octree<Real>::NormalInfo normalInfo;
    std::vector<Real> kernelDensityWeights;
    std::vector<Real> centerWeights;
    
    // OctNode<TreeNodeData>::SetAllocator(1024 * 1024);
    // Reset<Real>();
    
    int pointCount = tree.template SetTree<float>(
        pointStream, minDepth, depth, fullDepth, splatDepth, samplesPerNode,
        scaleFactor, useConfidence, useNormalWeight, constraintWeight, adaptiveExponent,
        pointInfo, normalInfo, kernelDensityWeights, centerWeights, boundaryType
    );
    
    if (pointCount <= 0) return 0;
    
    Pointer(Real) constraints = tree.SetLaplacianConstraints(normalInfo);
    Pointer(Real) solution = tree.SolveSystem(pointInfo, constraints, false, 8, depth, 0, 1e-3);
    
    FreePointer(constraints);
    
    Real isoValue = tree.GetIsoValue(solution, centerWeights);
    
    CoredVectorMeshData<PlyOrientedVertex<float>> mesh;
    tree.template GetMCIsoSurface<PlyOrientedVertex<float>>(NULL, solution, isoValue, mesh);
    
    DeletePointer(solution);
    
    return (int)(mesh.inCorePoints.size() + mesh.outOfCorePointCount());
}

void test_reconstruct_sphere() {
    std::cout << "Testing sphere reconstruction..." << std::endl;
    const int numPoints = 2000;
    std::vector<std::pair<Point3D<float>, Point3D<float>>> points(numPoints);
    
    for (int i = 0; i < numPoints; ++i) {
        float u = (float)rand() / (float)RAND_MAX;
        float v = (float)rand() / (float)RAND_MAX;
        float theta = 2.0f * (float)M_PI * u;
        float phi = acos(2.0f * v - 1.0f);
        
        float x = 0.3f * sin(phi) * cos(theta) + 0.5f;
        float y = 0.3f * sin(phi) * sin(theta) + 0.5f;
        float z = 0.3f * cos(phi) + 0.5f;
        
        points[i].first = Point3D<float>(x, y, z);
        points[i].second = Point3D<float>(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    }
    
    MemoryPointStream<float> stream(numPoints, points.data());
    int vertexCount = Reconstruct<float>(&stream, 6);
    std::cout << "Sphere vertices: " << vertexCount << std::endl;
    assert(vertexCount > 0);
}

void test_reconstruct_cube() {
    std::cout << "Testing cube reconstruction..." << std::endl;
    std::vector<std::pair<Point3D<float>, Point3D<float>>> points;
    
    auto add_face = [&](Point3D<float> n, Point3D<float> u, Point3D<float> v) {
        for (int i = 0; i < 40; ++i) {
            for (int j = 0; j < 40; ++j) {
                float fu = (i / 39.0f) - 0.5f;
                float fv = (j / 39.0f) - 0.5f;
                Point3D<float> p = n * 0.3f + u * fu * 0.6f + v * fv * 0.6f + Point3D<float>(0.5f, 0.5f, 0.5f);
                points.push_back({p, n});
            }
        }
    };
    
    add_face({ 1, 0, 0}, {0, 1, 0}, {0, 0, 1});
    add_face({-1, 0, 0}, {0, 1, 0}, {0, 0, 1});
    add_face({ 0, 1, 0}, {1, 0, 0}, {0, 0, 1});
    add_face({ 0,-1, 0}, {1, 0, 0}, {0, 0, 1});
    add_face({ 0, 0, 1}, {1, 0, 0}, {0, 1, 0});
    add_face({ 0, 0,-1}, {1, 0, 0}, {0, 1, 0});
    
    MemoryPointStream<float> stream(points.size(), points.data());
    int vertexCount = Reconstruct<float>(&stream, 6);
    std::cout << "Cube vertices: " << vertexCount << std::endl;
    assert(vertexCount > 0);
}

}

int test_reconstruction() {
    OctNode<TreeNodeData>::SetAllocator(1024 * 1024);
    test_reconstruct_sphere();
    test_reconstruct_cube();
    return 0;
}
