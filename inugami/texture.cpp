#include "texture.h"

#include "loaders.h"

#include <algorithm>
#include <stdexcept>

namespace Inugami {

std::list<std::pair<Texture::Index, Texture::Value>> Texture::pool(0);

Texture::Index::Index(const std::string &inName, bool inSmooth, bool inClamp) :
    name(inName),
    smooth(inSmooth), clamp(inClamp)
{}

Texture::Index::Index() :
    name(""),
    smooth(false), clamp(false)
{}

bool Texture::Index::operator<(const Index &in) const
{
    if (name < in.name) return true;
    if (name == in.name)
    {
        if (!smooth && in.smooth) return true;
        if (smooth == in.smooth)
        {
            if (!clamp && in.clamp) return true;
        }
    }
    return false;
}

Texture::Value::Value() :
    id(0),
    width(0), height(0),
    users(0)
{}

Texture::Texture(const std::string &filename, bool smooth, bool clamp) :
    pos()
{
    Index i(filename, smooth, clamp);

    auto comp = [](const std::pair<Index, Value> &p, const Index &i)
    {
        return (p.first < i);
    };
    pos = std::lower_bound(pool.begin(), pool.end(), i, comp);

    if (pos == pool.end()
     || pos->first.name != i.name
     || pos->first.smooth != i.smooth
     || pos->first.clamp != i.clamp)
    {
        std::vector<char> data;
        if (!loadImageFromFile(i.name, data)) throw std::runtime_error("Couldn't load image!");

        GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D, newID);

        GLuint filter = (i.smooth)? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        GLuint wrap = (i.clamp)? GL_CLAMP : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            *reinterpret_cast<int*>(&data[0]), *reinterpret_cast<int*>(&data[4]),
            0, GL_RGBA, GL_UNSIGNED_BYTE, &data[8]
        );

        Value val;
        val.id = newID;
        val.width = *reinterpret_cast<int*>(&data[0]);
        val.height = *reinterpret_cast<int*>(&data[sizeof(int)]);
        val.users = 1;

        pos = pool.insert(pos, std::pair<Index, Value>(i, val));
    }
    else
    {
        ++pos->second.users;
    }
}

Texture::Texture(int) :
    pos()
{
    Index i("", false, false);

    auto comp = [](const std::pair<Index, Value> &p, const Index &i)
    {
        return (p.first < i);
    };
    pos = std::lower_bound(pool.begin(), pool.end(), i, comp);

    if (pos == pool.end()
     || pos->first.name != i.name
     || pos->first.smooth != i.smooth
     || pos->first.clamp != i.clamp)
    {
        unsigned int data = 0xffffffff;

        GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D, newID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            1, 1,
            0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(&data)
        );

        Value val;
        val.id = newID;
        val.width = 1;
        val.height = 1;
        val.users = 1;

        pos = pool.insert(pos, std::pair<Index, Value>(i, val));
    }
    else
    {
        ++pos->second.users;
    }
}

Texture::Texture(const Texture &in) :
    pos(in.pos)
{
    ++pos->second.users;
}

Texture::~Texture()
{
    if (--pos->second.users == 0)
    {
        glDeleteTextures(1, &pos->second.id);
        pool.erase(pos);
    }
}

Texture &Texture::operator=(const Texture &in)
{
    if (pos == in.pos) return *this;

    if (--pos->second.users == 0)
    {
        glDeleteTextures(1, &pos->second.id);
        pool.erase(pos);
    }

    pos = in.pos;
    ++pos->second.users;

    return *this;
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, pos->second.id);
}

unsigned int Texture::getWidth() const
{
    return pos->second.width;
}

unsigned int Texture::getHeight() const
{
    return pos->second.height;
}

} // namespace Inugami
