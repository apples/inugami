#ifndef INUGAMI_IMAGE_H
#define INUGAMI_IMAGE_H

#include <png++/png.hpp>

#include <string>
#include <vector>

namespace Inugami {

class Image
{
public:
    using Pixel = png::rgba_pixel;

    Image();
    Image(int w, int h);
    Image(const std::string& filename);
    Image(const Image& in);
    Image(Image&& in);

    Image& operator=(const Image& in);
    Image& operator=(Image&& in);

    Pixel& pixelAt(int x, int y);
    const Pixel& pixelAt(int x, int y) const;
    void resize(int w, int h);

    int width, height;
private:
    std::vector<Pixel> pixels;
};

} // namespace Inugami

#endif // INUGAMI_IMAGE_H
