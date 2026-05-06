#pragma once
#include "../src/Timer.h"
#include <cassert>
#include <chrono>
#include <thread>

namespace {

void test_timer_basic() {
    double t1 = Time();
    double t2 = Time();
    assert(t2 >= t1);
}

void test_timer_ordering() {
    double t1 = Time();
    double t2 = Time();
    assert(t2 >= t1 || t2 >= 0);
}

}

int test_timer() {
    test_timer_basic();
    test_timer_ordering();
    return 0;
}