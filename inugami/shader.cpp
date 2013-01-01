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

    std::map<Type, GLuint> ids;

    Program::const_iterator i;
    if ((i=source.find(Type::VERT)) != source.end())
    {
        ids[Type::VERT] = glCreateShader(GL_VERTEX_SHADER);
        if (!compile(ids[Type::VERT], i->second))
        {
            throw std::runtime_error("Failed to compile vertex shader!");
        }
    }
    if ((i=source.find(Type::TCS)) != source.end())
    {
        ids[Type::TCS] = glCreateShader(GL_TESS_CONTROL_SHADER);
        if (!compile(ids[Type::TCS], i->second))
        {
            throw std::runtime_error("Failed to compile TCS shader!");
        }
    }
    if ((i=source.find(Type::TES)) != source.end())
    {
        ids[Type::TES] = glCreateShader(GL_TESS_EVALUATION_SHADER);
        if (!compile(ids[Type::TES], i->second))
        {
            throw std::runtime_error("Failed to compile TES shader!");
        }
    }
    if ((i=source.find(Type::GEO)) != source.end())
    {
        ids[Type::GEO] = glCreateShader(GL_GEOMETRY_SHADER);
        if (!compile(ids[Type::GEO], i->second))
        {
            throw std::runtime_error("Failed to compile geometry shader!");
        }
    }
    if ((i=source.find(Type::FRAG)) != source.end())
    {
        ids[Type::FRAG] = glCreateShader(GL_FRAGMENT_SHADER);
        if (!compile(ids[Type::FRAG], i->second))
        {
            throw std::runtime_error("Failed to compile fragment shader!");
        }
    }

    program = glCreateProgram();
    for (auto &p : ids) glAttachShader(program, p.second);
    glLinkProgram(program);
    for (auto &p : ids) glDeleteShader(p.second);

    GLint status;
    glGetShaderiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) throw std::runtime_error("Failed to link program!");
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::bind()
{
    glUseProgram(program);
}

} // namespace Inugami
