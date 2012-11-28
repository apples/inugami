#ifndef INUGAMI_TEXTURE_H
#define INUGAMI_TEXTURE_H

#include "opengl.h"

#include <list>
#include <string>
#include <utility>

namespace Inugami {

class Texture
{
public:
    Texture(const std::string &filename, bool smooth, bool clamp);
    Texture(const char *filename, bool smooth, bool clamp);
    Texture(int);
    Texture(const Texture &in);
    virtual ~Texture();

    Texture &operator=(const Texture &in);

    void bind() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

private:
    class Index
    {
    public:
        Index();
        Index(const std::string &inName, bool inSmooth, bool inClamp);
        bool operator<(const Index &in) const;
        std::string name;
        bool smooth, clamp;
    };

    struct Value
    {
        Value();
        GLuint id;
        unsigned int width, height;
        int users;
    };

    std::list<std::pair<Index, Value>>::iterator pos;
    static std::list<std::pair<Index, Value>> pool;
};

} // namespace Inugami

#endif // INUGAMI_TEXTURE_H
