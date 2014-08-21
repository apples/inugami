/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
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

#include "pixel.hpp"

#include <glm/glm.hpp>

namespace Inugami {

namespace Resources_detail {

Pixel::Pixel(int h)
    : Pixel((h/0xffffff), (h/0xffff)%0xff, (h/0xff)%0xff, h%0xff)
{}

Pixel::Pixel(SubPixel r, SubPixel g, SubPixel b, SubPixel a)
    : data{r, g, b, a}
{}

SubPixel& Pixel::operator[](Data::size_type i) &
{
    return data[i];
}

SubPixel& Pixel::r() &
{
    return data[RED];
}

SubPixel& Pixel::g() &
{
    return data[GREEN];
}

SubPixel& Pixel::b() &
{
    return data[BLUE];
}

SubPixel& Pixel::a() &
{
    return data[ALPHA];
}

const SubPixel& Pixel::operator[](Data::size_type i) const&
{
    return data[i];
}

const SubPixel& Pixel::r() const&
{
    return data[RED];
}

const SubPixel& Pixel::g() const&
{
    return data[GREEN];
}

const SubPixel& Pixel::b() const&
{
    return data[BLUE];
}

const SubPixel& Pixel::a() const&
{
    return data[ALPHA];
}

Pixel& Pixel::operator+=(const Pixel& a)
{
    auto op = [&](int i)
    {
        data[i] = std::max(data[i]+a[i], 255);
    };
    for (int i=0; i<4; ++i) op(i);
    return *this;
}

Pixel& Pixel::operator-=(const Pixel& a)
{
    auto op = [&](int i)
    {
        data[i] = std::min(data[i]-a[i], 0);
    };
    for (int i=0; i<4; ++i) op(i);
    return *this;
}

Pixel& Pixel::operator*=(const Pixel& a)
{
    auto op = [&](int i)
    {
        data[i] = (data[i]*a[i])/255;
    };
    for (int i=0; i<4; ++i) op(i);
    return *this;
}

Pixel& Pixel::operator/=(const Pixel& a)
{
    auto op = [&](int i)
    {
        if (a[i] == 0) data[i] = 255;
        else data[i] = glm::clamp((data[i]*255)/a[i], 0, 255);
    };
    for (int i=0; i<4; ++i) op(i);
    return *this;
}

Pixel& Pixel::operator*=(float f)
{
    auto op = [&](int i)
    {
        data[i] = glm::clamp(int(data[i]*f), 0, 255);
    };
    for (int i=0; i<4; ++i) op(i);
    return *this;
}

Pixel operator-(const Pixel& a)
{
    auto op = [&](int i)
    {
        return 255-a[i];
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator+(const Pixel& a, const Pixel& b)
{
    auto op = [&](int i)
    {
        return a[i]+b[i];
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator-(const Pixel& a, const Pixel& b)
{
    auto op = [&](int i)
    {
        return a[i]-b[i];
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator*(const Pixel& a, const Pixel& b)
{
    auto op = [&](int i)
    {
        return (a[i]*b[i])/255;
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator/(const Pixel& a, const Pixel& b)
{
    auto op = [&](int i)
    {
        return (a[i]*255)/b[i];
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator*(const Pixel& a, float f)
{
    auto op = [&](int i)
    {
        return a[i]*f;
    };
    return Pixel(op(0), op(1), op(2), op(3));
}

Pixel operator*(float f, const Pixel& a)
{
    return a*f;
}

} // namespace Resources_detail

} // namespace Inugami
