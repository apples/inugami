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

#ifndef INUGAMI_SHADERPROGRAM_H
#define INUGAMI_SHADERPROGRAM_H

#include <string>
#include <vector>

namespace Inugami {

/*! @brief Container for a shader program.
 *
 *  This object is designed to be sent to a Shader for uploading to a GPU.
 */
class ShaderProgram
{
public:

    /*! @brief Type of shader.
     *
     *  | Type | Shader                 |
     *  |------|------------------------|
     *  | VERT | Vertex                 |
     *  | TCS  | Tesselation Control    |
     *  | TES  | Tesselation Evaluation |
     *  | GEO  | Geometry               |
     *  | FRAG | Fragment               |
     */
    enum Type
    {
        VERT, TCS, TES, GEO, FRAG
    };

    /*! @brief Creates default shader.
     *
     *  The created shader is a very basic shader that has very limited
     *  functionality.
     *
     *  @return Basic shader.
     */
    static ShaderProgram fromDefault();

    /*! @brief Creates shader from files.
     *
     *  Files beginning with the given prefix and ending with specific
     *  extensions (.vert, .tcs, .tes, .geo, and .frag) are loaded.
     *
     *  @param in File prefix.
     *
     *  @return %Shader.
     */
    static ShaderProgram fromName(std::string in);

    /*! @brief Default constructor.
     */
    ShaderProgram();

    /*! @brief Destructor.
     */
    virtual ~ShaderProgram();

    /*! @brief 5-element vector of.
     */
    std::vector<std::string> sources;
};

} // namespace Inugami

#endif // INUGAMI_SHADERPROGRAM_H
