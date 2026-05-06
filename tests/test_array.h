#pragma once
#include "../src/Array.h"
#include <cassert>
#include <cstring>
#include <cstdint>

namespace {

void test_aligned_malloc_basic() {
    void* mem = aligned_malloc(100, 16);
    assert(mem != nullptr);
    aligned_free(mem);
}

void test_aligned_malloc_size() {
    void* mem = aligned_malloc(1000, 32);
    assert(mem != nullptr);

    std::memset(mem, 0, 1000);

    aligned_free(mem);
}

void test_aligned_malloc_alignment() {
    void* mem = aligned_malloc(100, 64);
    assert(mem != nullptr);

    uintptr_t addr = reinterpret_cast<uintptr_t>(mem);
    assert((addr % 64) == 0);

    aligned_free(mem);
}

void test_aligned_malloc_zero_size() {
    void* mem = aligned_malloc(0, 16);
    assert(mem != nullptr);
    aligned_free(mem);
}

void test_aligned_malloc_various_alignment() {
    for (size_t align = 16; align <= 256; align *= 2) {
        void* mem = aligned_malloc(100, align);
        assert(mem != nullptr);

        uintptr_t addr = reinterpret_cast<uintptr_t>(mem);
        assert((addr % align) == 0);

        aligned_free(mem);
    }
}

}

int test_array() {
    test_aligned_malloc_basic();
    test_aligned_malloc_size();
    test_aligned_malloc_alignment();
    test_aligned_malloc_zero_size();
    test_aligned_malloc_various_alignment();
    return 0;
}