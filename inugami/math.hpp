/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

#include "mathtypes.hpp"

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

template <typename T, typename U>
T lerp(T a, T b, U c)
{
    return (b-a)*c+a;
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
    return ((T(0) < x)?1:0) - ((x < T(0))?1:0);
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
    return std::sin(float(x));
}

template <typename T>
T cos(T x)
{
    return std::cos(float(x));
}

template <typename T>
T asin(T x)
{
    return std::asin(float(x));
}

template <typename T>
T acos(T x)
{
    return std::acos(float(x));
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

#endif // MATH_HPP_INCLUDED
