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

#include "inugami.hpp"

#include "mesh.hpp"
#include "texture.hpp"

#include <vector>

namespace Inugami {

class Spritesheet
{
public:
    Spritesheet() = delete;
    Spritesheet(const Image& img, int tw, int th, float cx=0.5f, float cy=0.5f);
    Spritesheet(const Texture& in, int tw, int th, float cx=0.5f, float cy=0.5f);
    Spritesheet(Texture&& in, int tw, int th, float cx = 0.5f, float cy = 0.5f);
    Spritesheet(const Spritesheet& in);
    Spritesheet(Spritesheet&& in);
    virtual ~Spritesheet();

    Spritesheet& operator=(const Spritesheet& in);
    Spritesheet& operator=(Spritesheet&& in);

    void draw(int r, int c) const;

    ConstAttr<int,Spritesheet> tilesX, tilesY;

private:
    void generateMeshes(int tw, int th, float cx, float cy);

    Texture tex;
    std::vector<Mesh> meshes;
};

} // namespace Inugami

#endif // INUGAMI_SPRITESHEET_H
