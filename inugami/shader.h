/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

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
