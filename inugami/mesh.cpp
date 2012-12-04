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

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vec3))); //TODO find pointer offset type
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vec3)*2)); //TODO find pointer offset type

    glGenBuffers(1, &ele);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle)*triangles.size(), &triangles[0], GL_STATIC_DRAW);

    initted = true;
    return *this;
}

void Mesh::draw()
{
    if (!initted) throw std::logic_error("Mesh not initialized.");

    //glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vec3)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vec3)*2));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
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
                vertices[i.v[j]].uv.x(),
                vertices[i.v[j]].uv.y()
            );
            glNormal3f(
                vertices[i.v[j]].norm.x(),
                vertices[i.v[j]].norm.y(),
                vertices[i.v[j]].norm.z()
            );
            glVertex3f(
                vertices[i.v[j]].pos.x(),
                vertices[i.v[j]].pos.y(),
                vertices[i.v[j]].pos.z()
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
