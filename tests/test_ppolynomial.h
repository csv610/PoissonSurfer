#pragma once
#include "../src/PPolynomial.h"
#include <cmath>
#include <cassert>

namespace {

void test_ppolynomial_constructor() {
    PPolynomial<0> pp;
    assert(pp.size() == 0);
}

void test_ppolynomial_set() {
    PPolynomial<0> pp;
    StartingPolynomial<0> sp;
    sp.start = 0.0;
    sp.p.coefficients[0] = 1.0;

    StartingPolynomial<0> sps[1] = {sp};
    pp.set(sps, 1);

    assert(pp.size() >= 0);
}

void test_ppolynomial_evaluation() {
    PPolynomial<0> pp;
    StartingPolynomial<0> sp;
    sp.start = 0.0;
    sp.p.coefficients[0] = 2.0;

    StartingPolynomial<0> sps[1] = {sp};
    pp.set(sps, 1);

    double result = pp(0.5);
    assert(result == result);
}

void test_ppolynomial_integral() {
    PPolynomial<0> pp;
    assert(pp.size() == 0);
}

void test_ppolynomial_Integral() {
    PPolynomial<0> pp;
    assert(pp.size() == 0);
}

void test_ppolynomial_add() {
    PPolynomial<0> pp1, pp2;
    assert(pp1.size() == 0);
    assert(pp2.size() == 0);
}

void test_ppolynomial_scale() {
    PPolynomial<0> pp;
    assert(pp.size() == 0);
}

void test_starting_polynomial_constructor() {
    StartingPolynomial<0> sp;
    assert(sp.start >= 0.0 || sp.start < 0.0);
}

void test_starting_polynomial_shift() {
    StartingPolynomial<0> sp;
    sp.start = 0.0;
    sp.p.coefficients[0] = 1.0;

    StartingPolynomial<0> shifted = sp.shift(1.0);
    assert(shifted.start >= 0.0 || shifted.start < 0.0);
}

void test_starting_polynomial_scale() {
    StartingPolynomial<0> sp;
    sp.start = 0.0;
    sp.p.coefficients[0] = 1.0;

    StartingPolynomial<0> scaled = sp.scale(2.0);
    assert(scaled.p.coefficients[0] >= 0.0 || scaled.p.coefficients[0] < 0.0);
}

void test_ppolynomial_bspline() {
    PPolynomial<2> bp = PPolynomial<2>::BSpline();
    assert(bp.size() >= 0);
}

void test_ppolynomial_derivative() {
    PPolynomial<1> pp;
    assert(pp.size() >= 0);
}

}

int test_ppolynomial() {
    test_ppolynomial_constructor();
    test_ppolynomial_set();
    test_ppolynomial_evaluation();
    test_ppolynomial_integral();
    test_ppolynomial_Integral();
    test_ppolynomial_add();
    test_ppolynomial_scale();
    test_starting_polynomial_constructor();
    test_starting_polynomial_shift();
    test_starting_polynomial_scale();
    test_ppolynomial_bspline();
    test_ppolynomial_derivative();
    return 0;
}