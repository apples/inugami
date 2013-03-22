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

#include "opengl.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Inugami {

class Texture final
{
    friend class SharedBank;
    friend class TextureException;
public:
    using Data = std::vector<char>;
    static int& initData(Data& in, int width, int height);
    static int& dataWidth(Data& in);
    static int& dataHeight(Data& in);
    static char* dataPixel(Data& in, int x, int y);
    static const int& dataWidth(const Data& in);
    static const int& dataHeight(const Data& in);
    static const char* dataPixel(const Data& in, int x, int y);

    Texture(Core& coreIn, const std::string &filename, bool smooth=false, bool clamp=false);
    Texture(Core& coreIn, const Data& data, bool smooth=false, bool clamp=false);
    Texture(Core& coreIn, unsigned int color, bool smooth=false, bool clamp=false);
    Texture(const Texture &in);
    ~Texture();

    Texture &operator=(const Texture &in);

    void bind(unsigned int slot) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

    const std::string& getName() const;

private:
    class Index
    {
    public:
        Index();
        Index(const std::string &inName, bool inSmooth, bool inClamp);
        bool operator<(const Index &in) const;
        bool operator==(const Index &in) const;
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

    using Bank = std::map<Index, Value>;

    void setParams(const Data& data);

    Index id;

    Bank& bank;
};

} // namespace Inugami

#endif // INUGAMI_TEXTURE_H
