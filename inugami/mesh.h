/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

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
    class Vertex
    {
    public:
        bool operator==(const Vertex &in);
        ::glm::vec3 pos;
        ::glm::vec3 norm;
        ::glm::vec2 uv;
    };

    class Triangle
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
