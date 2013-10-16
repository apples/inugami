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

#include "spritesheet.hpp"

#include "geometry.hpp"
#include "mathtypes.hpp"
#include "utility.hpp"

#include <limits>
#include <utility>

namespace Inugami {

Spritesheet::Spritesheet(const Image& img, int tw, int th, float cx, float cy)
    : Spritesheet(Texture(img, false, false), tw, th, cx, cy)
{}

Spritesheet::Spritesheet(const Texture& in, int tw, int th, float cx, float cy)
    : tilesX(0)
    , tilesY(0)
    , tex(in)
    , meshes()
{
    generateMeshes(tw, th, cx, cy);
}

void Spritesheet::draw(int r, int c) const
{
    tex.bind(0);
    meshes[r*tilesX+c].draw();
}

void Spritesheet::generateMeshes(int tw, int th, float cx, float cy)
{
    constexpr float E = 0.0f; // std::numeric_limits<float>::epsilon() * 1.0e4;

    tilesX = tex.width/tw;
    tilesY = tex.height/th;

    Geometry::Vertex vert[4];
    Geometry::Triangle tri;

    vert[0].pos = Vec3{-cx*tw   , -cy*th   , 0.f};
    vert[1].pos = Vec3{-cx*tw   , -cy*th+th, 0.f};
    vert[2].pos = Vec3{-cx*tw+tw, -cy*th+th, 0.f};
    vert[3].pos = Vec3{-cx*tw+tw, -cy*th   , 0.f};
    vert[0].norm = Vec3{0.f, 0.f, 1.f};
    vert[1].norm = Vec3{0.f, 0.f, 1.f};
    vert[2].norm = Vec3{0.f, 0.f, 1.f};
    vert[3].norm = Vec3{0.f, 0.f, 1.f};

    for (int r = 0; r<tilesY; ++r)
    {
        for (int c = 0; c<tilesX; ++c)
        {
            Geometry geo;

            float x1 = float(c  )/float(tilesX)+E;
            float x2 = float(c+1)/float(tilesX)-E;

            float y1 = float(tilesY-r-1)/float(tilesY)+E;
            float y2 = float(tilesY-r  )/float(tilesY)-E;

            vert[0].tex = Vec2{x1, y1};
            vert[1].tex = Vec2{x1, y2};
            vert[2].tex = Vec2{x2, y2};
            vert[3].tex = Vec2{x2, y1};

            tri[0] = addOnceVec(geo.vertices, vert[0]);
            tri[1] = addOnceVec(geo.vertices, vert[1]);
            tri[2] = addOnceVec(geo.vertices, vert[2]);

            geo.triangles.push_back(tri);

            tri[1] = addOnceVec(geo.vertices, vert[3]);

            geo.triangles.push_back(tri);

            meshes.emplace_back(std::move(geo));
        }
    }
}

} // namespace Inugami
