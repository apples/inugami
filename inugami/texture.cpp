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

Texture::Texture(Core& coreIn, const std::string &name, bool smooth, bool clamp) :
    id(name, smooth, clamp),
    bank(coreIn.banks->textureBank)
{
    if (bank.find(id) == bank.end())
    {
        Image img;

        coreIn.activate();

        if (id.name.substr(0,2) == "0x" && id.name.size() == 10)
        {
            img.resize(1,1);
            img.pixelAt(0,0).red   = dehexify(id.name.substr(2,2));
            img.pixelAt(0,0).green = dehexify(id.name.substr(4,2));
            img.pixelAt(0,0).blue  = dehexify(id.name.substr(6,2));
            img.pixelAt(0,0).alpha = dehexify(id.name.substr(8,2));
        }
        else
        {
            img = Image(name);
        }

        setParams(img);
    }
    else
    {
        ++bank[id].users;
    }
}

Texture::Texture(Core& coreIn, const Image& img, bool smooth, bool clamp) :
    id(stringify(this), smooth, clamp),
    bank(coreIn.banks->textureBank)
{
    coreIn.activate();
    setParams(img);
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

void Texture::setParams(const Image& img)
{
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
        img.width, img.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, &img.pixelAt(0,0)
    );

    Value& val = bank[id];

    val.id = newID;
    val.width = img.width;
    val.height = img.height;
    val.users = 1;
}

} // namespace Inugami
