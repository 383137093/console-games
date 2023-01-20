#ifndef ASHES_RANDOM_H
#define ASHES_RANDOM_H

#include <ctime>
#include <random>
#include <type_traits>

namespace ashes { namespace random {

inline void ResetSeed()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

inline float Uniform()
{
    return static_cast<float>(std::rand()) / RAND_MAX;
}

inline int IntRange(int a, int b)
{
    return static_cast<int>(Uniform() * (b - a) + 0.5f) + a;
}

inline bool Bool()
{
    return (std::rand() & 1) == 1;
}

inline bool Probability(float uniform)
{
    if (uniform <= 0.0f) { return false; }
    if (uniform >= 1.0f) { return true; }
    return Uniform() < uniform;
}

}}

#endif