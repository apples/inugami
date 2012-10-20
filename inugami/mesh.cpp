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
    vertices(0), triangles(0)
{
    //ctor
}

Mesh::~Mesh()
{
    //dtor
}

void Mesh::draw()
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
