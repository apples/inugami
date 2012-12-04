#ifndef INUGAMI_SHADER_H
#define INUGAMI_SHADER_H

#include "opengl.h"

#include <list>
#include <string>
#include <map>

namespace Inugami {

class Shader
{
public:
    enum class Type
    {
        VERT, TCS, TES, GEO, FRAG
    };

    struct OPOPP
    {
        int a, b;
        float c;
    };

    typedef std::map<Type, std::string> Program;

    Shader(const Program &in);
    ~Shader();

    void bind();

//    void setUniform(const char *name, const bool val);
//    void setUniform(const char *name, const float val);
//    void setUniform(const char *name, const int val);
//    void setUniform(const char *name, const Vec2 &val);
//    void setUniform(const char *name, const Vec3 &val);
//    void setUniform(const char *name, const Vec4 &val);
//    void setUniform(const char *name, const Mat3 &val);
//    void setUniform(const char *name, const Mat4 &val);

private:
    GLuint program;
};

} // namespace Inugami

#endif // INUGAMI_SHADER_H
