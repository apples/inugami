/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

namespace Inugami{ class Texture; }

#ifndef INUGAMI_TEXTURE_H
#define INUGAMI_TEXTURE_H

#include "inugami.hpp"

#include "image.hpp"
#include "opengl.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Inugami {

class Texture final
{
    friend class TextureException;
public:
    Texture() = delete;
    Texture(const Image& img, bool smooth=false, bool clamp=false);
    Texture(const Texture &in);
    Texture(Texture&& in);
    ~Texture();

    Texture &operator=(const Texture &in);
    Texture &operator=(Texture&& in);

    void bind(unsigned int slot) const;

    ConstAttr<int,Texture> width, height;

private:
    struct Shared
    {
        int users;
        GLuint id;
    };

    Shared* share;

    void upload(const Image& data, bool smooth, bool clamp);
};

} // namespace Inugami

#endif // INUGAMI_TEXTURE_H
