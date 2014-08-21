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

#ifndef INUGAMI_PIXEL_HPP
#define INUGAMI_PIXEL_HPP

#include <array>

namespace Inugami {

namespace Resources_detail {

using SubPixel = unsigned char;

/*! @brief An RGBA8 pixel.
 *
 *  An array of 8-bit SubPixel%s: red, green, blue, and alpha.
 */
class Pixel
{
    using Data = std::array<SubPixel,4>;
    Data data;
public:
    enum Color
    {
          RED
        , GREEN
        , BLUE
        , ALPHA
    };

    Pixel() = default;
    Pixel(int h);
    Pixel(SubPixel r, SubPixel g, SubPixel b, SubPixel a);

    SubPixel& operator[](Data::size_type i) &;
    SubPixel& r() &;
    SubPixel& g() &;
    SubPixel& b() &;
    SubPixel& a() &;

    const SubPixel& operator[](Data::size_type i) const&;
    const SubPixel& r() const&;
    const SubPixel& g() const&;
    const SubPixel& b() const&;
    const SubPixel& a() const&;

    Pixel& operator+=(const Pixel& a);
    Pixel& operator-=(const Pixel& a);
    Pixel& operator*=(const Pixel& a);
    Pixel& operator/=(const Pixel& a);

    Pixel& operator*=(float f);
};

Pixel operator-(const Pixel& a);

Pixel operator+(const Pixel& a, const Pixel& b);
Pixel operator-(const Pixel& a, const Pixel& b);
Pixel operator*(const Pixel& a, const Pixel& b);
Pixel operator/(const Pixel& a, const Pixel& b);

Pixel operator*(const Pixel& a, float f);
Pixel operator*(float f, const Pixel& a);

} // namespace Resources_detail

} // namespace Inugami

#endif // INUGAMI_PIXEL_HPP
