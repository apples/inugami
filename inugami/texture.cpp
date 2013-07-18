/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
 * Version: 0.2.0
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

#include "texture.hpp"

#include "core.hpp"
#include "loaders.hpp"
#include "utility.hpp"
#include "exception.hpp"

#include <algorithm>

namespace Inugami {

class TextureException : public Exception
{
public:
    TextureException() = delete;

    TextureException(const TextureException& in)
        : tex(in.tex)
        , err(in.err)
    {}

    TextureException(TextureException&& in)
        : tex(in.tex)
        , err(std::move(in.err))
    {}

    TextureException(const Texture *source, std::string error)
        : tex(source)
        , err("")
    {
        err += "Texture Exception: ";
        err += error;
    }

    virtual const char* what() const noexcept override
    {
        return err.c_str();
    }

    const Texture *tex;
    std::string err;
};

Texture::Shared::Shared()
    : id(0)
{
    glGenTextures(1, &id);
}

Texture::Shared::~Shared()
{
    glDeleteTextures(1, &id);
}

Texture::Texture(const Image& img, bool smooth, bool clamp)
    : width (img.width)
    , height(img.height)
    , share (new Shared)
{
    upload(img, smooth, clamp);
}

void Texture::bind(unsigned int slot) const
{
    if (slot > 31) throw TextureException(this, "Invalid texture slot!");
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, share->id);
}

void Texture::upload(const Image& img, bool smooth, bool clamp)
{
    glBindTexture(GL_TEXTURE_2D, share->id);

    GLuint filter = (smooth)? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    GLuint wrap = (clamp)? GL_CLAMP : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        img.width, img.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, &img.pixelAt(0,0)
    );
}

} // namespace Inugami
