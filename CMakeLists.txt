cmake_minimum_required(VERSION 3.10)
project(surfrecon LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)
find_package(assimp REQUIRED)

set(COMMON_SOURCES
    Src/CmdLineParser.cpp
    Src/Factor.cpp
    Src/Geometry.cpp
    Src/MarchingCubes.cpp
    Src/plyfile.cpp
)

add_executable(surfrecon
    ${COMMON_SOURCES}
    Src/PoissonRecon.cpp
)
target_include_directories(surfrecon PRIVATE Src)
target_link_libraries(surfrecon PRIVATE OpenMP::OpenMP_CXX)

add_executable(surftrimmer
    ${COMMON_SOURCES}
    Src/SurfaceTrimmer.cpp
)
target_include_directories(surftrimmer PRIVATE Src)
target_link_libraries(surftrimmer PRIVATE OpenMP::OpenMP_CXX)

add_executable(surfremesh
    ${COMMON_SOURCES}
    Src/surfremesh.cpp
)
target_include_directories(surfremesh PRIVATE Src)
target_link_libraries(surfremesh PRIVATE OpenMP::OpenMP_CXX assimp::assimp)

enable_testing()

set(TEST_SOURCES
    src/Geometry.cpp
    src/MarchingCubes.cpp
    src/Factor.cpp
)

add_executable(run_tests
    tests/test_runner.cpp
    tests/test_vector.h
    tests/test_polynomial.h
    tests/test_geometry.h
    tests/test_marching_cubes.h
    tests/test_ppolynomial.h
    tests/test_sparse_matrix.h
    tests/test_reconstruction.h tests/test_reconstruction_execute.h
    ${TEST_SOURCES}
)
target_include_directories(run_tests PRIVATE src)
target_link_libraries(run_tests PRIVATE OpenMP::OpenMP_CXX)
target_compile_definitions(run_tests PRIVATE POISSON_RECON_TESTING)

if(MSVC)
    target_compile_definitions(surfrecon PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(surftrimmer PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(run_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(surfrecon PRIVATE -Wno-deprecated)
    target_compile_options(surftrimmer PRIVATE -Wno-deprecated)
endif()

add_test(NAME AllTests COMMAND run_tests)
