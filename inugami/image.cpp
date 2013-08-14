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

#include "image.hpp"

#include "math.hpp"

#include <png++/png.hpp>

#include <map>
#include <cmath>
#include <utility>
#include <random>

using namespace std;

namespace Inugami {

Image Image::fromPNG(const string& filename) //static
{
    Image rval;

    png::image<png::rgba_pixel> image(filename);

    rval.resize(image.get_width(), image.get_height());

    for (int r=0; r<rval.height; ++r)
    {
        for (int c=0; c<rval.width; ++c)
        {
            rval.pixelAt(c,r)[0] = image.get_pixel(c,rval.height-r-1).red;
            rval.pixelAt(c,r)[1] = image.get_pixel(c,rval.height-r-1).green;
            rval.pixelAt(c,r)[2] = image.get_pixel(c,rval.height-r-1).blue;
            rval.pixelAt(c,r)[3] = image.get_pixel(c,rval.height-r-1).alpha;
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
                rval.pixelAt(j,i)[k] = roll(rng);
            }
        }
    }

    return rval;
}

Image::Image(int w, int h, const Pixel& color)
    : width(w)
    , height(h)
    , pixels(w*h, color)
{}

Image::Pixel& Image::pixelAt(int x, int y)
{
    return pixels[y*width+x];
}

const Image::Pixel& Image::pixelAt(int x, int y) const
{
    return pixels[y*width+x];
}

void Image::resize(int w, int h)
{
    width  = w;
    height = h;
    pixels.resize(width*height);
}

} // namespace Inugami
