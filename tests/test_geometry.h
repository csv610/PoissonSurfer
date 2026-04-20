#pragma once
#include "../src/Geometry.h"
#include <cmath>
#include <cassert>

namespace {

void test_point3d_constructor() {
    Point3D<double> p;
    assert(p[0] == 0.0);
    assert(p[1] == 0.0);
    assert(p[2] == 0.0);
}

void test_point3d_constructor_with_values() {
    Point3D<double> p(1.0, 2.0, 3.0);
    assert(p[0] == 1.0);
    assert(p[1] == 2.0);
    assert(p[2] == 3.0);
}

void test_point3d_operators() {
    Point3D<double> p1(1.0, 2.0, 3.0);
    Point3D<double> p2(4.0, 5.0, 6.0);

    Point3D<double> sum = p1 + p2;
    assert(sum[0] == 5.0);
    assert(sum[1] == 7.0);
    assert(sum[2] == 9.0);

    Point3D<double> diff = p2 - p1;
    assert(diff[0] == 3.0);
    assert(diff[1] == 3.0);
    assert(diff[2] == 3.0);

    Point3D<double> scaled = p1 * 2.0;
    assert(scaled[0] == 2.0);
    assert(scaled[1] == 4.0);
    assert(scaled[2] == 6.0);
}

void test_point3d_dot() {
    Point3D<double> p1(1.0, 2.0, 3.0);
    Point3D<double> p2(4.0, 5.0, 6.0);

    double dot = Point3D<double>::Dot(p1, p2);
    assert(std::abs(dot - 32.0) < 1e-10);
}

void test_point3d_length() {
    Point3D<double> p(3.0, 4.0, 0.0);
    double len = Length(p);
    assert(std::abs(len - 5.0) < 1e-10);
}

void test_point3d_distance() {
    Point3D<double> p1(0.0, 0.0, 0.0);
    Point3D<double> p2(3.0, 4.0, 0.0);
    double dist = Distance(p1, p2);
    assert(std::abs(dist - 5.0) < 1e-10);
}

void test_xform3x3_identity() {
    XForm3x3<double> identity = XForm3x3<double>::Identity();

    assert(std::abs(identity(0,0) - 1.0) < 1e-10);
    assert(std::abs(identity(1,1) - 1.0) < 1e-10);
    assert(std::abs(identity(2,2) - 1.0) < 1e-10);
    assert(std::abs(identity(0,1)) < 1e-10);
}

void test_xform3x3_multiply() {
    XForm3x3<double> m = XForm3x3<double>::Identity();
    Point3D<double> p(1.0, 2.0, 3.0);
    Point3D<double> result = m * p;

    assert(std::abs(result[0] - 1.0) < 1e-10);
    assert(std::abs(result[1] - 2.0) < 1e-10);
    assert(std::abs(result[2] - 3.0) < 1e-10);
}

void test_xform3x3_transpose() {
    XForm3x3<double> m;
    m(0,1) = 1.0;
    m(1,0) = 2.0;

    XForm3x3<double> mt = m.transpose();
    assert(std::abs(mt(0,1) - 2.0) < 1e-10);
    assert(std::abs(mt(1,0) - 1.0) < 1e-10);
}

void test_xform3x3_determinant() {
    XForm3x3<double> identity = XForm3x3<double>::Identity();
    double det = identity.determinant();
    assert(std::abs(det - 1.0) < 1e-10);
}

void test_xform3x3_inverse() {
    XForm3x3<double> identity = XForm3x3<double>::Identity();
    XForm3x3<double> inv = identity.inverse();

    assert(std::abs(inv(0,0) - 1.0) < 1e-10);
    assert(std::abs(inv(1,1) - 1.0) < 1e-10);
    assert(std::abs(inv(2,2) - 1.0) < 1e-10);
}

void test_xform4x4_identity() {
    XForm4x4<double> identity = XForm4x4<double>::Identity();

    assert(std::abs(identity(0,0) - 1.0) < 1e-10);
    assert(std::abs(identity(1,1) - 1.0) < 1e-10);
    assert(std::abs(identity(2,2) - 1.0) < 1e-10);
    assert(std::abs(identity(3,3) - 1.0) < 1e-10);
}

void test_xform4x4_multiply() {
    XForm4x4<double> m = XForm4x4<double>::Identity();
    Point3D<double> p(1.0, 2.0, 3.0);
    Point3D<double> result = m * p;

    assert(std::abs(result[0] - 1.0) < 1e-10);
    assert(std::abs(result[1] - 2.0) < 1e-10);
    assert(std::abs(result[2] - 3.0) < 1e-10);
}

void test_cross_product() {
    Point3D<double> p1(1.0, 0.0, 0.0);
    Point3D<double> p2(0.0, 1.0, 0.0);
    Point3D<double> result;

    CrossProduct(p1, p2, result);

    assert(std::abs(result[0] - 0.0) < 1e-10);
    assert(std::abs(result[1] - 0.0) < 1e-10);
    assert(std::abs(result[2] - 1.0) < 1e-10);
}

void test_square_length() {
    Point3D<double> p(3.0, 4.0, 0.0);
    double sqLen = SquareLength(p);
    assert(std::abs(sqLen - 25.0) < 1e-10);
}

void test_square_distance() {
    Point3D<double> p1(0.0, 0.0, 0.0);
    Point3D<double> p2(3.0, 4.0, 0.0);
    double sqDist = SquareDistance(p1, p2);
    assert(std::abs(sqDist - 25.0) < 1e-10);
}

}

int test_geometry() {
    test_point3d_constructor();
    test_point3d_constructor_with_values();
    test_point3d_operators();
    test_point3d_dot();
    test_point3d_length();
    test_point3d_distance();
    test_xform3x3_identity();
    test_xform3x3_multiply();
    test_xform3x3_transpose();
    test_xform3x3_determinant();
    test_xform3x3_inverse();
    test_xform4x4_identity();
    test_xform4x4_multiply();
    test_cross_product();
    test_square_length();
    test_square_distance();
    return 0;
}