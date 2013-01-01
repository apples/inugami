/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_SPRITESHEET_H
#define INUGAMI_SPRITESHEET_H

#include "mesh.h"
#include "texture.h"

#include <map>
#include <vector>

namespace Inugami
{

class Spritesheet
{
public:
    Spritesheet(const char *filename, unsigned int w, unsigned int h, float cx = 0.5f, float cy = 0.5f);
    Spritesheet(const std::string &filename, unsigned int w, unsigned int h, float cx = 0.5f, float cy = 0.5f);
    virtual ~Spritesheet();

    void draw(unsigned int r, unsigned int c);

    Mesh &getMesh(unsigned int r, unsigned int c);
    Texture &getTex();

private:
    struct Dimensions
    {
        bool operator<(const Dimensions &in) const;
        unsigned int w, h;
    };

    void init(const std::string &filename, unsigned int w, unsigned int h);

    Texture tex;
    std::vector<Mesh> *sprites;

    Dimensions dim;

    static std::map<Dimensions, std::vector<Mesh>> pool;
};

} // namespace Inugami

#endif // INUGAMI_SPRITESHEET_H
