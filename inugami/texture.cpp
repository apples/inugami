/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "texture.hpp"

#include "core.hpp"
#include "loaders.hpp"
#include "utility.hpp"
#include "exception.hpp"
#include "sharedbank.hpp"

#include <algorithm>

namespace Inugami {

class TextureException : public Exception
{
public:
    TextureException(const Texture *source, std::string error) :
        tex(source), err(error)
    {}

    virtual const char* what() const noexcept override
    {
        std::string rval;
        rval += "Texture Exception: ";
        rval += tex->id.name;
        rval += "; ";
        rval += err;
        return rval.c_str();
    }

    const Texture *tex;
    std::string err;
};

int& Texture::initData(Data& in, int width, int height) //static
{
    in.resize(sizeof(int)*2+width*height*4);
    dataWidth(in) = width;
    dataHeight(in) = height;
}

int& Texture::dataWidth(Data& in) //static
{
    return *reinterpret_cast<int*>(&in[0]);
}

int& Texture::dataHeight(Data& in) //static
{
    return *reinterpret_cast<int*>(&in[sizeof(int)]);
}

char* Texture::dataPixel(Data& in, int x, int y) //static
{
    return &in[sizeof(int)*2+(y*dataWidth(in)+x)*4];
}

const int& Texture::dataWidth(const Data& in) //static
{
    return *reinterpret_cast<const int*>(&in[0]);
}

const int& Texture::dataHeight(const Data& in) //static
{
    return *reinterpret_cast<const int*>(&in[sizeof(int)]);
}

const char* Texture::dataPixel(const Data& in, int x, int y) //static
{
    return &in[sizeof(int)*2+(y*dataWidth(in)+x)*4];
}

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

Texture::Texture(Core& coreIn, const std::string &filename, bool smooth, bool clamp) :
    id(filename, smooth, clamp),
    bank(coreIn.banks->textureBank)
{
    if (bank.find(id) == bank.end())
    {
        std::vector<char> data;

        coreIn.activate();

        if (id.name.substr(0,2) == "0x" && id.name.size() == 10)
        {
            data.resize(sizeof(int)*3);
            dataWidth(data) = 1;
            dataHeight(data) = 1;

            for (int i=0; i<4; ++i)
            {
                dataPixel(data,0,0)[i] = dehexify(id.name.substr(2+i*2,2));
            }
        }
        else
        {
            if (!loadImageFromFile(id.name, data)) throw TextureException(this, "Failed to load image!");
        }

        setParams(data);
    }
    else
    {
        ++bank[id].users;
    }
}

Texture::Texture(Core& coreIn, const Data &data, bool smooth, bool clamp) :
    id(stringify(this), smooth, clamp),
    bank(coreIn.banks->textureBank)
{
    coreIn.activate();
    setParams(data);
}

Texture::Texture(Core& coreIn, unsigned int color, bool smooth, bool clamp) :
    Texture(coreIn, hexify(color), smooth, clamp)
{}

Texture::Texture(const Texture &in) :
    id(in.id),
    bank(in.bank)
{
    ++bank[id].users;
}

Texture::~Texture()
{
    auto iter = bank.find(id);
    if (--iter->second.users == 0)
    {
        glDeleteTextures(1, &iter->second.id);
        bank.erase(iter);
    }
}

Texture &Texture::operator=(const Texture &in)
{
    if (id == in.id) return *this;

    auto iter = bank.find(id);
    if (--iter->second.users == 0)
    {
        glDeleteTextures(1, &iter->second.id);
        bank.erase(iter);
    }

    id = in.id;
    ++ bank[id].users;

    return *this;
}

void Texture::bind(unsigned int slot) const
{
    if (slot > 31) throw TextureException(this, "Invalid texture slot!");
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, bank[id].id);
}

unsigned int Texture::getWidth() const
{
    return bank[id].width;
}

unsigned int Texture::getHeight() const
{
    return bank[id].height;
}

const std::string& Texture::getName() const
{
    return id.name;
}

void Texture::setParams(const Data& data)
{
    if (data.size() != sizeof(int)*2+dataWidth(data)*dataHeight(data)*4)
    {
        throw TextureException(this, "Invalid data!");
    }

    GLuint newID;
    glGenTextures(1, &newID);
    glBindTexture(GL_TEXTURE_2D, newID);

    GLuint filter = (id.smooth)? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    GLuint wrap = (id.clamp)? GL_CLAMP : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        dataWidth(data), dataHeight(data),
        0, GL_RGBA, GL_UNSIGNED_BYTE, &data[sizeof(int)*2]
    );

    Value& val = bank[id];

    val.id = newID;
    val.width = dataWidth(data);
    val.height = dataHeight(data);
    val.users = 1;
}

} // namespace Inugami
