#ifndef OPENGL_HPP_INCLUDED
#define OPENGL_HPP_INCLUDED

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Inugami {

template <typename T>
struct GLType;

template <>
struct GLType<char>
{
    using type = GLbyte;
    static constexpr auto value = GL_BYTE;
};

template <>
struct GLType<short>
{
    using type = GLshort;
    static constexpr auto value = GL_SHORT;
};

template <>
struct GLType<int>
{
    using type = GLint;
    static constexpr auto value = GL_INT;
    static bool isValidType(GLenum t)
    {
        static constexpr GLenum uniformValues[] = {
              GL_INT
            , GL_BOOL
            , GL_SAMPLER_1D
            , GL_SAMPLER_2D
            , GL_SAMPLER_3D
            , GL_SAMPLER_CUBE
            , GL_SAMPLER_1D_SHADOW
            , GL_SAMPLER_2D_SHADOW
        };
        for (auto&& i : uniformValues) if (t==i) return true;
        return false;
    }
    static void uniformFunc(GLint loc, int val)
    {
        return glUniform1i(loc, val);
    }
};

template <>
struct GLType<unsigned char>
{
    using type = GLubyte;
    static constexpr auto value = GL_UNSIGNED_BYTE;
};

template <>
struct GLType<unsigned short>
{
    using type = GLushort;
    static constexpr auto value = GL_UNSIGNED_SHORT;
};

template <>
struct GLType<unsigned>
{
    using type = GLuint;
    static constexpr auto value = GL_UNSIGNED_INT;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, unsigned val)
    {
        return glUniform1ui(loc, val);
    }
};

template <>
struct GLType<float>
{
    using type = GLfloat;
    static constexpr auto value = GL_FLOAT;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, float val)
    {
        return glUniform1f(loc, val);
    }
};

template <>
struct GLType<double>
{
    using type = GLdouble;
    static constexpr auto value = GL_DOUBLE;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, double val)
    {
        return glUniform1d(loc, val);
    }
};

template <>
struct GLType<glm::vec2>
{
    static constexpr auto value = GL_FLOAT_VEC2;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::vec2& val)
    {
        return glUniform2fv(loc, 1, &val[0]);
    }
};

template <>
struct GLType<glm::vec3>
{
    static constexpr auto value = GL_FLOAT_VEC3;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::vec3& val)
    {
        return glUniform3fv(loc, 1, &val[0]);
    }
};

template <>
struct GLType<glm::vec4>
{
    static constexpr auto value = GL_FLOAT_VEC4;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::vec4& val)
    {
        return glUniform4fv(loc, 1, &val[0]);
    }
};

template <>
struct GLType<glm::mat2>
{
    static constexpr auto value = GL_FLOAT_MAT2;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::mat2& val)
    {
        return glUniformMatrix2fv(loc, 1, GL_FALSE, &val[0][0]);
    }
};

template <>
struct GLType<glm::mat3>
{
    static constexpr auto value = GL_FLOAT_MAT3;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::mat3& val)
    {
        return glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
    }
};

template <>
struct GLType<glm::mat4>
{
    static constexpr auto value = GL_FLOAT_MAT4;
    static bool isValidType(GLenum t)
    {
        return (t==value);
    }
    static void uniformFunc(GLint loc, const glm::mat4& val)
    {
        return glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
    }
};

} // namespace Inugami

#endif // OPENGL_HPP_INCLUDED
