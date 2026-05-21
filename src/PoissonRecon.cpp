/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#endif // _WIN32
#include "MyTime.h"
#include "MarchingCubes.h"
#include "Octree.h"
#include "SparseMatrix.h"
#include "CmdLineParser.h"
#include "PPolynomial.h"
#include "ply.h"
#include "MemoryUsage.h"
#ifdef _OPENMP
#include "omp.h"
#endif // _OPENMP
void DumpOutput( const char* format , ... );
void DumpOutput2( char* str , const char* format , ... );
#include "MultiGridOctreeData.h"

#define DEFAULT_FULL_DEPTH 5

#include <stdarg.h>
char* outputFile=NULL;
int echoStdout=0;
void DumpOutput( const char* format , ... )
{
	if( outputFile )
	{
		FILE* fp = fopen( outputFile , "a" );
		va_list args;
		va_start( args , format );
		vfprintf( fp , format , args );
		fclose( fp );
		va_end( args );
	}
	if( echoStdout )
	{
		va_list args;
		va_start( args , format );
		vprintf( format , args );
		va_end( args );
	}
}
void DumpOutput2( char* str , const char* format , ... )
{
	if( outputFile )
	{
		FILE* fp = fopen( outputFile , "a" );
		va_list args;
		va_start( args , format );
		vfprintf( fp , format , args );
		fclose( fp );
		va_end( args );
	}
	if( echoStdout )
	{
		va_list args;
		va_start( args , format );
		vprintf( format , args );
		va_end( args );
	}
	va_list args;
	va_start( args , format );
	vsprintf( str , format , args );
	va_end( args );
	if( str[strlen(str)-1]=='\n' ) str[strlen(str)-1] = 0;
}


cmdLineString
	In( "in" ) ,
	Out( "out" ) ,
	VoxelGrid( "voxel" ) ,
	XForm( "xForm" );

cmdLineReadable
#ifdef _WIN32
	Performance( "performance" ) ,
#endif // _WIN32
	Complete( "complete" ) ,
	ShowResidual( "showResidual" ) ,
	NoComments( "noComments" ) ,
	PolygonMesh( "polygonMesh" ) ,
	Confidence( "confidence" ) ,
	NormalWeights( "nWeights" ) ,
	NonManifold( "nonManifold" ) ,
	ASCII( "ascii" ) ,
	Density( "density" ) ,
	Verbose( "verbose" ) ,
	Double( "double" );

cmdLineInt
	Depth( "depth" , 8 ) ,
	CGDepth( "cgDepth" , 0 ) ,
	KernelDepth( "kernelDepth" ) ,
	AdaptiveExponent( "adaptiveExp" , 1 ) ,
	Iters( "iters" , 8 ) ,
	VoxelDepth( "voxelDepth" , -1 ) ,
	FullDepth( "fullDepth" , DEFAULT_FULL_DEPTH ) ,
	MinDepth( "minDepth" , 0 ) ,
	MaxSolveDepth( "maxSolveDepth" ) ,
	BoundaryType( "boundary" , 1 ) ,
	Threads( "threads" , omp_get_num_procs() );

cmdLineFloat
	SamplesPerNode( "samplesPerNode" , 1.f ) ,
	Scale( "scale" , 1.1f ) ,
	CSSolverAccuracy( "cgAccuracy" , float(1e-3) ) ,
	PointWeight( "pointWeight" , 4.f );


cmdLineReadable* params[] =
{
	&In , &Depth , &Out , &XForm ,
	&Scale , &Verbose , &CSSolverAccuracy , &NoComments , &Double ,
	&KernelDepth , &SamplesPerNode , &Confidence , &NormalWeights , &NonManifold , &PolygonMesh , &ASCII , &ShowResidual , &VoxelDepth ,
	&PointWeight , &VoxelGrid , &Threads , &MaxSolveDepth ,
	&AdaptiveExponent , &BoundaryType ,
	&Density ,
	&FullDepth ,
	&MinDepth ,
	&CGDepth , &Iters ,
	&Complete ,
#ifdef _WIN32
	&Performance ,
#endif // _WIN32
};


