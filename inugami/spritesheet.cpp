#include "spritesheet.h"

#include "utility.h"

namespace Inugami {

Spritesheet::Spritesheet(Renderer* r, const char* fileName, unsigned int w, unsigned int h)
{
    init(r, std::string(fileName), w, h);
}

Spritesheet::Spritesheet(Renderer* r, const std::string& fileName, unsigned int w, unsigned int h)
{
    init(r, fileName, w, h);
}

Spritesheet::~Spritesheet()
{
    renderer->dropTexture(tex);
}

Mesh &Spritesheet::getMesh(unsigned int r, unsigned int c)
{
    if (c>tilesW || r>tilesH) throw;
    return sprites[r*tilesW+c];
}

Renderer::Texture &Spritesheet::getTex()
{
    return tex;
}

void Spritesheet::init(Renderer *r, const std::string &fileName, unsigned int w, unsigned int h)
{
    renderer = r;
    auto pow2 = [](unsigned int i) {return (i!=0 && (i&(i-1))==0);};

    //Throw if dimensions aren't powers of two
    if (!pow2(w) || !pow2(h)) throw;

    Renderer::TexParams tp;
    tp.clamp = false;
    tp.smooth = false;
    tex = renderer->loadTexture(fileName, tp);
    if (!pow2(tex.width) || !pow2(tex.height))
    {
        renderer->dropTexture(tex);
        throw;
    }

    tilesW = tex.width/w;
    tilesH = tex.width/h;
    sprites.resize(tilesW * tilesH);

    Mesh::Vertex vert;
    vert.pos.z() = 0.0f;
    vert.norm.x() = 0.0f;
    vert.norm.y() = 0.0f;
    vert.norm.z() = 1.0f;

    Vector<float, 2> uvStep;
    uvStep.x() = float(w)/float(tex.width);
    uvStep.y() = float(h)/float(tex.height);

    for (unsigned int r = 0; r<tilesH; ++r)
    {
        for (unsigned int c = 0; c<tilesW; ++c)
        {
            float u = float(c)*uvStep.x();
            float v = 1.0f-float(r)*uvStep.y();

            Mesh &mesh = sprites[r*tilesW +c];
            Mesh::Triangle tri;

            vert.pos.x() = -float(w/2);
            vert.pos.y() = float(h/2);
            vert.uv.x() = u;
            vert.uv.y() = v;
            tri.v[0] = addOnce(mesh.vertices, vert);
            vert.pos.y() = -float(h/2);
            vert.uv.y() = v-uvStep.y()+1.953125e-3f; //Constant prevents rounding errors
            tri.v[1] = addOnce(mesh.vertices, vert);
            vert.pos.x() = float(w/2);
            vert.uv.x() = u+uvStep.x();
            tri.v[2] = addOnce(mesh.vertices, vert);
            mesh.triangles.push_back(tri);
            tri.v[1] = tri.v[2];
            vert.pos.y() = float(h/2);
            vert.uv.y() = v;
            tri.v[2] = addOnce(mesh.vertices, vert);
            mesh.triangles.push_back(tri);

            mesh.init();
        }
    }
}

} // namespace Inugami
