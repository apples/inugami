#ifndef INUGAMI_MESH_H
#define INUGAMI_MESH_H

#include "mathtypes.h"
#include "opengl.h"

#include <vector>

namespace Inugami {

class Mesh
{
    friend class Core;
public:
    class __attribute__((__packed__)) Vertex
    {
    public:
        bool operator==(const Vertex &in);
        Vec3 pos;
        Vec3 norm;
        Vec2 uv;
    };

    class __attribute__((__packed__)) Triangle
    {
    public:
        bool operator==(const Triangle &in);
        unsigned int v[3];
    };

    Mesh();
    virtual ~Mesh();

    Mesh &init();
    void draw();
    void drawImmediate();

    int addVertex(const Vertex &in);
    int addTriangle(const Triangle &in);
    void reserve(int v, int t);

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    bool initted;
    GLuint vbo, vao, ele;
};

} // namespace Inugami

#endif // INUGAMI_MESH_H