void ShowUsage(char* ex)
{
	printf( "Usage: %s\n" , ex );
	printf( "\t --%s  <input points>\n" , In.name );

	printf( "\t[--%s <ouput triangle mesh>]\n" , Out.name );
	printf( "\t[--%s <ouput voxel grid>]\n" , VoxelGrid.name );

	printf( "\t[--%s <maximum reconstruction depth>=%d]\n" , Depth.name , Depth.value );
	printf( "\t\t Running at depth d corresponds to solving on a 2^d x 2^d x 2^d\n" );
	printf( "\t\t voxel grid.\n" );

	printf( "\t[--%s <full depth>=%d]\n" , FullDepth.name , FullDepth.value );
	printf( "\t\t This flag specifies the depth up to which the octree should be complete.\n" );

	printf( "\t[--%s <depth at which to extract the voxel grid>=<%s>]\n" , VoxelDepth.name , Depth.name );

	printf( "\t[--%s <conjugate-gradients depth>=%d]\n" , CGDepth.name , CGDepth.value );
	printf( "\t\t The depth up to which a conjugate-gradients solver should be used.\n");

	printf( "\t[--%s <scale factor>=%f]\n" , Scale.name , Scale.value );
	printf( "\t\t Specifies the factor of the bounding cube that the input\n" );
	printf( "\t\t samples should fit into.\n" );

	printf( "\t[--%s <minimum number of samples per node>=%f]\n" , SamplesPerNode.name, SamplesPerNode.value );
	printf( "\t\t This parameter specifies the minimum number of points that\n" );
	printf( "\t\t should fall within an octree node.\n" );

	printf( "\t[--%s <interpolation weight>=%f]\n" , PointWeight.name , PointWeight.value );
	printf( "\t\t This value specifies the weight that point interpolation constraints are\n" );
	printf( "\t\t given when defining the (screened) Poisson system.\n" );

	printf( "\t[--%s <iterations>=%d]\n" , Iters.name , Iters.value );
	printf( "\t\t This flag specifies the (maximum if CG) number of solver iterations.\n" );

#ifdef _OPENMP
	printf( "\t[--%s <num threads>=%d]\n" , Threads.name , Threads.value );
	printf( "\t\t This parameter specifies the number of threads across which\n" );
	printf( "\t\t the solver should be parallelized.\n" );
#endif // _OPENMP

	printf( "\t[--%s]\n" , Confidence.name );
	printf( "\t\t If this flag is enabled, the size of a sample's normals is\n" );
	printf( "\t\t used as a confidence value, affecting the sample's\n" );
	printf( "\t\t constribution to the reconstruction process.\n" );

	printf( "\t[--%s]\n" , NormalWeights.name );
	printf( "\t\t If this flag is enabled, the size of a sample's normals is\n" );
	printf( "\t\t used as to modulate the interpolation weight.\n" );

#if 0
	printf( "\t[--%s]\n" , NonManifold.name );
	printf( "\t\t If this flag is enabled, the isosurface extraction does not add\n" );
	printf( "\t\t a planar polygon's barycenter in order to ensure that the output\n" );
	printf( "\t\t mesh is manifold.\n" );
#endif

	printf( "\t[--%s]\n" , PolygonMesh.name);
	printf( "\t\t If this flag is enabled, the isosurface extraction returns polygons\n" );
	printf( "\t\t rather than triangles.\n" );

#if 0
	printf( "\t[--%s <minimum depth>=%d]\n" , MinDepth.name , MinDepth.value );
	printf( "\t\t This flag specifies the coarsest depth at which the system is to be solved.\n" );

	printf( "\t[--%s <cg solver accuracy>=%g]\n" , CSSolverAccuracy.name , CSSolverAccuracy.value );
	printf( "\t\t This flag specifies the accuracy cut-off to be used for CG.\n" );

	printf( "\t[--%s <adaptive weighting exponent>=%d]\n", AdaptiveExponent.name , AdaptiveExponent.value );
	printf( "\t\t This flag specifies the exponent scale for the adaptive weighting.\n" );

#ifdef _WIN32
	printf( "\t[--%s]\n" , Performance.name );
	printf( "\t\t If this flag is enabled, the running time and peak memory usage\n" );
	printf( "\t\t is output after the reconstruction.\n" );
#endif // _WIN32
#endif

	printf( "\t[--%s]\n" , Density.name );
	printf( "\t\t If this flag is enabled, the sampling density is written out with the vertices.\n" );

#if 0
	printf( "\t[--%s]\n" , ASCII.name );
	printf( "\t\t If this flag is enabled, the output file is written out in ASCII format.\n" );
	
	printf( "\t[--%s]\n" , NoComments.name );
	printf( "\t\t If this flag is enabled, the output file will not include comments.\n" );
#endif
	
	printf( "\t[--%s]\n" , Double.name );
	printf( "\t\t If this flag is enabled, the reconstruction will be performed with double-precision floats.\n" );

	printf( "\t[--%s]\n" , Verbose.name );
	printf( "\t\t If this flag is enabled, the progress of the reconstructor will be output to STDOUT.\n" );
}
template< class Real >
static XForm4x4< Real > loadXForm()
{
	XForm4x4< Real > xForm;
	if( XForm.set )
	{
		FILE* fp = fopen( XForm.value , "r" );
		if( !fp )
		{
			fprintf( stderr , "[WARNING] Could not read x-form from: %s\n" , XForm.value );
			xForm = XForm4x4< Real >::Identity();
		}
		else
		{
			int count = 0;
			for( int i=0 ; i<4 ; i++ ) for( int j=0 ; j<4 ; j++ )
			{
				float f;
				if( fscanf( fp , " %f " , &f )==1 ){ xForm(i,j) = static_cast<Real>(f); count++; }
			}
			if( count!=16 )
			{
				fprintf( stderr , "[WARNING] Could not read x-form from: %s (got %d values, expected 16)\n" , XForm.value , count );
				xForm = XForm4x4< Real >::Identity();
			}
			fclose( fp );
		}
	}
	else xForm = XForm4x4< Real >::Identity();
	return xForm;
}

