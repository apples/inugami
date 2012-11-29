#include "spritesheet.h"

#include "utility.h"

namespace Inugami {

std::map<Spritesheet::Dimensions, std::vector<Mesh>> Spritesheet::pool;

bool Spritesheet::Dimensions::operator<(const Dimensions &in) const
{
    if (w < in.w) return true;
    if (w == in.w) if (h < in.h) return true;
    return false;
}

Spritesheet::Spritesheet(const char* filename, unsigned int w, unsigned int h) :
    Spritesheet(std::string(filename), w, h)
{}

Spritesheet::Spritesheet(const std::string& filename, unsigned int w, unsigned int h) :
    tex(filename, false, false)
{
    auto pow2 = [](unsigned int i) {return (i!=0 && (i&(i-1))==0);};

    //Throw if dimensions aren't powers of two
    if (!pow2(w) || !pow2(h)) throw;

    dim.w = tex.getWidth()/w;
    dim.h = tex.getHeight()/h;

    sprites = &pool[dim];

    if (sprites->size() == 0)
    {
        sprites->resize(dim.w * dim.h);

        Mesh::Vertex vert;
        vert.pos.z() = 0.0f;
        vert.norm.x() = 0.0f;
        vert.norm.y() = 0.0f;
        vert.norm.z() = 1.0f;

        Vector<float, 2> uvStep;
        uvStep.x() = float(w)/float(tex.getWidth());
        uvStep.y() = float(h)/float(tex.getHeight());

        for (unsigned int r = 0; r<dim.h; ++r)
        {
            for (unsigned int c = 0; c<dim.w; ++c)
            {
                float u = float(c)*uvStep.x();
                float v = 1.0f-float(r)*uvStep.y();

                Mesh &mesh = (*sprites)[r*dim.w +c];
                Mesh::Triangle tri;

                vert.pos.x() = -float(w/2);
                vert.pos.y() = float(h/2);
                vert.uv.x() = u;
                vert.uv.y() = v;
                tri.v[0] = mesh.addVertex(vert);
                vert.pos.y() = -float(h/2);
                vert.uv.y() = v-uvStep.y()+1.953125e-3f; //Constant prevents rounding errors
                tri.v[1] = mesh.addVertex(vert);
                vert.pos.x() = float(w/2);
                vert.uv.x() = u+uvStep.x();
                tri.v[2] = mesh.addVertex(vert);
                mesh.addTriangle(tri);
                tri.v[1] = tri.v[2];
                vert.pos.y() = float(h/2);
                vert.uv.y() = v;
                tri.v[2] = mesh.addVertex(vert);
                mesh.addTriangle(tri);

                mesh.init();
            }
        }
    }
}

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
    return (*sprites)[r*dim.w+c];
}

Texture &Spritesheet::getTex()
{
    return tex;
}

} // namespace Inugami
