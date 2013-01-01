/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_TEXTURE_H
#define INUGAMI_TEXTURE_H

#include "opengl.h"

#include <list>
#include <string>
#include <utility>

namespace Inugami {

class Texture final
{
public:
    Texture(const std::string &filename, bool smooth, bool clamp);
    Texture(int);
    Texture(const Texture &in);
    ~Texture();

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

    class Value
    {
    public:
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
