#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include "mathtypes.h"

#include <cmath>

namespace Inugami
{

template <typename T>
T clamp(T x, T low, T high)
{
    if (x < low) return low;
    if (x >= high) return high;
    return x;
}

template <typename T>
T wrap(T x, T low, T high)
{
    T d = high-low;
    while (x < low) x+=d;
    while (x > high) x-=d;
    return x;
}

template <typename T>
T high(T x, T y)
{
    return (x>y)?x:y;
}

template <typename T>
T low(T x, T y)
{
    return (x>y)?y:x;
}

template <typename T>
T high(T x, T y, T z)
{
    return (x>y) ? ((x>z)?x:z) : ((y>z)?y:z);
}

template <typename T>
T low(T x, T y, T z)
{
    return (x>y) ? ((y>z)?z:y) : ((x>z)?z:y);
}

template <typename T>
int sgn(T x)
{
    return (T(0) < x) - (x < T(0));
}

template <typename T>
T toRadians(T x)
{
    static const double r = 1.74532925199433e-2; // pi/180
    return x*r;
}

template <typename T>
T toDegrees(T x)
{
    static const double r = 5.72957795130823e1; // 180/pi
    return x*r;
}

template <typename T>
T sin(T x)
{
    return std::sin(x);
}

template <typename T>
T cos(T x)
{
    return std::cos(x);
}

template <typename T>
T asin(T x)
{
    return std::asin(x);
}

template <typename T>
T acos(T x)
{
    return std::acos(x);
}

template <typename T>
T sind(T x)
{
    return std::sin(toRadians(x));
}

template <typename T>
T cosd(T x)
{
    return std::cos(toRadians(x));
}

} // namespace Inugami

#endif // MATH_H_INCLUDED
