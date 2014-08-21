/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
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

#include "../../opengl.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace Inugami {

namespace Resources_detail {

Texture::Shared::Shared()
    : id(0)
    , width()
    , height()
{
    glGenTextures(1, &id);
}

Texture::Shared::~Shared()
{
    glDeleteTextures(1, &id);
}

Texture::Texture(const Image& img, bool smooth, bool clamp)
    : share (std::make_shared<Shared>())
{
    upload(img, smooth, clamp);
}

void Texture::bind(unsigned int slot) const
{
    if (slot > 31) throw std::runtime_error("Invalid texture slot!");
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, share->id);
}

int Texture::getWidth() const
{
    return share->width;
}

int Texture::getHeight() const
{
    return share->height;
}

void Texture::upload(const Image& img, bool smooth, bool clamp)
{
    share->width = img.getWidth();
    share->height = img.getHeight();

    glBindTexture(GL_TEXTURE_2D, share->id);

    GLuint filter = (smooth)? GL_LINEAR : GL_NEAREST;
    GLuint wrap   = (clamp )? GL_CLAMP  : GL_REPEAT;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(
        GL_TEXTURE_2D
        , 0
        , GL_RGBA
        , img.getWidth()
        , img.getHeight()
        , 0
        , GL_RGBA
        , GL_UNSIGNED_BYTE
        , &img[0][0]
    );
}

} // namespace Resources_detail

} // namespace Inugami
