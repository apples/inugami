/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "texture.h"

#include "loaders.h"

#include <algorithm>
#include <stdexcept>

namespace Inugami {

Texture::Bank Texture::pool;

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

bool Texture::Index::operator==(const Index &in) const
{
    if (name == in.name) return true;
    if (smooth == in.smooth) return true;
    if (clamp == in.clamp) return true;
    return false;
}

Texture::Value::Value() :
    id(0),
    width(0), height(0),
    users(0)
{}

Texture::Texture(const std::string &filename, bool smooth, bool clamp) :
    id(filename, smooth, clamp)
{
    if (pool.find(id) == pool.end())
    {
        std::vector<char> data;
        if (!loadImageFromFile(id.name, data)) throw std::runtime_error("Couldn't load image!");

        GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D, newID);

        GLuint filter = (id.smooth)? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        GLuint wrap = (id.clamp)? GL_CLAMP : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            *reinterpret_cast<int*>(&data[0]), *reinterpret_cast<int*>(&data[4]),
            0, GL_RGBA, GL_UNSIGNED_BYTE, &data[8]
        );

        pool[id].id = newID;
        pool[id].width = *reinterpret_cast<int*>(&data[0]);
        pool[id].height = *reinterpret_cast<int*>(&data[sizeof(int)]);
        pool[id].users = 1;
    }
    else
    {
        ++pool[id].users;
    }
}

Texture::Texture(int) :
    id("", false, false)
{
    if (pool.find(id) == pool.end())
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

        pool[id].id = newID;
        pool[id].width = 1;
        pool[id].height = 1;
        pool[id].users = 1;

    }
    else
    {
        ++pool[id].users;
    }
}

Texture::Texture(const Texture &in) :
    id(in.id)
{
    ++pool[id].users;
}

Texture::~Texture()
{
    if (--pool[id].users == 0)
    {
        glDeleteTextures(1, &pool[id].id);
        pool.erase(pool.find(id));
    }
}

Texture &Texture::operator=(const Texture &in)
{
    if (id == in.id) return *this;

    if (--pool[id].users == 0)
    {
        glDeleteTextures(1, &pool[id].id);
        pool.erase(pool.find(id));
    }

    id = in.id;
    ++pool[id].users;

    return *this;
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, pool[id].id);
}

unsigned int Texture::getWidth() const
{
    return pool[id].width;
}

unsigned int Texture::getHeight() const
{
    return pool[id].height;
}

const std::string& Texture::getName() const
{
    return id.name;
}

} // namespace Inugami
