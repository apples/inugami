/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.2.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

#include "mathtypes.hpp"

#include <cmath>
#include <type_traits>

namespace Inugami {

constexpr double PI = 3.141592653589793;

/*! @brief Clamps a value within a range.
 *
 *  @param x Value to clamp.
 *  @param low Minimum value.
 *  @param high Maximum value.
 *
 *  @return Clamped value.
 */
template <typename T>
T clamp(T x, T low, T high)
{
    if (x <  low)  return low;
    if (x >= high) return high;
    return x;
}

/*! @brief Wraps a value within a range.
 *
 *  @param x Value to wrap.
 *  @param low Minimum value.
 *  @param high Maximum value.
 *
 *  @return Wrapped value.
 */
template <typename T>
T wrap(T x, T low, T high)
{
    T d = high-low;
    while (x <  low) x+=d;
    while (x > high) x-=d;
    return x;
}

/*! @brief Linear interpolation.
 *
 *  @param a Value at @a t=0.
 *  @param b Value at @a t=1.
 *  @param t Time.
 *
 *  @return Result.
 */
template <typename T, typename U>
T lerp(T a, T b, U t)
{
    return (b-a)*t+a;
}

/*! @brief Choose highest.
 *
 *  @param x First value.
 *  @param y Second value.
 *
 *  @return Highest of @a x and @a y.
 */
template <typename T, typename U>
typename std::common_type<T, U>::type
high(T x, U y)
{
    return (x > y)? x : y;
}

/*! @brief Choose highest.
 *
 *  @param a First value.
 *  @param b Second value.
 *  @param args Other values.
 *
 *  @return Highest of all given values.
 */
template <typename T, typename U, typename... VT>
typename std::common_type<T, U, VT...>::type
high(T a, U b, VT... args)
{
    return high(high(a, b), args...);
}

/*! @brief Choose lowest.
 *
 *  @param x First value.
 *  @param y Second value.
 *
 *  @return Lowest of @a x and @a y.
 */
template <typename T, typename U>
typename std::common_type<T, U>::type
low(T x, U y)
{
    return (x > y)? y : x;
}

/*! @brief Choose lowest.
 *
 *  @param a First value.
 *  @param b Second value.
 *  @param args Other values.
 *
 *  @return Lowest of all given values.
 */
template <typename T, typename U, typename... VT>
typename std::common_type<T, U, VT...>::type
low(T a, U b, VT... args)
{
    return low(low(a, b), args...);
}

/*! @brief Determines the sign of a value.
 *
 *  @param x Value.
 *
 *  @return -1, 0, or 1.
 */
template <typename T>
int sgn(T x)
{
    return ((T{} < x)? 1 : 0) - ((x < T{})? 1 : 0);
}

/*! @brief Converts degrees to radians.
 *
 *  @param x Degrees.
 *
 *  @return Radians.
 */
template <typename T>
T toRadians(T x)
{
    constexpr double r = PI/180.0;
    return x*r;
}

/*! @brief Converts radians to degrees.
 *
 *  @param x Radians.
 *
 *  @return Degrees.
 */
template <typename T>
T toDegrees(T x)
{
    constexpr double r = 180.0/PI;
    return x*r;
}

/*! @brief Sine.
 *
 *  @param x Radians.
 *
 *  @return Sine value.
 */
template <typename T>
T sin(T x)
{
    return std::sin(x);
}

/*! @brief Cosine.
 *
 *  @param x Radians.
 *
 *  @return Cosine value.
 */
template <typename T>
T cos(T x)
{
    return std::cos(x);
}

/*! @brief Arcsine.
 *
 *  @param x Radians.
 *
 *  @return Arcsine value.
 */
template <typename T>
T asin(T x)
{
    return std::asin(x);
}

/*! @brief Arccosine.
 *
 *  @param x Radians.
 *
 *  @return Arccosine value.
 */
template <typename T>
T acos(T x)
{
    return std::acos(x);
}

/*! @brief Sine of degrees.
 *
 *  @param x Degrees.
 *
 *  @return Sine value.
 */
template <typename T>
T sind(T x)
{
    return std::sin(toRadians(x));
}

/*! @brief Cosine of degrees.
 *
 *  @param x Degrees.
 *
 *  @return Cosine value.
 */
template <typename T>
T cosd(T x)
{
    return std::cos(toRadians(x));
}

} // namespace Inugami

#endif // MATH_HPP_INCLUDED
