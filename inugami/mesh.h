#ifndef INUGAMI_MESH_H
#define INUGAMI_MESH_H

#include "mathtypes.h"

#include <vector>
#include <GL/glfw.h>

namespace Inugami {

class Mesh
{
public:
    class Vertex
    {
    public:
        bool operator==(const Vertex &in);
        Vector<float, 3> pos;
        Vector<float, 3> norm;
        Vector<float, 2> uv;
    };

    class Triangle
    {
    public:
        bool operator==(const Triangle &in);
        int v[3];
    };

    Mesh();
    virtual ~Mesh();

    void init();
    void draw();

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

private:
    bool initted;
    GLuint vbo;
};

} // namespace Inugami

#endif // INUGAMI_MESH_H
