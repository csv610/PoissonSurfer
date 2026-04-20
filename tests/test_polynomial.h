#pragma once
#include "../src/Polynomial.h"
#include <cmath>
#include <cassert>

namespace {

void test_polynomial_constructor() {
    Polynomial<2> p;
    assert(p.isZero());
}

void test_polynomial_evaluation() {
    Polynomial<2> p;
    p.coefficients[0] = 1;
    p.coefficients[1] = 2;
    p.coefficients[2] = 3;

    double result = p(1.0);
    assert(std::abs(result - (1 + 2 + 3)) < 1e-10);
}

void test_polynomial_add() {
    Polynomial<2> p1, p2;
    p1.coefficients[0] = 1;
    p2.coefficients[0] = 2;

    Polynomial<2> sum = p1 + p2;
    assert(std::abs(sum.coefficients[0] - 3) < 1e-10);
}

void test_polynomial_subtract() {
    Polynomial<2> p1, p2;
    p1.coefficients[0] = 5;
    p2.coefficients[0] = 2;

    Polynomial<2> diff = p1 - p2;
    assert(std::abs(diff.coefficients[0] - 3) < 1e-10);
}

void test_polynomial_scale() {
    Polynomial<2> p;
    p.coefficients[0] = 1;
    p.coefficients[1] = 2;

    Polynomial<2> scaled = p * 2.0;
    assert(std::abs(scaled.coefficients[0] - 2) < 1e-10);
    assert(std::abs(scaled.coefficients[1] - 4) < 1e-10);
}

void test_polynomial_integral() {
    Polynomial<0> p;
    p.coefficients[0] = 2;

    double result = p.integral(0.0, 1.0);
    assert(std::abs(result - 2.0) < 1e-10);
}

void test_polynomial_derivative() {
    Polynomial<2> p;
    p.coefficients[0] = 0;
    p.coefficients[1] = 2;
    p.coefficients[2] = 3;

    Polynomial<1> dp = p.derivative();
    assert(std::abs(dp.coefficients[0] - 2) < 1e-10);
    assert(std::abs(dp.coefficients[1] - 6) < 1e-10);
}

}

int test_polynomial() {
    test_polynomial_constructor();
    test_polynomial_evaluation();
    test_polynomial_add();
    test_polynomial_subtract();
    test_polynomial_scale();
    test_polynomial_integral();
    test_polynomial_derivative();
    return 0;
}