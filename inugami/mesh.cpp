/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "mesh.h"

#include "utility.h"

#include <stdexcept>

namespace Inugami {

bool Mesh::Vertex::operator==(const Vertex &in)
{
    return (pos==in.pos&&norm==in.norm&&uv==in.uv);
}

bool Mesh::Triangle::operator==(const Triangle &in)
{
    return (v[0]==in.v[0]&&v[1]==in.v[1]&&v[2]==in.v[2]);
}

Mesh::Mesh() :
    vertices(0), triangles(0),
    initted(false),
    vbo(0), ele(0)
{}

Mesh::~Mesh()
{
    if (initted)
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ele);
    }
}

Mesh &Mesh::init()
{
    if (initted)
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ele);
    }

    vertices.shrink_to_fit();
    triangles.shrink_to_fit();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3))); //TODO find pointer offset type
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3)*2)); //TODO find pointer offset type

    glGenBuffers(1, &ele);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle)*triangles.size(), &triangles[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    initted = true;
    return *this;
}

void Mesh::draw()
{
    if (!initted) throw std::logic_error("Mesh not initialized.");

    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0);
//    glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3)));
//    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3)*2));

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
    glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT, 0);
}

void Mesh::drawImmediate()
{
    glBegin(GL_TRIANGLES);
    for (Triangle &i : triangles)
    {
        for (int j=0; j<3; ++j)
        {
            glTexCoord2f(
                vertices[i.v[j]].uv.x,
                vertices[i.v[j]].uv.y
            );
            glNormal3f(
                vertices[i.v[j]].norm.x,
                vertices[i.v[j]].norm.y,
                vertices[i.v[j]].norm.z
            );
            glVertex3f(
                vertices[i.v[j]].pos.x,
                vertices[i.v[j]].pos.y,
                vertices[i.v[j]].pos.z
            );
        }
    }
    glEnd();
}

int Mesh::addVertex(const Vertex &in)
{
    return addOnce(vertices, in);
}

int Mesh::addTriangle(const Triangle &in)
{
    return addOnce(triangles, in);
}

void Mesh::reserve(int v, int t)
{
    if (v>=0) vertices.reserve(v);
    if (t>=0) triangles.reserve(t);
}

} // namespace Inugami
