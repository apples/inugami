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

#include "exception.hpp"
#include "math.hpp"

#include <png++/png.hpp>

#include <map>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <utility>

namespace Inugami {

class ImageE_OutOfBounds
    : public Exception
{
    std::string err;
public:
    ImageE_OutOfBounds(const Image& img, int x, int y)
        : err()
    {
        std::stringstream ss;
        ss << "Inugami::Image Exception: Accessing invalid pixel at ("
           << x << "," << y << ") in " << img.width << "x" << img.height
           << " image."
        ;
        err = ss.str();
    }

    virtual const char* what() const noexcept override
    {
        return err.c_str();
    }
};

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

Pixel& Image::at(int x, int y) &
{
    if (x<0 || x>=width || y<0 || y>=height)
    {
        throw ImageE_OutOfBounds(*this, x, y);
    }
    return pixels[y*width+x];
}

const Pixel& Image::at(int x, int y) const&
{
    if (x<0 || x>=width || y<0 || y>=height)
    {
        throw ImageE_OutOfBounds(*this, x, y);
    }
    return pixels[y*width+x];
}

Pixel* Image::operator[](int y) &
{
    return &pixels[y*width];
}

const Pixel* Image::operator[](int y) const&
{
    return &pixels[y*width];
}

void Image::resize(int w, int h)
{
    width  = w;
    height = h;
    pixels.resize(width*height);
}

Image blur(Image img)
{
    Image rval(img.width, img.height, {0, 0, 0, 0});

    double coeffs[3][3] = {
          {0.10, 0.10, 0.10}
        , {0.10, 0.20, 0.10}
        , {0.10, 0.10, 0.10}
    };

    auto pick = [&](int x, int y) -> Pixel&
    {
        if (x<0) x=0;
        else if (x>=img.width) x=img.width-1;
        if (y<0) y=0;
        else if (y>=img.height) y=img.height-1;
        return img.at(x, y);
    };

    for (int r=0; r<img.height; ++r)
    {
        for (int c=0; c<img.width; ++c)
        {
            auto&& p = rval.at(c, r);
            for (int rr=-1; rr<=1; ++rr)
            {
                for (int cc=-1; cc<=1; ++cc)
                {
                    p += pick(c+cc, r+rr) * coeffs[1+cc][1+rr];
                }
            }
        }
    }

    return rval;
}

Image amplify(Image img)
{
    class MinMax
    {
    public:
        MinMax()
            : min(-1)
            , max(0)
            , dist(0.0)
        {}

        void set(SubPixel p)
        {
            if (p < min) min = p;
            if (p > max) max = p;
        }

        void setdist()
        {
            if (max-min == 0) return;
            dist = 255.0/(max-min);
        }

        SubPixel min;
        SubPixel max;
        double dist;
    };

    MinMax red;
    MinMax green;
    MinMax blue;
    MinMax alpha;

    for (int r=0; r<img.height; ++r)
    {
        for (int c=0; c<img.width; ++c)
        {
            auto&& p = img.at(c, r);
            red  .set(p.r());
            green.set(p.g());
            blue .set(p.b());
            alpha.set(p.a());
        }
    }

    red  .setdist();
    green.setdist();
    blue .setdist();
    alpha.setdist();

    auto calc = [](const MinMax& mm, SubPixel& p)
    {
        if (mm.dist == 0.0) return;
        p = double(p-mm.min)/mm.dist;
    };

    for (int r=0; r<img.height; ++r)
    {
        for (int c=0; c<img.width; ++c)
        {
            auto&& p = img.at(c, r);
            calc(red  , p.r());
            calc(green, p.g());
            calc(blue , p.b());
            calc(alpha, p.a());
        }
    }

    return img;
}

} // namespace Inugami
