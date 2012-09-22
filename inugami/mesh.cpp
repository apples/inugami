#include "mesh.h"

namespace Inugami {

Mesh::Mesh() :
    displayList(0),
    vertices(0), normals(0), texCoords(0), triangles(0),
    listCreated(false)
{
    //ctor
}

Mesh::~Mesh()
{
    //dtor
}

void Mesh::createList()
{
    if (!listCreated)
    {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE);
        drawImmediate();
        glEndList();
    }
}

void Mesh::drawList()
{
    glCallList(displayList);
}

void Mesh::drawImmediate()
{
    glBegin(GL_TRIANGLES);
    for (auto i : triangles)
    {
        for (int j=0; j<3; ++j)
        {
            glTexCoord2f(
                texCoords[i.vertices[j].uv].u,
                texCoords[i.vertices[j].uv].v
            );
            glNormal3f(
                normals[i.vertices[j].norm].x,
                normals[i.vertices[j].norm].y,
                normals[i.vertices[j].norm].z
            );
            glVertex3f(
                vertices[i.vertices[j].pos].x,
                vertices[i.vertices[j].pos].y,
                vertices[i.vertices[j].pos].z
            );
        }
    }
    glEnd();
}

} // namespace Inugami
