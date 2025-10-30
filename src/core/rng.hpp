#pragma once
#include <random>

inline int rng_int(int lo, int hi) {
    static thread_local std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(gen);
}
