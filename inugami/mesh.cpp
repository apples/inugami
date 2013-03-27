/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "mesh.hpp"

#include "exception.hpp"
#include "geometry.hpp"
#include "utility.hpp"

#include <string>

using namespace std;

namespace Inugami {

template <class Container>
static void initVertexArray(GLuint vertexArray, GLuint elementArray, const Container& data)
{
    glBindVertexArray(vertexArray);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid*)(sizeof(Geometry::Vec3)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), (GLvoid*)(sizeof(Geometry::Vec3)*2));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(typename Container::value_type)*data.size(), &data[0], GL_STATIC_DRAW);
}

static void drawVertexArray(GLuint vertexArray, GLuint mode, GLuint elementCount)
{
    if (elementCount > 0)
    {
        glBindVertexArray(vertexArray);
        glDrawElements(mode, elementCount, GL_UNSIGNED_INT, 0);
    }
}

class MeshException : public Exception
{
public:
    MeshException(const Mesh* source, string error) :
        mesh(source), err(error)
    {}

    virtual const char* what() const noexcept override
    {
        string rval;
        rval += "Mesh Exception: ";
        rval += hexify(mesh);
        rval += "; ";
        rval += err;
        return rval.c_str();
    }

    const Mesh* mesh;
    string err;
};

Mesh::Shared::Shared() :
    vertexBuffer(0),
    pointArray(0), pointElements(0),
    lineArray(0), lineElements(0),
    triangleArray(0), triangleElements(0),
    pointCount(0), lineCount(0), triangleCount(0),
    users(1)
{
    glGenBuffers(1, &vertexBuffer);
    glGenVertexArrays(1, &pointArray);
    glGenVertexArrays(1, &lineArray);
    glGenVertexArrays(1, &triangleArray);
    glGenBuffers(1, &pointElements);
    glGenBuffers(1, &lineElements);
    glGenBuffers(1, &triangleElements);
}

Mesh::Shared::~Shared()
{
    glDeleteBuffers(1, &triangleElements);
    glDeleteBuffers(1, &lineElements);
    glDeleteBuffers(1, &pointElements);
    glDeleteVertexArrays(1, &triangleArray);
    glDeleteVertexArrays(1, &lineArray);
    glDeleteVertexArrays(1, &pointArray);
    glDeleteBuffers(1, &vertexBuffer);
}

Mesh::Mesh(const Geometry& in) :
    share(new Shared)
{
    glBindBuffer(GL_ARRAY_BUFFER, share->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::Vertex)*in.vertices.size(), &in.vertices[0], GL_STATIC_DRAW);

    initVertexArray(share->pointArray, share->pointElements, in.points);
    initVertexArray(share->lineArray, share->lineElements, in.lines);
    initVertexArray(share->triangleArray, share->triangleElements, in.triangles);

    share->pointCount = in.points.size();
    share->lineCount = in.lines.size()*2;
    share->triangleCount = in.triangles.size()*3;
}

Mesh::Mesh(const Mesh& in) :
    share(in.share)
{
    ++share->users;
}

Mesh::Mesh(Mesh&& in) :
    share(in.share)
{
    in.share = nullptr;
}

Mesh& Mesh::operator=(const Mesh& in)
{
    if (share && --share->users == 0)
    {
        delete share;
    }

    share = in.share;
    ++share->users;

    return *this;
}

Mesh& Mesh::operator=(Mesh&& in)
{
    if (share && --share->users == 0)
    {
        delete share;
    }

    share = in.share;
    in.share = nullptr;

    return *this;
}

Mesh::~Mesh()
{
    if (share && --share->users == 0)
    {
        delete share;
    }
}

void Mesh::draw() const
{
    drawVertexArray(share->triangleArray, GL_TRIANGLES, share->triangleCount);
    drawVertexArray(share->lineArray, GL_LINES, share->lineCount);
    drawVertexArray(share->pointArray, GL_POINTS, share->pointCount);
}

} // namespace Inugami
