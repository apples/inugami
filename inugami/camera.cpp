/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "camera.hpp"

namespace Inugami {

Camera::Camera() :
    cullFaces(false), depthTest(false),
    projection(1.0f),
    view(1.0f)
{}

Camera::~Camera()
{}

Camera& Camera::perspective(float fov, float aspect, float near, float far)
{
    projection = ::glm::perspective(fov, aspect, near, far);
    return *this;
}

Camera& Camera::ortho(float left, float right, float bottom, float top, float near, float far)
{
    projection = ::glm::ortho(left, right, bottom, top, near, far);
    return *this;
}

Camera& Camera::translate(const Vec3& pos)
{
    view = ::glm::translate(view, pos);
    return *this;
}

Camera& Camera::rotate(float deg, const Vec3& axis)
{
    view = ::glm::rotate(view, deg, axis);
    return *this;
}

Camera& Camera::pitch(float deg)
{
    view = ::glm::rotate(view, deg, Vec3(0.f, 1.f, 0.f));
    return *this;
}

Camera& Camera::yaw(float deg)
{
    view = ::glm::rotate(view, deg, Vec3(1.f, 0.f, 0.f));
    return *this;
}

Camera& Camera::roll(float deg)
{
    view = ::glm::rotate(view, deg, Vec3(0.f, 0.f, 1.f));
    return *this;
}

auto Camera::getProjection() const -> const Mat4&
{
    return projection;
}

auto Camera::getView() const -> const Mat4&
{
    return view;
}

} // namespace Inugami
