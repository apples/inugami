/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
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

#include "spritesheet.hpp"

#include "geometry.hpp"
#include "utility.hpp"

#include <limits>
#include <utility>

using namespace std;

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
    //TODO Spritesheet epsilon
    constexpr float E = numeric_limits<float>::epsilon() * 1.0e3;

    tilesX = tex.width/tw;
    tilesY = tex.height/th;

    Geometry::Vertex vert[4];
    Geometry::Triangle tri;

    vert[0].pos = Geometry::Vec3{-cx*tw,    -cy*th,    0.f};
    vert[1].pos = Geometry::Vec3{-cx*tw,    -cy*th+th, 0.f};
    vert[2].pos = Geometry::Vec3{-cx*tw+tw, -cy*th+th, 0.f};
    vert[3].pos = Geometry::Vec3{-cx*tw+tw, -cy*th,    0.f};
    vert[0].norm = Geometry::Vec3{0.f, 0.f, 1.f};
    vert[1].norm = Geometry::Vec3{0.f, 0.f, 1.f};
    vert[2].norm = Geometry::Vec3{0.f, 0.f, 1.f};
    vert[3].norm = Geometry::Vec3{0.f, 0.f, 1.f};

    for (int r = 0; r<tilesY; ++r)
    {
        for (int c = 0; c<tilesX; ++c)
        {
            Geometry geo;

            vert[0].tex = Geometry::Vec2{float(c)/float(tilesX)+E, float(tilesY-r-1)/float(tilesY)+E};
            tri[0] = addOnce(geo.vertices, vert[0]);

            vert[1].tex = Geometry::Vec2{float(c)/float(tilesX)+E, float(tilesY-r)/float(tilesY)-E};
            tri[1] = addOnce(geo.vertices, vert[1]);

            vert[2].tex = Geometry::Vec2{float(c+1)/float(tilesX)-E, float(tilesY-r)/float(tilesY)-E};
            tri[2] = addOnce(geo.vertices, vert[2]);

            geo.triangles.push_back(tri);

            vert[3].tex = Geometry::Vec2{float(c+1)/float(tilesX)-E, float(tilesY-r-1)/float(tilesY)+E};
            tri[1] = addOnce(geo.vertices, vert[3]);

            geo.triangles.push_back(tri);

            meshes.emplace_back(geo);
        }
    }
}

} // namespace Inugami
