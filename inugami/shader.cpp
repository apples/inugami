/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
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

#include "shader.hpp"

#include "loaders.hpp"
#include "shaderprogram.hpp"

#include <array>
#include <vector>

namespace Inugami {

ShaderException::ShaderException(const std::string &codeStr, const std::string &errStr)
    : code(codeStr)
    , err("")
{
    err += "Shader error:";
    err += errStr;
    if (code != "")
    {
        err += "\n    -- SHADER CODE --\n";
        err += code;
    }
}

const char* ShaderException::what() const noexcept
{
    return err.c_str();
}

Shader::Shared::Shared()
    : program(glCreateProgram())
    , uniforms()
{}

Shader::Shared::~Shared()
{
    glDeleteProgram(program);
}

Shader::Shader(const ShaderProgram &source)
    : share(new Shared)
{
    auto compile = [](const GLuint id, const std::string &codeStr)
    {
        const GLchar *code = codeStr.c_str();
        const GLchar *codeVec[] = {code};
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
            throw ShaderException(codeStr, &log[0]);
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
            throw ShaderException("\n", &log[0]);
        }
    }

    for (auto &p : ids)
    {
        glDetachShader(share->program, p);
        glDeleteShader(p);
    }

    initUniforms();
}

void Shader::bind() const
{
    glUseProgram(share->program);
}

void Shader::setUniform(const std::string& name, const float val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniform1f(uni.location, val);
}

void Shader::setUniform(const std::string& name, const double val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_DOUBLE) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniform1d(uni.location, val);
}

void Shader::setUniform(const std::string& name, const int val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type == GL_INT
    ||  uni.type == GL_SAMPLER_1D
    ||  uni.type == GL_SAMPLER_2D
    ||  uni.type == GL_SAMPLER_3D
    ||  uni.type == GL_SAMPLER_CUBE
    ||  uni.type == GL_SAMPLER_1D_SHADOW
    ||  uni.type == GL_SAMPLER_2D_SHADOW)
    {
        glUniform1i(uni.location, val);
    }
    else
    {
        throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    }
}

void Shader::setUniform(const std::string& name, const ::glm::vec2 &val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT_VEC2) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniform2fv(uni.location, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec3 &val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT_VEC3) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniform3fv(uni.location, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec4 &val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT_VEC4) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniform4fv(uni.location, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat3 &val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT_MAT3) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniformMatrix3fv(uni.location, 1, GL_FALSE, &val[0][0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat4 &val) const
{
    const Uniform& uni = getUniform(name);
    if (uni.type != GL_FLOAT_MAT4) throw ShaderException("", "Uniform "+name+" is of incorrect type.");
    glUniformMatrix4fv(uni.location, 1, GL_FALSE, &val[0][0]);
}

void Shader::initUniforms()
{
    GLint numUniforms, maxLength;

    glGetProgramiv(share->program, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(share->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

    GLchar* name = new GLchar[maxLength];
    Uniform tmpUniform;
    for (int i=0; i<numUniforms; ++i)
    {
        glGetActiveUniform(share->program, i, maxLength, nullptr, &tmpUniform.size, &tmpUniform.type, name);
        tmpUniform.location = glGetUniformLocation(share->program, name);
        share->uniforms[name] = tmpUniform;
    }
}

const Shader::Uniform& Shader::getUniform(const std::string& name) const
{
    auto iter = share->uniforms.find(name);
    if (iter == share->uniforms.end())
    {
        throw ShaderException("", "Could not find uniform "+name);
    }
    return iter->second;
}

} // namespace Inugami
