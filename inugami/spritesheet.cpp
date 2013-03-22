/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "spritesheet.hpp"

#include "core.hpp"
#include "sharedbank.hpp"
#include "utility.hpp"

#include <limits>
#include <string>

namespace Inugami {

bool Spritesheet::Index::operator<(const Index &in) const
{
    return chainComp(w, in.w,
                     h, in.h,
                     tw, in.tw,
                     th, in.th,
                     cx, in.cx,
                     cy, in.cy);
}

Spritesheet::Value::Value() :
    meshes(), users(0)
{}

Spritesheet::Spritesheet(Core& coreIn, const std::string& filename, unsigned int w, unsigned int h, float cx, float cy) :
    tex(coreIn, filename, false, false),
    bank(coreIn.banks->spritesheetBank)
{
    constexpr float E = std::numeric_limits<float>::epsilon() * 1.0e3;

    dim.w = tex.getWidth()/w;
    dim.h = tex.getHeight()/h;
    dim.tw = w;
    dim.th = h;
    dim.cx = cx;
    dim.cy = cy;

    auto& val = bank[dim];

    if (val.users == 0)
    {
        auto& sprites = val.meshes;
        sprites.resize(dim.w * dim.h);

        Mesh::Vertex vert;
        vert.pos.z = 0.0f;
        vert.norm.x = 0.0f;
        vert.norm.y = 0.0f;
        vert.norm.z = 1.0f;

        ::glm::vec2 uvStep;
        uvStep.x = float(w)/float(tex.getWidth());
        uvStep.y = float(h)/float(tex.getHeight());

        for (unsigned int r = 0; r<dim.h; ++r)
        {
            for (unsigned int c = 0; c<dim.w; ++c)
            {
                float u = float(c)*uvStep.x;
                float v = 1.0f-float(r)*uvStep.y;

                Mesh &mesh = *new Mesh(coreIn);
                sprites[r*dim.w +c] = &mesh;

                Mesh::Triangle tri;

                vert.pos.x = -float(w)*cx;
                vert.pos.y = float(h)*cy;
                vert.uv.x = u+E;
                vert.uv.y = v-E;
                tri.v[0] = mesh.addVertex(vert);
                vert.pos.y = -float(h)*(1.0f-cy);
                vert.uv.y = v-uvStep.y+E;
                tri.v[1] = mesh.addVertex(vert);
                vert.pos.x = float(w)*(1.0f-cx);
                vert.uv.x = u+uvStep.x-E;
                tri.v[2] = mesh.addVertex(vert);
                mesh.addTriangle(tri);
                tri.v[1] = tri.v[2];
                vert.pos.y = float(h)*cy;
                vert.uv.y = v-E;
                tri.v[2] = mesh.addVertex(vert);
                mesh.addTriangle(tri);

                mesh.init();
            }
        }
    }

    ++val.users;
}

Spritesheet::Spritesheet(const Spritesheet& in) :
    tex(in.tex),
    dim(in.dim),
    bank(in.bank)
{
    ++ bank[dim].users;
}

Spritesheet::~Spritesheet()
{
    auto iter = bank.find(dim);
    if (-- iter->second.users == 0)
    {
        bank.erase(iter);
    }
}

void Spritesheet::draw(unsigned int r, unsigned int c)
{
    getTex().bind(0);
    getMesh(r, c).draw();
}

Mesh &Spritesheet::getMesh(unsigned int r, unsigned int c)
{
    if (c>dim.w || r>dim.h) throw;
    return *bank[dim].meshes[r*dim.w+c];
}

Texture &Spritesheet::getTex()
{
    return tex;
}

} // namespace Inugami
