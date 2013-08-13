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
#include "utility.hpp"

#include <list>
#include <memory>
#include <string>
#include <map>

namespace Inugami {

/*! @brief Shader exception.
 *
 *  This exception is throw upon compile errors or uniform errors.
 */
class ShaderException
    : public Exception
{
public:
    virtual const char* what() const noexcept override;
    std::string err;
};

class ShaderCompileException
    : public ShaderException
{
public:
    ShaderCompileException(const std::string &codeStr, const std::string &errStr);
};

class ShaderLinkException
    : public ShaderException
{
public:
    ShaderLinkException(const std::string &errStr);
};

class ShaderUniformException
    : public ShaderException
{
public:
    ShaderUniformException(const std::string &name);
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
    template <typename T>
    bool setUniform(const std::string& name, T&& val) const;

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
    const Uniform* getUniform(const std::string& name) const;

    std::shared_ptr<Shared> share;
};

template <typename T>
inline bool Shader::setUniform(const std::string& name, T&& val) const
{
    using GT = GLType<typename std::decay<T>::type>;
    const Uniform* uni = getUniform(name);
    if (!uni) return false;
    if (uni->type != GT::value) throw ShaderUniformException(name);
    GT::uniformFunc(uni->location, val);
    return true;
}

template <>
inline bool Shader::setUniform<int>(const std::string& name, int&& val) const
{
    using GT = GLType<int>;
    const Uniform* uni = getUniform(name);
    if (!uni) return false;
    if (    uni->type != GT::value
        &&  uni->type != GL_SAMPLER_1D
        &&  uni->type != GL_SAMPLER_2D
        &&  uni->type != GL_SAMPLER_3D
        &&  uni->type != GL_SAMPLER_CUBE
        &&  uni->type != GL_SAMPLER_1D_SHADOW
        &&  uni->type != GL_SAMPLER_2D_SHADOW
    ) throw ShaderUniformException(name);
    GT::uniformFunc(uni->location, val);
    return true;
}

} // namespace Inugami

#endif // INUGAMI_SHADER_H
