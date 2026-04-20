#include "test_vector.h"
#include "test_polynomial.h"
#include <iostream>

int main() {
    std::cout << "Running Vector tests..." << std::endl;
    if (test_vector() != 0) {
        std::cerr << "Vector tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Vector tests PASSED" << std::endl;

    std::cout << "Running Polynomial tests..." << std::endl;
    if (test_polynomial() != 0) {
        std::cerr << "Polynomial tests FAILED" << std::endl;
        return 1;
    }
    std::cout << "Polynomial tests PASSED" << std::endl;

    std::cout << "All tests PASSED" << std::endl;
    return 0;
}