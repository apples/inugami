#ifndef INUGAMI_IMAGE_H
#define INUGAMI_IMAGE_H

#include "utility.hpp"

#include <png++/png.hpp>

#include <array>
#include <string>
#include <vector>

namespace Inugami {

class Image
{
public:
    using Pixel = std::array<unsigned char,4>;
    using SubPixel = unsigned char;

    static Image fromGradient(int w, int h, const Pixel& a, float ax, float ay, const Pixel& b, float bx, float by);
    static Image fromPNG(const std::string& filename);
    static Image fromNoise(int w, int h, double freq);

    Image();
    Image(int w, int h, const Pixel& color = Pixel{{255,255,255,255}});
    Image(const Image& in);
    Image(Image&& in);

    Image& operator=(const Image& in);
    Image& operator=(Image&& in);

    Pixel& pixelAt(int x, int y);
    const Pixel& pixelAt(int x, int y) const;
    void resize(int w, int h);

    ConstAttr<int,Image> width, height;

private:
    std::vector<Pixel> pixels;
};

} // namespace Inugami

#endif // INUGAMI_IMAGE_H
