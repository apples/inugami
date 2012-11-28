#ifndef INUGAMI_MESH_H
#define INUGAMI_MESH_H

#include "mathtypes.h"
#include "opengl.h"

#include <vector>

namespace Inugami {

class Mesh
{
friend class Renderer;
public:
    class __attribute__((__packed__)) Vertex
    {
    public:
        bool operator==(const Vertex &in);
        Vector<float, 3> pos;
        Vector<float, 3> norm;
        Vector<float, 2> uv;
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
    GLuint vbo, ele;
};

} // namespace Inugami

#endif // INUGAMI_MESH_H
