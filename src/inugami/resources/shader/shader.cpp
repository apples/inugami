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

#include "shader.hpp"

#include "shaderprogram.hpp"

#include <array>
#include <sstream>
#include <string>
#include <vector>

namespace Inugami {

namespace Resources_detail {

Shader::Uniform::Uniform()
    : shader(nullptr)
    , data(nullptr)
{}

Shader::Uniform::Uniform(const Shader* s, const UniformData* u)
    : shader(s)
    , data(u)
{}

Shader::Shared::Shared()
    : program(glCreateProgram())
    , uniforms()
{}

Shader::Shared::~Shared()
{
    glDeleteProgram(program);
}

Shader::Shader(const ShaderProgram &source)
    : share(std::make_shared<Shared>())
{
    auto compile = [](const GLuint id, const std::string &codeStr)
    {
        const GLchar* code = codeStr.c_str();
        const GLchar* codeVec[] = {code};
        glShaderSource(id, 1, codeVec, nullptr);
        glCompileShader(id);
        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLsizei len = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
            std::vector<GLchar> log(len);
            glGetShaderInfoLog(id, 1024, &len, &log[0]);
            throw std::runtime_error(&log[0]);
        }
    };

    static constexpr GLuint shaderTypes[5] = {
        GL_VERTEX_SHADER,
        GL_TESS_CONTROL_SHADER,
        GL_TESS_EVALUATION_SHADER,
        GL_GEOMETRY_SHADER,
        GL_FRAGMENT_SHADER
    };

    std::vector<GLuint> ids;

    for (int i=0; i<5; ++i)
    {
        if (source.sources[i] != "")
        {
            GLuint shader = glCreateShader(shaderTypes[i]);
            ids.push_back(shader);
            compile(shader, source.sources[i]);
        }
    }

    for (auto &p : ids) glAttachShader(share->program, p);
    glLinkProgram(share->program);

    for (auto &p : ids)
    {
        glDetachShader(share->program, p);
        glDeleteShader(p);
    }

    GLint status;
    glGetProgramiv(share->program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLsizei len = 0;
        glGetProgramiv(share->program, GL_INFO_LOG_LENGTH, &len);
        if (len>=1)
        {
            std::vector<GLchar> log(len);
            glGetProgramInfoLog(share->program, len, &len, &log[0]);
            throw std::runtime_error(&log[0]);
        }
    }

    initUniforms();
}

void Shader::bind() const
{
    glUseProgram(share->program);
}

Shader::Uniform Shader::uniform(const std::string& name) const
{
    auto iter = share->uniforms.find(name);
    if (iter == share->uniforms.end()) return Uniform(this, nullptr);
    return Uniform(this, &iter->second);
}

void Shader::initUniforms()
{
    GLint numUniforms, maxLength;

    glGetProgramiv(share->program, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(share->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

    GLchar* name = new GLchar[maxLength];
    UniformData tmpUniform;
    for (int i=0; i<numUniforms; ++i)
    {
        glGetActiveUniform(share->program, i, maxLength, nullptr, &tmpUniform.size, &tmpUniform.type, name);
        tmpUniform.location = glGetUniformLocation(share->program, name);
        share->uniforms[name] = tmpUniform;
    }
}

} // namespace Resources_detail

} // namespace Inugami
