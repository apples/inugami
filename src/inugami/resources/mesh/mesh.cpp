/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "mesh.hpp"

#include "geometry.hpp"

#include "../../opengl.hpp"

#include <sstream>
#include <string>

namespace Inugami {

namespace Resources_detail {

GLuint make_vbo_id()
{
    GLuint rv;
    glGenBuffers(1, &rv);
    return rv;
}

void destroy_vbo_id(GLuint id)
{
    glDeleteBuffers(1, &id);
}

GLuint make_vao_id()
{
    GLuint rv;
    glGenVertexArrays(1, &rv);
    return rv;
}

void destroy_vao_id(GLuint id)
{
    glDeleteVertexArrays(1, &id);
}

template <class Container>
static void initVertexArray(GLuint vertexArray, GLuint indexArray, const Container& data)
{
    auto setAttribute = [](GLuint index, GLint size, std::size_t offset) {
        GLvoid* offset_v = reinterpret_cast<GLvoid*>(offset);
        return glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), offset_v);
    };

    auto databytes = sizeof(typename Container::value_type)*data.size();

    glBindVertexArray(vertexArray);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    setAttribute(0, 3, offsetof(Geometry::Vertex, pos));
    setAttribute(1, 3, offsetof(Geometry::Vertex, norm));
    setAttribute(2, 2, offsetof(Geometry::Vertex, tex));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArray);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, databytes, &data[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

static void drawVertexArray(GLuint vertexArray, GLuint mode, GLuint elementCount)
{
    glBindVertexArray(vertexArray);
    glDrawElements(mode, elementCount, GL_UNSIGNED_INT, nullptr);
}

Mesh::Mesh(const Geometry& in)
    : share(std::make_shared<Shared>())
{
    glBindBuffer(GL_ARRAY_BUFFER, share->vertices.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::Vertex)*in.vertices.size(), &in.vertices[0], GL_STATIC_DRAW);

    initVertexArray(share->   points.get(), share->   pointIndices.get(), in.points);
    initVertexArray(share->    lines.get(), share->    lineIndices.get(), in.lines);
    initVertexArray(share->triangles.get(), share->triangleIndices.get(), in.triangles);

    share->pointCount    = in.points   .size();
    share->lineCount     = in.lines    .size()*2;
    share->triangleCount = in.triangles.size()*3;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw() const
{
    drawVertexArray(share->triangles.get(), GL_TRIANGLES, share->triangleCount);
    drawVertexArray(share->    lines.get(), GL_LINES,     share->lineCount);
    drawVertexArray(share->   points.get(), GL_POINTS,    share->pointCount);
}

} // namespace Resources_detail

} // namespace Inugami
