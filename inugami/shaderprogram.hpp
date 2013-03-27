#ifndef INUGAMI_SHADERPROGRAM_H
#define INUGAMI_SHADERPROGRAM_H

#include <string>
#include <vector>

namespace Inugami {

class ShaderProgram
{
public:
    enum Type
    {
        VERT, TCS, TES, GEO, FRAG
    };

    static ShaderProgram fromDefault();
    static ShaderProgram fromName(std::string in);
    static ShaderProgram fromYAML(const std::string& in);

    ShaderProgram();
    virtual ~ShaderProgram();

    std::vector<std::string> sources;
};

} // namespace Inugami

#endif // INUGAMI_SHADERPROGRAM_H
