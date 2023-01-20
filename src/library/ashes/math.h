#ifndef ASHES_MATH_H
#define ASHES_MATH_H

namespace ashes { namespace math {

template <class T>
T Lerp(const T& a, const T& b, float t)
{
    return static_cast<T>(a + (b - a) * t);
}

template <class T>
T Clamp(const T& x, const T& low, const T& high)
{
    return x < low ? low : (x > high ? high : x);
}

}}

#endif