static void buildComments( char** comments , int& commentNum , int paramNum )
{
	DumpOutput2( comments[commentNum++] , "Running Screened Poisson Reconstruction (Version 6.13)\n" );
	char str[1024];
	for( int i=0 ; i<paramNum ; i++ )
		if( params[i]->set )
		{
			params[i]->writeValue( str );
			if( strlen( str ) ) DumpOutput2( comments[commentNum++] , "\t--%s %s\n" , params[i]->name , str );
			else                DumpOutput2( comments[commentNum++] , "\t--%s\n" , params[i]->name );
		}
}

static int validateDepth( int kernelDepth , int depth )
{
	if( kernelDepth>depth )
	{
		fprintf( stderr,"[ERROR] %s can't be greater than %s: %d <= %d\n" , KernelDepth.name , Depth.name , kernelDepth , depth );
		return 0;
	}
	return 1;
}

static PointStream< float >* openPointStream( const char* path )
{
	char* ext = GetFileExtension( const_cast<char*>( path ) );
	PointStream< float >* stream;
	if     ( !strcasecmp( ext , "bnpts" ) ) stream = new BinaryPointStream< float >( path );
	else if( !strcasecmp( ext , "ply"   ) ) stream = new    PLYPointStream< float >( path );
	else                                    stream = new  ASCIIPointStream< float >( path );
	delete[] ext;
	return stream;
}

template< class Real >
static void writeVoxelGrid( Octree< Real >& tree , Pointer( Real ) solution , Real isoValue )
{
	double t = Time();
	FILE* fp = fopen( VoxelGrid.value , "wb" );
	if( !fp ) fprintf( stderr , "Failed to open voxel file for writing: %s\n" , VoxelGrid.value );
	else
	{
		int res;
		Pointer( Real ) values = tree.Evaluate( solution , res , isoValue , VoxelDepth.value );
		fwrite( &res , sizeof(int) , 1 , fp );
		if( sizeof(Real)==sizeof(float) ) fwrite( values , sizeof(float) , res*res*res , fp );
		else
		{
			float *fValues = new float[res*res*res];
			for( int i=0 ; i<res*res*res ; i++ ) fValues[i] = static_cast<float>( values[i] );
			fwrite( fValues , sizeof(float) , res*res*res , fp );
			delete[] fValues;
		}
		fclose( fp );
		DeletePointer( values );
	}
	DumpOutput( "Got voxel grid in: %f\n" , Time()-t );
}

