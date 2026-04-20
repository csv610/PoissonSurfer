#pragma once
#include "../src/Vector.h"
#include <cmath>
#include <cassert>
#include <iostream>

namespace {

void test_vector_constructor() {
    Vector<double> v(5);
    assert(v.Dimensions() == 5);
}

void test_vector_setzero() {
    Vector<double> v(3);
    v.SetZero();
    assert(v(0) == 0.0);
    assert(v(1) == 0.0);
    assert(v(2) == 0.0);
}

void test_vector_operators() {
    Vector<double> v1(3), v2(3);
    v1(0) = 1; v1(1) = 2; v1(2) = 3;
    v2(0) = 4; v2(1) = 5; v2(2) = 6;

    Vector<double> sum = v1 + v2;
    assert(sum(0) == 5);
    assert(sum(1) == 7);
    assert(sum(2) == 9);

    Vector<double> diff = v2 - v1;
    assert(diff(0) == 3);
    assert(diff(1) == 3);
    assert(diff(2) == 3);

    Vector<double> scaled = v1 * 2.0;
    assert(scaled(0) == 2.0);
    assert(scaled(1) == 4.0);
    assert(scaled(2) == 6.0);
}

void test_vector_dot() {
    Vector<double> v1(3), v2(3);
    v1(0) = 1; v1(1) = 2; v1(2) = 3;
    v2(0) = 4; v2(1) = 5; v2(2) = 6;

    double dot = v1.Dot(v2);
    assert(std::abs(dot - 32.0) < 1e-10);
}

void test_vector_length() {
    Vector<double> v(3);
    v(0) = 3; v(1) = 4; v(2) = 0;

    double len = v.Length();
    assert(std::abs(len - 5.0) < 1e-10);
}

void test_vector_normalize() {
    Vector<double> v(3);
    v(0) = 3; v(1) = 4; v(2) = 0;
    v.Normalize();

    assert(std::abs(v.Length() - 1.0) < 1e-10);
    assert(std::abs(v(0) - 0.6) < 1e-10);
    assert(std::abs(v(1) - 0.8) < 1e-10);
}

}

int test_vector() {
    test_vector_constructor();
    test_vector_setzero();
    test_vector_operators();
    test_vector_dot();
    test_vector_length();
    test_vector_normalize();
    return 0;
}