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

#include "image.hpp"

#include <png++/png.hpp>

#include <cmath>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace Inugami {

namespace Resources_detail {

Image Image::fromPNG(const std::string& filename) //static
{
    Image rval;

    png::image<png::rgba_pixel> image(filename);

    rval.width = image.get_width();
    rval.height = image.get_height();
    rval.pixels.clear();
    rval.pixels.reserve(rval.width*rval.height);

    for (int r=0; r<rval.height; ++r)
    {
        for (int c=0; c<rval.width; ++c)
        {
            auto&& p = image.get_pixel(c, rval.height-r-1);
            rval.pixels.emplace_back(p.red, p.green, p.blue, p.alpha);
        }
    }

    return rval;
}

Image Image::fromNoise(int w, int h) //static
{
    Image rval(w, h);

    std::mt19937 rng;
    std::uniform_int_distribution<int> roll(0,255);

    for (int i=0; i<w; ++i)
    {
        for (int j=0; j<h; ++j)
        {
            for (int k=0; k<4; ++k)
            {
                rval.at(j,i)[k] = roll(rng);
            }
        }
    }

    return rval;
}

Image::Image(int w, int h)
    : width(w)
    , height(h)
    , pixels(w*h, {255, 255, 255, 255})
{}

Image::Image(int w, int h, const Pixel& color)
    : width(w)
    , height(h)
    , pixels(w*h, color)
{}

Pixel& Image::at(int x, int y)
{
    if (x<0 || x>=width || y<0 || y>=height)
    {
        throw std::out_of_range("Image::at(): Out of range!");
    }
    return pixels[y*width+x];
}

const Pixel& Image::at(int x, int y) const
{
    if (x<0 || x>=width || y<0 || y>=height)
    {
        throw std::out_of_range("Image::at(): Out of range!");
    }
    return pixels[y*width+x];
}

Image::Row Image::operator[](int y)
{
    return &pixels[y*width];
}

Image::CRow Image::operator[](int y) const
{
    return &pixels[y*width];
}

void Image::resize(int w, int h)
{
    width  = w;
    height = h;
    pixels.resize(width*height);
}

int Image::getWidth() const
{
    return width;
}

int Image::getHeight() const
{
    return height;
}

} // namespace Resources_detail

} // namespace Inugami