template< class Real , class Vertex >
static void writeMesh( Octree< Real >& octree , CoredFileMeshData< Vertex >& mesh , const XForm4x4< Real >& iXForm , Pointer( Real ) solution , Real isoValue , std::vector< Real >* kernelDensityWeights , double& maxMemoryUsage , char** comments , int& commentNum , double& tt )
{
	double t = Time();
	octree.maxMemoryUsage = 0;
	octree.GetMCIsoSurface( kernelDensityWeights ? GetPointer( *kernelDensityWeights ) : NullPointer< Real >() , solution , isoValue , mesh , true , !NonManifold.set , PolygonMesh.set );
	if( PolygonMesh.set ) DumpOutput2( comments[commentNum++] , "#         Got polygons in: %9.1f (s), %9.1f (MB)\n" , Time()-t , octree.maxMemoryUsage );
	else                  DumpOutput2( comments[commentNum++] , "#        Got triangles in: %9.1f (s), %9.1f (MB)\n" , Time()-t , octree.maxMemoryUsage );
	maxMemoryUsage = std::max< double >( maxMemoryUsage , octree.maxMemoryUsage );
	DumpOutput2( comments[commentNum++],"#             Total Solve: %9.1f (s), %9.1f (MB)\n" , Time()-tt , maxMemoryUsage );

	if( NoComments.set )
	{
		if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , NULL , 0 , iXForm );
		else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , NULL , 0 , iXForm );
	}
	else
	{
		if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , const_cast<const char**>(comments) , commentNum , iXForm );
		else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , const_cast<const char**>(comments) , commentNum , iXForm );
	}
	DumpOutput( "Vertices / Polygons: %d / %d\n" , mesh.outOfCorePointCount()+static_cast<int>(mesh.inCorePoints.size()) , mesh.polygonCount() );
}

