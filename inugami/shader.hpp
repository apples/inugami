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

#include "inugami.hpp"

#include "opengl.hpp"

#include <list>
#include <string>
#include <map>

namespace Inugami {

class Shader
{
public:
    Shader() = delete;
    Shader(const ShaderProgram &in);
    Shader(const Shader& in);
    Shader(Shader&& in);
    virtual ~Shader();

    Shader& operator=(const Shader& in);
    Shader& operator=(Shader&& in);

    void bind() const;

    void setUniform(const std::string& name, const bool val) const;
    void setUniform(const std::string& name, const float val) const;
    void setUniform(const std::string& name, const double val) const;
    void setUniform(const std::string& name, const int val) const;
    void setUniform(const std::string& name, const ::glm::vec2 &val) const;
    void setUniform(const std::string& name, const ::glm::vec3 &val) const;
    void setUniform(const std::string& name, const ::glm::vec4 &val) const;
    void setUniform(const std::string& name, const ::glm::mat3 &val) const;
    void setUniform(const std::string& name, const ::glm::mat4 &val) const;

private:
    class Shared
    {
    public:
        Shared();
        ~Shared();
        GLuint program;
        int users;
    };

    GLuint getUniformLocation(const std::string& name) const;

    Shared* share;
};

} // namespace Inugami

#endif // INUGAMI_SHADER_H
