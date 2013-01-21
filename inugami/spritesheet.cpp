#include "spritesheet.h"

#include "utility.h"

#include <limits>

namespace Inugami {

std::map<Spritesheet::Dimensions, std::vector<Mesh>> Spritesheet::pool;

bool Spritesheet::Dimensions::operator<(const Dimensions &in) const
{
    return chainComp(w, in.w,
                     h, in.h,
                     tw, in.tw,
                     th, in.th,
                     cx, in.cx,
                     cy, in.cy);
}

Spritesheet::Spritesheet(const std::string& filename, unsigned int w, unsigned int h, float cx, float cy) :
    tex(filename, false, false)
{
    constexpr float E = std::numeric_limits<float>::epsilon() * 1.0e3;

    auto pow2 = [](unsigned int i) {return (i!=0 && (i&(i-1))==0);};

    dim.w = tex.getWidth()/w;
    dim.h = tex.getHeight()/h;
    dim.tw = w;
    dim.th = h;
    dim.cx = cx;
    dim.cy = cy;

    auto& sprites = pool[dim]; //FIXME spritesheet pool

    if (sprites.size() == 0)
    {
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

                Mesh &mesh = sprites[r*dim.w +c];
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
}

Spritesheet::Spritesheet(const Spritesheet& in) :
    tex(in.tex),
    dim(in.dim)
{}

Spritesheet::~Spritesheet()
{}

void Spritesheet::draw(unsigned int r, unsigned int c)
{
    getTex().bind();
    getMesh(r, c).draw();
}

Mesh &Spritesheet::getMesh(unsigned int r, unsigned int c)
{
    if (c>dim.w || r>dim.h) throw;
    return pool[dim][r*dim.w+c];
}

Texture &Spritesheet::getTex()
{
    return tex;
}

} // namespace Inugami
