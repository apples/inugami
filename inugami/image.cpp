#include "image.hpp"

#include "math.hpp"

#include <libnoise/noise.h>
#include <png++/png.hpp>

#include <map>
#include <cmath>
#include <utility>

using namespace std;

namespace Inugami {

Image Image::fromGradient(int w, int h, const Pixel& a, float ax, float ay, const Pixel& b, float bx, float by) //static
{
    Image rval(w, h);

    float gx = bx-ax;
    float gy = by-ay;
    float mag = std::sqrt(gx*gx+gy*gy);
    gx /= mag;
    gy /= mag;

    for (int y=0; y<h; ++y)
    {
        float py = (float(y)/float(h-1))-ay;
        for (int x=0; x<w; ++x)
        {
            float px = (float(x)/float(w-1))-ax;
            float proj = clamp((px*gx+py*gy)/mag, 0.f, 1.f);
            for (int i=0; i<4; ++i) rval.pixelAt(x, y)[i] = a[i] + (b[i]-a[i])*proj;
        }
    }

    return rval;
}

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

Image Image::fromNoise(int w, int h, double freq) //static
{
    Image rval(w, h);
    noise::module::Perlin perlin;

    perlin.SetFrequency(freq);

    for (int i=0; i<w; ++i)
    {
        for (int j=0; j<h; ++j)
        {
            const double x = double(i)/double(w-1);
            const double y = double(j)/double(h-1);
            const double z = 0.0;

            for (int octave = 0; octave < 4; ++octave)
            {
                perlin.SetOctaveCount(octave+1);

                double val = perlin.GetValue(x,y,z);
                val = val*0.5 + 0.5;
                val = clamp(val, 0.0, 1.0);

                rval.pixelAt(j,i)[octave] = 255*val;
            }
        }
    }

    return rval;
}

Image::Image() :
    width(0), height(0),
    pixels(0)
{}

Image::Image(int w, int h, const Pixel& color) :
    width(w), height(h),
    pixels(w*h, color)
{}

Image::Image(const Image& in) :
    width(in.width), height(in.height),
    pixels(in.pixels)
{}

Image::Image(Image&& in) :
    width(in.width), height(in.height),
    pixels(move(in.pixels))
{}

Image& Image::operator=(const Image& in)
{
    width = in.width;
    height = in.height;
    pixels = in.pixels;
    return *this;
}

Image& Image::operator=(Image&& in)
{
    width = in.width;
    height = in.height;
    pixels = move(in.pixels);
    return *this;
}

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
    width = w;
    height = h;
    pixels.resize(width*height);
}

} // namespace Inugami
