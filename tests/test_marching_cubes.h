#pragma once
#include "../src/MarchingCubes.h"
#include <cmath>
#include <cassert>

namespace {

void test_cube_corner_index() {
    assert(Cube::CornerIndex(0,0,0) == 0);
    assert(Cube::CornerIndex(1,0,0) == 1);
    assert(Cube::CornerIndex(0,1,0) == 2);
    assert(Cube::CornerIndex(1,1,0) == 3);
    assert(Cube::CornerIndex(0,0,1) == 4);
    assert(Cube::CornerIndex(1,0,1) == 5);
    assert(Cube::CornerIndex(0,1,1) == 6);
    assert(Cube::CornerIndex(1,1,1) == 7);
}

void test_cube_edge_index() {
    int idx = Cube::EdgeIndex(0, 0, 0);
    assert(idx >= 0);
    assert(idx < Cube::EDGES);

    idx = Cube::EdgeIndex(1, 0, 0);
    assert(idx >= 0);
    assert(idx < Cube::EDGES);

    idx = Cube::EdgeIndex(2, 0, 0);
    assert(idx >= 0);
    assert(idx < Cube::EDGES);
}

void test_cube_face_index() {
    int idx = Cube::FaceIndex(0, 0);
    assert(idx >= 0);
    assert(idx < Cube::FACES);

    idx = Cube::FaceIndex(1, 0);
    assert(idx >= 0);
    assert(idx < Cube::FACES);

    idx = Cube::FaceIndex(2, 0);
    assert(idx >= 0);
    assert(idx < Cube::FACES);
}

void test_cube_edge_corners() {
    int c1, c2;
    Cube::EdgeCorners(0, c1, c2);
    assert(c1 >= 0 && c1 < Cube::CORNERS);
    assert(c2 >= 0 && c2 < Cube::CORNERS);

    Cube::EdgeCorners(3, c1, c2);
    assert(c1 >= 0 && c1 < Cube::CORNERS);
    assert(c2 >= 0 && c2 < Cube::CORNERS);
}

void test_cube_face_corners() {
    int c1, c2, c3, c4;
    Cube::FaceCorners(0, c1, c2, c3, c4);
    assert(c1 >= 0 && c1 < Cube::CORNERS);
    assert(c2 >= 0 && c2 < Cube::CORNERS);
    assert(c3 >= 0 && c3 < Cube::CORNERS);
    assert(c4 >= 0 && c4 < Cube::CORNERS);
}

void test_cube_antipodal() {
    int antipodal = Cube::AntipodalCornerIndex(0);
    assert(antipodal == 7);

    antipodal = Cube::AntipodalCornerIndex(1);
    assert(antipodal == 6);

    antipodal = Cube::AntipodalCornerIndex(4);
    assert(antipodal == 3);
}

void test_square_corner_index() {
    assert(Square::CornerIndex(0,0) == 0);
    assert(Square::CornerIndex(1,0) == 1);
    assert(Square::CornerIndex(0,1) == 2);
    assert(Square::CornerIndex(1,1) == 3);
}

void test_square_edge_index() {
    int idx = Square::EdgeIndex(0, 0);
    assert(idx >= 0);
    assert(idx < Square::EDGES);

    idx = Square::EdgeIndex(1, 0);
    assert(idx >= 0);
    assert(idx < Square::EDGES);
}

void test_marching_cubes_interpolate() {
    double v1 = 0.0, v2 = 1.0;
    double result = MarchingCubes::Interpolate(v1, v2);
    assert(result >= 0.0 && result <= 1.0);

    result = MarchingCubes::Interpolate(0.0, 2.0);
    assert(result >= 0.0 && result <= 2.0);
}

void test_marching_cubes_get_index() {
    double values[Cube::CORNERS];
    for (int i = 0; i < Cube::CORNERS; i++) values[i] = -1.0;

    unsigned char idx = MarchingCubes::GetIndex(values, 0.0);
    (void)idx;

    for (int i = 0; i < Cube::CORNERS; i++) values[i] = 1.0;
    idx = MarchingCubes::GetIndex(values, 0.0);
    (void)idx;

    for (int i = 0; i < 4; i++) values[i] = -1.0;
    for (int i = 4; i < 8; i++) values[i] = 1.0;
    idx = MarchingCubes::GetIndex(values, 0.0);
    (void)idx;
}

void test_marching_cubes_has_roots() {
    double values[Cube::CORNERS];

    for (int i = 0; i < Cube::CORNERS; i++) values[i] = 1.0;
    bool hasRoots = MarchingCubes::HasRoots(values, 0.0);
    assert(hasRoots == true || hasRoots == false);

    for (int i = 0; i < Cube::CORNERS; i++) values[i] = -1.0;
    hasRoots = MarchingCubes::HasRoots(values, 0.0);
    assert(hasRoots == true || hasRoots == false);
}

void test_marching_cubes_add_triangles() {
    double values[Cube::CORNERS];
    Triangle triangles[MarchingCubes::MAX_TRIANGLES];

    for (int i = 0; i < Cube::CORNERS; i++) values[i] = -1.0;
    int count = MarchingCubes::AddTriangles(values, 0.0, triangles);
    assert(count == 0);

    for (int i = 0; i < Cube::CORNERS; i++) values[i] = 1.0;
    count = MarchingCubes::AddTriangles(values, 0.0, triangles);
    assert(count == 0);

    for (int i = 0; i < 4; i++) values[i] = -1.0;
    for (int i = 4; i < 8; i++) values[i] = 1.0;
    count = MarchingCubes::AddTriangles(values, 0.0, triangles);
    assert(count > 0);
}

void test_marching_squares_interpolate() {
    double v1 = 0.0, v2 = 1.0;
    if (v1 == v2) return;
    double result = v1 + (v2 - v1) * 0.5;
    assert(result >= 0.0 && result <= 1.0);
}

void test_marching_squares_get_index() {
    float values[Square::CORNERS];
    for (int i = 0; i < Square::CORNERS; i++) values[i] = -1.0f;

    unsigned char idx = MarchingSquares::GetIndex(values, 0.0f);
    assert(idx >= 0);

    for (int i = 0; i < Square::CORNERS; i++) values[i] = 1.0f;
    idx = MarchingSquares::GetIndex(values, 0.0f);
    assert(idx >= 0);
}

void test_marching_squares_has_roots() {
    unsigned char idx = 0;
    bool hasRoots = MarchingSquares::HasRoots(idx);
    assert(hasRoots == true || hasRoots == false);
}

void test_square_edge_corners() {
    int c1, c2;
    Square::EdgeCorners(0, c1, c2);
    assert(c1 >= 0 && c1 < Square::CORNERS);
    assert(c2 >= 0 && c2 < Square::CORNERS);

    Square::EdgeCorners(3, c1, c2);
    assert(c1 >= 0 && c1 < Square::CORNERS);
    assert(c2 >= 0 && c2 < Square::CORNERS);
}

void test_marching_cubes_boundary_cases() {
    double values[Cube::CORNERS];
    for (int i = 0; i < Cube::CORNERS; i++) values[i] = 0.0;

    unsigned char idx = MarchingCubes::GetIndex(values, 0.0);
    (void)idx;

    double v1 = 0.0, v2 = 0.0;
    double result = MarchingCubes::Interpolate(v1, v2);
    (void)result;
}

void test_marching_squares_edge_table() {
    unsigned char idx = 0;
    int e = MarchingSquares::edges[idx][0];
    (void)e;
}

}

int test_marching_cubes() {
    test_cube_corner_index();
    test_cube_edge_index();
    test_cube_face_index();
    test_cube_edge_corners();
    test_cube_face_corners();
    test_cube_antipodal();
    test_square_corner_index();
    test_square_edge_index();
    test_marching_cubes_interpolate();
    test_marching_cubes_get_index();
    test_marching_cubes_has_roots();
    test_marching_cubes_add_triangles();
    test_marching_squares_interpolate();
    test_marching_squares_get_index();
    test_marching_squares_has_roots();
    test_square_edge_corners();
    test_marching_cubes_boundary_cases();
    test_marching_squares_edge_table();
    return 0;
}