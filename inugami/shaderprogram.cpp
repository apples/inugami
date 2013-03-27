#include "shaderprogram.hpp"

#include "exception.hpp"
#include "loaders.hpp"

#include <yaml-cpp/yaml.h>

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
        "uniform mat4 projectionMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 modelMatrix;\n"
        "out vec3 Position;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    TexCoord = VertexTexCoord;\n"
        "    Normal = normalize(VertexNormal);\n"
        "    Position = VertexPosition/5;\n"
        "    mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;\n"
        "    gl_Position = modelViewProjectionMatrix * vec4(VertexPosition,1.0);\n"
        "}\n"
    ;
    rval.sources[FRAG] =
        "#version 400\n"
        "in vec3 Position;\n"
        "in vec3 Normal;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D Tex1;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec4 texColor = texture( Tex1, TexCoord );\n"
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

ShaderProgram ShaderProgram::fromYAML(const std::string& in) //static
{
    static std::map<std::string, Type> typeStrings = {
        {"vert", VERT},
        {"tes", TES},
        {"tcs", TCS},
        {"geo", GEO},
        {"frag", FRAG},
    };

    ShaderProgram rval;

    YAML::Node yaml = YAML::LoadFile(in);

    if (!yaml.IsMap()) throw ShaderProgramException("YAML data must be a map!");

    for (auto i=yaml.begin(); i!=yaml.end(); ++i)
    {
        std::string key = i->first.as<std::string>();
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        auto j = typeStrings.find(key);
        if (j == typeStrings.end()) throw ShaderProgramException(std::string("Invalid shader type: ")+key);
        rval.sources[j->second] = loadTextFromFile(i->second.as<std::string>());
    }

    return rval;
}

ShaderProgram::ShaderProgram() :
    sources(5, "")
{}

ShaderProgram::~ShaderProgram()
{}

} // namespace Inugami
