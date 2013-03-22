/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "mesh.hpp"

#include "core.hpp"
#include "exception.hpp"
#include "sharedbank.hpp"
#include "utility.hpp"

namespace Inugami {

class MeshException : public Exception
{
public:
    MeshException(const Mesh* source, std::string error) :
        mesh(source), err(error)
    {}

    virtual const char* what() const noexcept override
    {
        std::string rval;
        rval += "Mesh Exception: ";
        rval += hexify(mesh);
        rval += "; ";
        rval += mesh->id.name;
        rval += "; ";
        rval += err;
        return rval.c_str();
    }

    const Mesh* mesh;
    std::string err;
};

bool Mesh::Vertex::operator==(const Vertex &in)
{
    return (pos==in.pos&&norm==in.norm&&uv==in.uv);
}

bool Mesh::Triangle::operator==(const Triangle &in)
{
    return (v[0]==in.v[0]&&v[1]==in.v[1]&&v[2]==in.v[2]);
}

bool Mesh::Index::operator<(const Index& in) const
{
    return chainComp(name, in.name);
}

Mesh::Value::Value() :
    vertices(), triangles(),
    initted(false),
    vbo(0), vao(0), ele(0),
    users(0)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ele);
}

Mesh::Value::Value(const Value& in) :
    vertices(in.vertices), triangles(in.triangles),
    initted(false),
    vbo(0), vao(0), ele(0),
    users(0)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ele);
}

Mesh::Value::~Value()
{
    glDeleteBuffers(1, &ele);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

int Mesh::Value::addVertex(const Vertex &in)
{
    return addOnce(vertices, in);
}

int Mesh::Value::addTriangle(const Triangle &in)
{
    return addOnce(triangles, in);
}

void Mesh::Value::reserve(int v, int t)
{
    if (v>=0) vertices.reserve(v);
    if (t>=0) triangles.reserve(t);
}

Mesh::Mesh(Core& coreIn) :
    id{hexify(this)},
    val(nullptr),
    bank(coreIn.banks->meshBank)
{
    val = &bank[id];
    ++val->users;
}

Mesh::Mesh(Core& coreIn, const std::string& fileName, bool autoInit) :
    id{fileName},
    val(nullptr),
    bank(coreIn.banks->meshBank)
{
    val = &bank[id];
    if (val->users == 0) loadObjFromFile(fileName, val);
    ++val->users;

    if (autoInit) init();
}

Mesh::Mesh(const Mesh& in) :
    id(in.id),
    val(in.val),
    bank(in.bank)
{
    ++val->users;
}

Mesh::~Mesh()
{
    if (--val->users == 0)
    {
        bank.erase(bank.find(id));
    }
}

const Mesh& Mesh::init() const
{
    val->vertices.shrink_to_fit();
    val->triangles.shrink_to_fit();

    glBindVertexArray(val->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, val->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*val->vertices.size(), &val->vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(::glm::vec3)*2));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, val->ele);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle)*val->triangles.size(), &val->triangles[0], GL_STATIC_DRAW);

    val->initted = true;
    return *this;
}

void Mesh::draw() const
{
    if (!val->initted) throw MeshException(this, "Can't draw; Not initialized!");

    glBindVertexArray(val->vao);
    glDrawElements(GL_TRIANGLES, val->triangles.size()*3, GL_UNSIGNED_INT, 0);
}

int Mesh::addVertex(const Vertex &in) const
{
    return val->addVertex(in);
}

int Mesh::addTriangle(const Triangle &in) const
{
    return val->addTriangle(in);
}

void Mesh::reserve(int v, int t) const
{
    val->reserve(v,t);
}

} // namespace Inugami
