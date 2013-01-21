/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "shader.h"

#include "loaders.h"

#include <stdexcept>

namespace Inugami {

Shader::Shader(const Program &source)
{
    auto compile = [](const GLuint id, const std::string &codeStr)
    {
        const GLchar *code = codeStr.c_str();
        const GLchar *codeVec[] = {code};
        glShaderSource(id, 1, codeVec, nullptr);
        glCompileShader(id);
        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        return (status == GL_TRUE);
    };

    static std::map<Type, GLint> shaderTypes = {
        {Type::VERT, GL_VERTEX_SHADER},
        {Type::TCS, GL_TESS_CONTROL_SHADER},
        {Type::TES, GL_TESS_EVALUATION_SHADER},
        {Type::GEO, GL_GEOMETRY_SHADER},
        {Type::FRAG, GL_FRAGMENT_SHADER}
    };

    std::map<Type, GLuint> ids;

    for (auto& p : source)
    {
        ids[p.first] = glCreateShader(shaderTypes[p.first]);
        if (!compile(ids[p.first], p.second))
        {
            throw std::runtime_error("Failed to compile shader!");
        }
    }

    program = glCreateProgram();
    for (auto &p : ids) glAttachShader(program, p.second);
    glLinkProgram(program);
    for (auto &p : ids) glDeleteShader(p.second);

    GLint status;
    glGetShaderiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) throw std::runtime_error("Failed to link shader!");
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::bind() const
{
    glUseProgram(program);
}

void Shader::setUniform(const std::string& name, const float val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1f(loc, val);
}

void Shader::setUniform(const std::string& name, const int val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform1i(loc, val);
}

void Shader::setUniform(const std::string& name, const ::glm::vec2 &val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform2fv(loc, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec3 &val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform3fv(loc, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec4 &val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniform4fv(loc, 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat3 &val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat4 &val) const
{
    GLint loc = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
}

} // namespace Inugami
