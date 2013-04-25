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

#include "shaderprogram.hpp"

#include "exception.hpp"
#include "loaders.hpp"

#include <algorithm>
#include <fstream>
#include <map>

namespace Inugami {

class ShaderProgramException : public Exception
{
public:
    ShaderProgramException(const std::string &errStr) :
        err(errStr)
    {}

    virtual const char* what() const noexcept override
    {
        std::string rval;
        rval += "ShaderProgram error: ";
        rval += err;
        return rval.c_str();
    }

    std::string err;
};

ShaderProgram ShaderProgram::fromDefault() //static
{
    ShaderProgram rval;

    rval.sources[VERT] =
        "#version 400\n"
        "layout (location = 0) in vec3 VertexPosition;\n"
        "layout (location = 1) in vec3 VertexNormal;\n"
        "layout (location = 2) in vec2 VertexTexCoord;\n"
        "uniform mat4 MVP;\n"
        "out vec3 Position;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    TexCoord = VertexTexCoord;\n"
        "    Normal = normalize(VertexNormal);\n"
        "    Position = VertexPosition;\n"
        "    gl_Position = MVP * vec4(VertexPosition,1.0);\n"
        "}\n"
    ;
    rval.sources[FRAG] =
        "#version 400\n"
        "in vec3 Position;\n"
        "in vec3 Normal;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D Tex0;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec4 texColor = texture( Tex0, TexCoord );\n"
        "    FragColor = texColor;\n"
        "}\n"
    ;

    return rval;
}

ShaderProgram ShaderProgram::fromName(std::string in) //static
{
    static std::map<std::string, Type> typeStrings = {
        {"vert", VERT},
        {"tes", TES},
        {"tcs", TCS},
        {"geo", GEO},
        {"frag", FRAG},
    };

    ShaderProgram rval;

    if (in[in.length()-1] != '.') in += '.';

    for (auto i=typeStrings.begin(); i!=typeStrings.end(); ++i)
    {
        try
        {
            rval.sources[i->second] = loadTextFromFile(in+i->first);
        }
        catch (...)
        {
            rval.sources[i->second] = "";
        }
    }

    return rval;
}

ShaderProgram::ShaderProgram() :
    sources(5, "")
{}

ShaderProgram::~ShaderProgram()
{}

} // namespace Inugami
