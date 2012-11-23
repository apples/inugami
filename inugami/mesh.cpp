#include "mesh.h"

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
    initted(false)
{
    //ctor
}

Mesh::~Mesh()
{
    //dtor
}

Mesh &Mesh::init()
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ele);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ele);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle)*triangles.size(), &triangles[0], GL_STATIC_DRAW);

    initted = true;
    return *this;
}

void Mesh::draw()
{
    if (!initted) throw;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector<float, 3>)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector<float, 3>)*2));

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


} // namespace Inugami
