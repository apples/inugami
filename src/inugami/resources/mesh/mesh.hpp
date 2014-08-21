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

#ifndef INUGAMI_MESH_H
#define INUGAMI_MESH_H

#include "geometry.hpp"

#include "../../opengl.hpp"

#include <memory>

namespace Inugami {

namespace Resources_detail {

GLuint make_vbo_id();
void destroy_vbo_id(GLuint id);

GLuint make_vao_id();
void destroy_vao_id(GLuint id);

template <GLuint(*make)(), void(*destroy)(GLuint)>
class GLObject
{
    GLuint id;

    public:

        GLObject()
            : id(make())
        {}

        GLObject(GLObject const&) = delete;
        GLObject& operator=(GLObject const&) = delete;

        GLObject(GLObject&& other)
            : id(std::exchange(other.id, 0))
        {}

        GLObject& operator=(GLObject&& other)
        {
            destroy(id);
            id = std::exchange(other.id, 0);
            return *this;
        }

        ~GLObject()
        {
            destroy(id);
        }

        GLuint get() const
        {
            return id;
        }
};

using VBO = GLObject<make_vbo_id, destroy_vbo_id>;
using VAO = GLObject<make_vao_id, destroy_vao_id>;

/*! @brief Mesh handle.
 *
 *  This class is basically a wrapper for OpenGL vertex arrays.
 */
class Mesh
{
    struct Shared
    {
        VBO vertices;
        VBO pointIndices;
        VBO lineIndices;
        VBO triangleIndices;
        VAO points;
        VAO lines;
        VAO triangles;
        int pointCount = 0;
        int lineCount = 0;
        int triangleCount = 0;
    };

    std::shared_ptr<Shared> share;

public:

    /*! @brief Primary constructor.
     *
     *  Uploads a Geometry to the GPU. The Geometry can be safely deleted after
     *  construction.
     *
     *  @param in Geometry to upload.
     */
    Mesh(const Geometry& in);

    /*! @brief Draws the Mesh.
     */
    void draw() const;
};

} // namespace Resources_detail

using Resources_detail::Mesh;

} // namespace Inugami

#endif // INUGAMI_MESH_H
