#ifndef INUGAMI_MESH_H
#define INUGAMI_MESH_H

#include <vector>
#include <GL/glfw.h>

namespace Inugami {

class Mesh
{
public:
    struct Vertex3 {float x, y, z;};
    struct Vertex2{float u, v;};
    struct VertexData {int pos; int norm; int uv;};
    struct Triangle {VertexData vertices[3];};

    Mesh();
    virtual ~Mesh();

    void createList();

    void drawList();
    void drawImmediate();

    std::vector<Vertex3> vertices;
    std::vector<Vertex3> normals;
    std::vector<Vertex2> texCoords;
    std::vector<Triangle> triangles;

private:
    bool listCreated;
    GLuint displayList;
};

} // namespace Inugami

#endif // INUGAMI_MESH_H
