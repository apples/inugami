#include "image.hpp"

#include <png++/png.hpp>

#include <utility>

using namespace std;

namespace Inugami {

Image::Image() :
    width(0), height(0),
    pixels(0)
{}

Image::Image(int w, int h) :
    width(w), height(h),
    pixels(w*h)
{}

Image::Image(const std::string& filename) :
    Image()
{
    png::image<Pixel> image(filename);

    width = image.get_width();
    height = image.get_height();
    pixels.resize(width*height);

    for (int r=0; r<height; ++r)
    {
        auto& row = image.get_row(height-r-1);
        std::copy(row.begin(), row.end(), pixels.begin()+r*width);
    }
}

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
