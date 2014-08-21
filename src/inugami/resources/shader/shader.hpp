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

#include "shaderprogram.hpp"

#include "../../opengl.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace Inugami {

namespace Resources_detail {

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

            if (!shader || !data) return false;

            if (!GT::isValidType(data->type)) throw std::logic_error("Invalid type!");

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

} // namespace Resources_detail

using Resources_detail::Shader;

} // namespace Inugami

#endif // INUGAMI_SHADER_H
