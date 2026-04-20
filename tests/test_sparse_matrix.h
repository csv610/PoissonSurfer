#pragma once
#include "../src/SparseMatrix.h"
#include <cmath>
#include <cassert>

namespace {

void test_sparse_matrix_constructor() {
    SparseMatrix<double> m(3);
    assert(m.rows == 3);
}

void test_sparse_matrix_resize() {
    SparseMatrix<double> m;
    m.Resize(3);
    assert(m.rows == 3);
}

void test_matrix_entry_constructor() {
    MatrixEntry<double> e1;
    assert(e1.N == -1);
    assert(e1.Value == 0.0);

    MatrixEntry<double> e2(5);
    assert(e2.N == 5);
    assert(e2.Value == 0.0);

    MatrixEntry<double> e3(3, 2.5);
    assert(e3.N == 3);
    assert(std::abs(e3.Value - 2.5) < 1e-10);
}

}

int test_sparse_matrix() {
    test_sparse_matrix_constructor();
    test_sparse_matrix_resize();
    test_matrix_entry_constructor();
    return 0;
}