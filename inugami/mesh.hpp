/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.2.0
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

#include "inugami.hpp"

#include "opengl.hpp"

#include <memory>

namespace Inugami {

/*! @brief Mesh handle.
 *
 *  This class is basically a wrapper for OpenGL vertex arrays.
 */
class Mesh
{
    friend class MeshException;
    Mesh() = delete;
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

private:
    class Shared
    {
    public:
        Shared();
        ~Shared();

        GLuint vertexBuffer;

        GLuint    pointArray,    pointElements;
        GLuint     lineArray,     lineElements;
        GLuint triangleArray, triangleElements;

        int pointCount, lineCount, triangleCount;
    };

    std::shared_ptr<Shared> share;
};

} // namespace Inugami

#endif // INUGAMI_MESH_H
