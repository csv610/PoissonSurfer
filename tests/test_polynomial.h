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

void test_polynomial_default_constructor() {
    Polynomial<3> p;
    assert(p.isZero());
}

void test_polynomial_is_zero() {
    Polynomial<2> p;
    assert(p.isZero());

    p.coefficients[0] = 1;
    assert(!p.isZero());
}

void test_polynomial_negate() {
    Polynomial<2> p;
    p.coefficients[0] = 1;
    p.coefficients[1] = 2;

    Polynomial<2> neg = -p;
    assert(std::abs(neg.coefficients[0] + 1.0) < 1e-10);
    assert(std::abs(neg.coefficients[1] + 2.0) < 1e-10);
}

void test_polynomial_multiply() {
    Polynomial<0> p1, p2;
    p1.coefficients[0] = 2;
    p2.coefficients[0] = 3;

    Polynomial<0> prod = p1 * p2;
    assert(std::abs(prod.coefficients[0] - 6.0) < 1e-10);
}

void test_polynomial_assignment() {
    Polynomial<2> p1, p2;
    p1.coefficients[0] = 1;
    p1.coefficients[1] = 2;
    p1.coefficients[2] = 3;

    p2 = p1;
    assert(std::abs(p2.coefficients[0] - 1.0) < 1e-10);
    assert(std::abs(p2.coefficients[1] - 2.0) < 1e-10);
    assert(std::abs(p2.coefficients[2] - 3.0) < 1e-10);
}

void test_polynomial_higher_degree() {
    Polynomial<3> p;
    p.coefficients[0] = 1;
    p.coefficients[1] = 2;
    p.coefficients[2] = 3;
    p.coefficients[3] = 4;

    double result = p(2.0);
    double expected = 1 + 2*2 + 3*4 + 4*8;
    assert(std::abs(result - expected) < 1e-10);
}

void test_polynomial_integral_value() {
    Polynomial<0> p;
    p.coefficients[0] = 5;

    double result = p.integral(0.0, 2.0);
    assert(std::abs(result - 10.0) < 1e-10);
}

void test_polynomial_polynomial_add() {
    Polynomial<2> p1, p2;
    p1.coefficients[0] = 1;
    p1.coefficients[1] = 2;

    p2.coefficients[0] = 3;
    p2.coefficients[1] = 4;

    p1 += p2;
    assert(std::abs(p1.coefficients[0] - 4.0) < 1e-10);
    assert(std::abs(p1.coefficients[1] - 6.0) < 1e-10);
}

void test_polynomial_polynomial_sub() {
    Polynomial<2> p1, p2;
    p1.coefficients[0] = 5;
    p1.coefficients[1] = 7;

    p2.coefficients[0] = 2;
    p2.coefficients[1] = 3;

    p1 -= p2;
    assert(std::abs(p1.coefficients[0] - 3.0) < 1e-10);
    assert(std::abs(p1.coefficients[1] - 4.0) < 1e-10);
}

void test_polynomial_scale_inplace() {
    Polynomial<2> p;
    p.coefficients[0] = 2;
    p.coefficients[1] = 4;

    p *= 0.5;
    assert(std::abs(p.coefficients[0] - 1.0) < 1e-10);
    assert(std::abs(p.coefficients[1] - 2.0) < 1e-10);
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
    test_polynomial_default_constructor();
    test_polynomial_is_zero();
    test_polynomial_negate();
    test_polynomial_multiply();
    test_polynomial_assignment();
    test_polynomial_higher_degree();
    test_polynomial_integral_value();
    test_polynomial_polynomial_add();
    test_polynomial_polynomial_sub();
    test_polynomial_scale_inplace();
    return 0;
}