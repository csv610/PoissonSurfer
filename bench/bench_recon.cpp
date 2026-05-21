#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <cstdio>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "../src/SparseMatrix.h"
#include "../src/MyTime.h"
#include "../src/Octree.h"
#include "../src/MultiGridOctreeData.h"
#include "../src/PointStream.h"
#include "../src/Geometry.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template< class Real > void SetIsoVertexValue( PlyOrientedVertex< float >& vertex , Real value ){ ; }

struct BenchResult {
    int depth;
    int pointCount;
    int vertexCount;
    double treeTime;
    double constraintTime;
    double solveTime;
    double isoTime;
    double mcTime;
    double totalTime;
};

static std::vector< std::pair< Point3D< float > , Point3D< float > > > makeSphere( int n )
{
    std::vector< std::pair< Point3D< float > , Point3D< float > > > pts( n );
    for( int i=0 ; i<n ; i++ )
    {
        float u = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
        float v = static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
        float theta = 2.0f * static_cast<float>( M_PI ) * u;
        float phi = acosf( 2.0f * v - 1.0f );
        float x = 0.3f * sinf( phi ) * cosf( theta ) + 0.5f;
        float y = 0.3f * sinf( phi ) * sinf( theta ) + 0.5f;
        float z = 0.3f * cosf( phi ) + 0.5f;
        pts[i].first  = Point3D< float >( x , y , z );
        pts[i].second = Point3D< float >( sinf(phi)*cosf(theta) , sinf(phi)*sinf(theta) , cosf(phi) );
    }
    return pts;
}

static std::vector< std::pair< Point3D< float > , Point3D< float > > > makeCube( int nPerFace )
{
    std::vector< std::pair< Point3D< float > , Point3D< float > > > pts;
    auto add_face = [&]( Point3D< float > n , Point3D< float > u , Point3D< float > v )
    {
        for( int i=0 ; i<nPerFace ; i++ )
            for( int j=0 ; j<nPerFace ; j++ )
            {
                float fu = ( i / static_cast<float>( nPerFace-1 ) ) - 0.5f;
                float fv = ( j / static_cast<float>( nPerFace-1 ) ) - 0.5f;
                Point3D< float > p = n * 0.3f + u * fu * 0.6f + v * fv * 0.6f + Point3D< float >( 0.5f , 0.5f , 0.5f );
                pts.push_back( { p , n } );
            }
    };
    add_face( { 1, 0, 0} , {0, 1, 0} , {0, 0, 1} );
    add_face( {-1, 0, 0} , {0, 1, 0} , {0, 0, 1} );
    add_face( { 0, 1, 0} , {1, 0, 0} , {0, 0, 1} );
    add_face( { 0,-1, 0} , {1, 0, 0} , {0, 0, 1} );
    add_face( { 0, 0, 1} , {1, 0, 0} , {0, 1, 0} );
    add_face( { 0, 0,-1} , {1, 0, 0} , {0, 1, 0} );
    return pts;
}

template< class Real >
static BenchResult benchReconstruction( const std::vector< std::pair< Point3D< float > , Point3D< float > > >& points , int depth )
{
    BenchResult r;
    memset( &r , 0 , sizeof(r) );
    r.depth = depth;
    r.pointCount = static_cast<int>( points.size() );

    OctNode< TreeNodeData >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );

    Octree< Real > tree;
    tree.threads = 1;

    int minDepth = 0;
    int fullDepth = 5;
    int splatDepth = depth - 2;

    typename Octree< Real >::PointInfo pointInfo;
    typename Octree< Real >::NormalInfo normalInfo;
    std::vector< Real > kernelDensityWeights;
    std::vector< Real > centerWeights;

    MemoryPointStream< float > stream( points.size() , points.data() );

    double t0 = Time();
    int pointCount = tree.template SetTree< float >(
        &stream , minDepth , depth , fullDepth , splatDepth , Real(1.0) , Real(1.25) ,
        false , false , Real(4.0) , 1 , pointInfo , normalInfo ,
        kernelDensityWeights , centerWeights , 1
    );
    r.treeTime = Time() - t0;
    if( pointCount <= 0 ) return r;

    double t1 = Time();
    Pointer( Real ) constraints = tree.SetLaplacianConstraints( normalInfo );
    r.constraintTime = Time() - t1;

    double t2 = Time();
    Pointer( Real ) solution = tree.SolveSystem( pointInfo , constraints , false , 8 , depth , 0 , Real(1e-3) );
    r.solveTime = Time() - t2;

    FreePointer( constraints );

    double t3 = Time();
    Real isoValue = tree.GetIsoValue( solution , centerWeights );
    r.isoTime = Time() - t3;

    CoredVectorMeshData< PlyOrientedVertex< float > > mesh;
    double t4 = Time();
    tree.template GetMCIsoSurface< PlyOrientedVertex< float > >( NULL , solution , isoValue , mesh );
    r.mcTime = Time() - t4;

    DeletePointer( solution );

    r.vertexCount = static_cast<int>( mesh.inCorePoints.size() + mesh.outOfCorePointCount() );
    r.totalTime = Time() - t0;

    return r;
}

static void printResults( const std::string& label , const std::vector< BenchResult >& results )
{
    std::cout << "\n=== " << label << " ===\n";
    std::cout << std::left;
    std::cout << std::setw(6) << "Depth"
              << std::setw(10) << "Points"
              << std::setw(10) << "Verts"
              << std::setw(10) << "Tree(s)"
              << std::setw(12) << "Constr(s)"
              << std::setw(10) << "Solve(s)"
              << std::setw(10) << "Total(s)"
              << "\n";
    std::cout << std::string( 68 , '-' ) << "\n";
    for( const auto& r : results )
    {
        std::cout << std::setw(6) << r.depth
                  << std::setw(10) << r.pointCount
                  << std::setw(10) << r.vertexCount
                  << std::setw(10) << std::fixed << std::setprecision(3) << r.treeTime
                  << std::setw(12) << std::fixed << std::setprecision(3) << r.constraintTime
                  << std::setw(10) << std::fixed << std::setprecision(3) << r.solveTime
                  << std::setw(10) << std::fixed << std::setprecision(3) << r.totalTime
                  << "\n";
    }
}

int main()
{
    std::cout << "PoissonSurfer Reconstruction Benchmarks\n";
    std::cout << "========================================\n";

    srand( 42 );

    auto sphere = makeSphere( 5000 );
    auto cube   = makeCube( 30 );

    int depths[] = { 4 , 5 };

    {
        std::vector< BenchResult > sphereResults;
        for( int d : depths )
        {
            std::cout << "  depth " << d << "..." << std::flush;
            BenchResult r = benchReconstruction< float >( sphere , d );
            if( r.vertexCount > 0 ) sphereResults.push_back( r );
            std::cout << " " << r.vertexCount << " verts, " << r.totalTime << " s\n";
        }
        printResults( "Sphere (5K points)" , sphereResults );
    }

    {
        std::vector< BenchResult > cubeResults;
        for( int d : depths )
        {
            std::cout << "  depth " << d << "..." << std::flush;
            BenchResult r = benchReconstruction< float >( cube , d );
            if( r.vertexCount > 0 ) cubeResults.push_back( r );
            std::cout << " " << r.vertexCount << " verts, " << r.totalTime << " s\n";
        }
        printResults( "Cube (30x30 per face, ~5.4K points)" , cubeResults );
    }

    std::cout << "\nBenchmarks complete.\n";
    return 0;
}
