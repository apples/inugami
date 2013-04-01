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
#include "exception.hpp"
#include "shaderprogram.hpp"

#include <yaml-cpp/yaml.h>

#include <array>
#include <vector>

namespace Inugami {

class ShaderException : public Exception
{
public:
    ShaderException(const std::string &codeStr, const std::string &errStr) :
        code(codeStr), err(errStr)
    {}

    virtual const char* what() const noexcept override
    {
        std::string rval;
        rval += "Shader error:\n";
        rval += "    -- SHADER CODE --\n";
        rval += code;
        rval += "    -- SHADER ERROR --\n";
        rval += err;
        return rval.c_str();
    }

    std::string code;
    std::string err;
};

Shader::Shared::Shared() :
    program(glCreateProgram()),
    users(1)
{}

Shader::Shared::~Shared()
{
    glDeleteProgram(program);
}

Shader::Shader(const ShaderProgram &source) :
    share(new Shared)
{
    auto compile = [](const GLuint id, const std::string &codeStr)
    {
        const GLchar *code = codeStr.c_str();
        const GLchar *codeVec[] = {code};
        glShaderSource(id, 1, codeVec, nullptr);
        glCompileShader(id);
        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei len = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
            std::vector<GLchar> log(len);
            glGetShaderInfoLog(id, 1024, &len, &log[0]);
            throw ShaderException(codeStr, &log[0]);
        }
    };

    static std::array<GLuint, 5> shaderTypes = {
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
    glGetShaderiv(share->program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLsizei len = 0;
        glGetProgramiv(share->program, GL_INFO_LOG_LENGTH, &len);
        if (len>1)
        {
            std::vector<GLchar> log(len);
            glGetProgramInfoLog(share->program, len, &len, &log[0]);
            throw ShaderException("\n", &log[0]);
        }
        else
        {
            throw ShaderException("\n", "UNKNOWN ERROR.\nSORRY.\n");
        }
    }

    for (auto &p : ids)
    {
        glDetachShader(share->program, p);
        glDeleteShader(p);
    }
}

Shader::Shader(const Shader& in) :
    share(in.share)
{
    ++share->users;
}

Shader::Shader(Shader&& in) :
    share(in.share)
{
    in.share = nullptr;
}

Shader::~Shader()
{
    if (--share->users == 0)
    {
        delete share;
    }
}

void Shader::bind() const
{
    glUseProgram(share->program);
}

void Shader::setUniform(const std::string& name, const float val) const
{
    glUniform1f(getUniformLocation(name.c_str()), val);
}

void Shader::setUniform(const std::string& name, const double val) const
{
    setUniform(name, float(val));
}

void Shader::setUniform(const std::string& name, const int val) const
{
    glUniform1i(getUniformLocation(name.c_str()), val);
}

void Shader::setUniform(const std::string& name, const ::glm::vec2 &val) const
{
    glUniform2fv(getUniformLocation(name.c_str()), 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec3 &val) const
{
    glUniform3fv(getUniformLocation(name.c_str()), 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::vec4 &val) const
{
    glUniform4fv(getUniformLocation(name.c_str()), 1, &val[0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat3 &val) const
{
    glUniformMatrix3fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &val[0][0]);
}

void Shader::setUniform(const std::string& name, const ::glm::mat4 &val) const
{
    glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &val[0][0]);
}

GLuint Shader::getUniformLocation(const std::string& name) const
{
    GLint loc = glGetUniformLocation(share->program, name.c_str());
    if (loc == -1) throw ShaderException("", "Could not find uniform "+name);
    return loc;
}

} // namespace Inugami