template< class Real , class Vertex >
int Execute( int argc , char* argv[] )
{
	Reset< Real >();
	int paramNum = sizeof(params)/sizeof(cmdLineReadable*);
	int commentNum=0;
	char **comments = new char*[paramNum+7];
	for( int i=0 ; i<paramNum+7 ; i++ ) comments[i] = new char[1024];

	if( Verbose.set ) echoStdout=1;

	XForm4x4< Real > xForm = loadXForm< Real >();
	XForm4x4< Real > iXForm = xForm.inverse();

	buildComments( comments , commentNum , paramNum );

	double tt=Time();
	Real isoValue = 0;

	Octree< Real > tree;
	tree.threads = Threads.value;
	if( !In.set ){ ShowUsage(argv[0]); return 0; }
	if( !MaxSolveDepth.set ) MaxSolveDepth.value = Depth.value;
	OctNode< TreeNodeData >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );

	int kernelDepth = KernelDepth.set ?  KernelDepth.value : Depth.value-2;
	if( !validateDepth( kernelDepth , Depth.value ) ) return EXIT_FAILURE;

	double maxMemoryUsage;
	double t=Time();
	tree.maxMemoryUsage=0;
	typename Octree< Real >::PointInfo* pointInfo = new typename Octree< Real >::PointInfo();
	typename Octree< Real >::NormalInfo* normalInfo = new typename Octree< Real >::NormalInfo();
	std::vector< Real >* kernelDensityWeights = new std::vector< Real >();
	std::vector< Real >* centerWeights = new std::vector< Real >();
	PointStream< float >* pointStream = openPointStream( In.value );
	int pointCount = tree.template SetTree< float >( pointStream , MinDepth.value , Depth.value , FullDepth.value , kernelDepth , static_cast<Real>(SamplesPerNode.value) , Scale.value , Confidence.set , NormalWeights.set , PointWeight.value , AdaptiveExponent.value , *pointInfo , *normalInfo , *kernelDensityWeights , *centerWeights , BoundaryType.value , xForm , Complete.set );
	delete pointStream;
	if( !Density.set ) delete kernelDensityWeights , kernelDensityWeights = NULL;

	DumpOutput2( comments[commentNum++] , "#             Tree set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
	DumpOutput( "Input Points: %d\n" , pointCount );
	DumpOutput( "Leaves/Nodes: %d/%d\n" , tree.tree.leaves() , tree.tree.nodes() );
	DumpOutput( "Memory Usage: %.3f MB\n" , static_cast<float>( MemoryInfo::Usage() )/(1<<20) );

	maxMemoryUsage = tree.maxMemoryUsage;
	t=Time() , tree.maxMemoryUsage=0;
	Pointer( Real ) constraints = tree.SetLaplacianConstraints( *normalInfo );
	delete normalInfo;
	DumpOutput2( comments[commentNum++] , "#      Constraints set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
	DumpOutput( "Memory Usage: %.3f MB\n" , static_cast<float>( MemoryInfo::Usage())/(1<<20) );
	maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

	t=Time() , tree.maxMemoryUsage=0;
	Pointer( Real ) solution = tree.SolveSystem( *pointInfo , constraints , ShowResidual.set , Iters.value , MaxSolveDepth.value , CGDepth.value , CSSolverAccuracy.value );
	delete pointInfo;
	FreePointer( constraints );
	DumpOutput2( comments[commentNum++] , "# Linear system solved in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
	DumpOutput( "Memory Usage: %.3f MB\n" , static_cast<float>( MemoryInfo::Usage() )/(1<<20) );
	maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

	CoredFileMeshData< Vertex > mesh;

	if( Verbose.set ) tree.maxMemoryUsage=0;
	t=Time();
	isoValue = tree.GetIsoValue( solution , *centerWeights );
	delete centerWeights;
	DumpOutput( "Got average in: %f\n" , Time()-t );
	DumpOutput( "Iso-Value: %e\n" , isoValue );

	if( VoxelGrid.set ) writeVoxelGrid< Real >( tree , solution , isoValue );
	if( Out.set ) writeMesh< Real , Vertex >( tree , mesh , iXForm , solution , isoValue , kernelDensityWeights , maxMemoryUsage , comments , commentNum , tt );

	FreePointer( solution );
	for( int i=0 ; i<paramNum+7 ; i++ ) delete[] comments[i];
	delete[] comments;
	return 1;
}

#ifdef _WIN32
inline double to_seconds( const FILETIME& ft )
{
	const double low_to_sec=100e-9; // 100 nanoseconds
	const double high_to_sec=low_to_sec*4294967296.0;
	return ft.dwLowDateTime*low_to_sec+ft.dwHighDateTime*high_to_sec;
}
#endif // _WIN32

int main( int argc , char* argv[] )
{
#if defined(WIN32) && defined(MAX_MEMORY_GB)
	if( MAX_MEMORY_GB>0 )
	{
		SIZE_T peakMemory = 1;
		peakMemory <<= 30;
		peakMemory *= MAX_MEMORY_GB;
		printf( "Limiting memory usage to %.2f GB\n" , static_cast<float>( peakMemory>>30 ) );
		HANDLE h = CreateJobObject( NULL , NULL );
		AssignProcessToJobObject( h , GetCurrentProcess() );

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_JOB_MEMORY;
		jeli.JobMemoryLimit = peakMemory;
		if( !SetInformationJobObject( h , JobObjectExtendedLimitInformation , &jeli , sizeof( jeli ) ) )
			fprintf( stderr , "Failed to set memory limit\n" );
	}
#endif // defined(WIN32) && defined(MAX_MEMORY_GB)
	double t = Time();

	cmdLineParse( argc-1 , &argv[1] , sizeof(params)/sizeof(cmdLineReadable*) , params , 1 );
	if( Density.set )
		if( Double.set ) Execute< double , PlyValueVertex< float > >( argc , argv );
		else             Execute< float  , PlyValueVertex< float > >( argc , argv );
	else
		if( Double.set ) Execute< double , PlyVertex< float > >( argc , argv );
		else             Execute< float  , PlyVertex< float > >( argc , argv );
#ifdef _WIN32
	if( Performance.set )
	{
		HANDLE cur_thread=GetCurrentThread();
		FILETIME tcreat, texit, tkernel, tuser;
		if( GetThreadTimes( cur_thread , &tcreat , &texit , &tkernel , &tuser ) )
			printf( "Time (Wall/User/Kernel): %.2f / %.2f / %.2f\n" , Time()-t , to_seconds( tuser ) , to_seconds( tkernel ) );
		else printf( "Time: %.2f\n" , Time()-t );
		HANDLE h = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		if( GetProcessMemoryInfo( h , &pmc , sizeof(pmc) ) ) printf( "Peak Memory (MB): %d\n" , pmc.PeakWorkingSetSize>>20 );
	}
#endif // _WIN32
	return EXIT_SUCCESS;
}
