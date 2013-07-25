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

#ifndef INUGAMI_SHADER_H
#define INUGAMI_SHADER_H

#include "inugami.hpp"

#include "exception.hpp"
#include "opengl.hpp"

#include <list>
#include <memory>
#include <string>
#include <map>

namespace Inugami {

/*! @brief Shader exception.
 *
 *  This exception is throw upon compile errors or uniform errors.
 */
class ShaderException : public Exception
{
public:
    ShaderException(const std::string &codeStr, const std::string &errStr);
    virtual const char* what() const noexcept override;
    std::string code;
    std::string err;
};

/*! @brief Shader handle.
 *
 *  This object manages shader programs for OpenGL.
 */
class Shader
{
public:
    /*! @brief Default constructor.
     */
    Shader() = default;

    /*! @brief Primary constructor.
     *
     *  Constructs the Shader from the given ShaderProgram. The ShaderProgram
     *  does not need to contain every type of shader, but at least a vertex
     *  and fragment shader are recommended.
     *
     *  @param in ShaderProgram to upload.
     */
    Shader(const ShaderProgram &in);

    /*! @brief Binds the shader.
     */
    void bind() const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const bool val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const float val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const double val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const int val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const ::glm::vec2 &val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const ::glm::vec3 &val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const ::glm::vec4 &val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const ::glm::mat3 &val) const;

    /*! @brief Sets a uniform variable in the shader.
     *
     *  @param name Name of uniform.
     *  @param val Value to upload.
     */
    void setUniform(const std::string& name, const ::glm::mat4 &val) const;

private:
    struct Uniform
    {
        GLenum type;
        GLint size;
        GLint location;
    };

    class Shared
    {
    public:
        Shared();
        ~Shared();
        GLuint program;
        std::map<std::string,Uniform> uniforms;
    };

    void initUniforms();
    const Uniform& getUniform(const std::string& name) const;

    std::shared_ptr<Shared> share;
};

} // namespace Inugami

#endif // INUGAMI_SHADER_H
