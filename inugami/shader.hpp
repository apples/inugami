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

#ifndef INUGAMI_SHADER_H
#define INUGAMI_SHADER_H

#include "inugami.hpp"

#include "exception.hpp"
#include "opengl.hpp"
#include "utility.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

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

class ShaderE_CompileError
    : public ShaderException
{
public:
    ShaderE_CompileError(const std::string &codeStr, const std::string &errStr);
};

class ShaderE_LinkError
    : public ShaderException
{
public:
    ShaderE_LinkError(const std::string &errStr);
};

class ShaderE_UniformTypeError
    : public ShaderException
{
public:
    ShaderE_UniformTypeError();
};

class ShaderE_UniformBindError
    : public ShaderException
{
public:
    ShaderE_UniformBindError();
};

class ShaderE_UniformShaderError
    : public ShaderException
{
public:
    ShaderE_UniformShaderError();
};

/*! @brief Shader handle.
 *
 *  This object manages shader programs for OpenGL.
 */
class Shader
{
    struct UniformData
    {
        GLenum type;
        GLint size;
        GLint location;
    };

public:
    class Uniform
    {
        const Shader* shader;
        const UniformData* data;
    public:
        Uniform();
        Uniform(const Shader* s, const UniformData* u);

        template <typename T>
        bool set(T&& t)
        {
            using GT = GLType<typename std::decay<T>::type>;

            if (!shader) throw ShaderE_UniformShaderError();
            if (!data) return false;

            if (!GT::isValidType(data->type)) throw ShaderE_UniformTypeError();
            if (!shader->isBound()          ) throw ShaderE_UniformBindError();

            GT::uniformFunc(data->location, std::forward<T>(t));
            return true;
        }
    };

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

    /*! @brief Returns @a true if the shader is currently bound.
     */
    bool isBound() const;

    /*! @brief Creates a uniform handle.
     *
     *  @param name Name of uniform.
     */
    Uniform uniform(const std::string& name) const;

private:
    static thread_local GLuint boundProgram;

    class Shared
    {
    public:
        Shared();
        ~Shared();
        GLuint program;
        std::unordered_map<std::string,UniformData> uniforms;
    };

    void initUniforms();
    const UniformData* getUniform(const std::string& name) const;

    std::shared_ptr<Shared> share;
};

} // namespace Inugami

#endif // INUGAMI_SHADER_H
