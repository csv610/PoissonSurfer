cmake_minimum_required(VERSION 3.10)
project(PoissonRecon LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

set(COMMON_SOURCES
    Src/CmdLineParser.cpp
    Src/Factor.cpp
    Src/Geometry.cpp
    Src/MarchingCubes.cpp
    Src/plyfile.cpp
)

add_executable(PoissonRecon
    ${COMMON_SOURCES}
    Src/PoissonRecon.cpp
)
target_include_directories(PoissonRecon PRIVATE Src)
target_link_libraries(PoissonRecon PRIVATE OpenMP::OpenMP_CXX)

add_executable(SurfaceTrimmer
    ${COMMON_SOURCES}
    Src/SurfaceTrimmer.cpp
)
target_include_directories(SurfaceTrimmer PRIVATE Src)
target_link_libraries(SurfaceTrimmer PRIVATE OpenMP::OpenMP_CXX)

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
    tests/test_reconstruction.h
    ${TEST_SOURCES}
)
target_include_directories(run_tests PRIVATE src)
target_link_libraries(run_tests PRIVATE OpenMP::OpenMP_CXX)
target_compile_definitions(run_tests PRIVATE POISSON_RECON_TESTING)

if(MSVC)
    target_compile_definitions(PoissonRecon PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(SurfaceTrimmer PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(run_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(PoissonRecon PRIVATE -Wno-deprecated)
    target_compile_options(SurfaceTrimmer PRIVATE -Wno-deprecated)
endif()

add_test(NAME AllTests COMMAND run_tests)
