#ifndef UTILS_H
#define UTILS_H

#include <random>

inline std::mt19937 &randg() {
    static thread_local std::mt19937 r = std::mt19937(std::random_device()());

    return r;
}

#endif // UTILS_H
