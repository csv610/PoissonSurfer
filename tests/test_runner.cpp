#include "test_vector.h"
#include "test_polynomial.h"
#include "test_geometry.h"
#include "test_marching_cubes.h"
#include "test_ppolynomial.h"
#include "test_sparse_matrix.h"
#include "test_reconstruction.h"
#include "test_reconstruction_execute.h"
#include "test_timer.h"
#include "test_array.h"
#include <iostream>

int main() {
    std::cout << "=== Running Vector tests ===" << std::endl;
    if (test_vector() != 0) {
        std::cerr << "Vector tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Vector tests PASSED" << std::endl;

    std::cout << "=== Running Polynomial tests ===" << std::endl;
    if (test_polynomial() != 0) {
        std::cerr << "Polynomial tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Polynomial tests PASSED" << std::endl;

    std::cout << "=== Running Geometry tests ===" << std::endl;
    if (test_geometry() != 0) {
        std::cerr << "Geometry tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Geometry tests PASSED" << std::endl;

    std::cout << "=== Running Marching Cubes tests ===" << std::endl;
    if (test_marching_cubes() != 0) {
        std::cerr << "Marching Cubes tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Marching Cubes tests PASSED" << std::endl;

    std::cout << "=== Running PPolynomial tests ===" << std::endl;
    if (test_ppolynomial() != 0) {
        std::cerr << "PPolynomial tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "PPolynomial tests PASSED" << std::endl;

    std::cout << "=== Running Sparse Matrix tests ===" << std::endl;
    if (test_sparse_matrix() != 0) {
        std::cerr << "Sparse Matrix tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Sparse Matrix tests PASSED" << std::endl;

    std::cout << "=== Running Reconstruction tests ===" << std::endl;
    if (test_reconstruction() != 0) {
        std::cerr << "Reconstruction tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Reconstruction tests PASSED" << std::endl;

    std::cout << "=== Running Reconstruction method (CLI) tests ===" << std::endl;
    if (test_reconstruction_method() != 0) {
        std::cerr << "Reconstruction method tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Reconstruction method tests PASSED" << std::endl;

    std::cout << "=== Running Timer tests ===" << std::endl;
    if (test_timer() != 0) {
        std::cerr << "Timer tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Timer tests PASSED" << std::endl;

    std::cout << "=== Running Array tests ===" << std::endl;
    if (test_array() != 0) {
        std::cerr << "Array tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Array tests PASSED" << std::endl;

    std::cout << "=== All tests PASSED ===" << std::endl;
    return 0;
}
