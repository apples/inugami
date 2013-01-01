/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "camera.h"

#include "math.h"

namespace Inugami {

Camera::Camera()
{
    //ctor
}

Camera::~Camera()
{
    //dtor
}

void Camera::translate(float x, float y, float z)
{
    pos.loc -= Vector<float, 3>(x, y, z);
}

void Camera::relocate(float x, float y, float z)
{
    pos.loc = Vector<float, 4>(x, y, z, 1.0);
}

void Camera::rotate(float angle, float x, float y, float z)
{
    pos.rot = quatFromAxisAngle(angle, Vector<float, 3>(x, y, z)) * pos.rot;
}

void Camera::pitch(float angle)
{
    pos.rot = quatFromAxisAngle(angle, Vector<float, 3>(1.0, 0.0, 0.0)) * pos.rot;
}

void Camera::roll(float angle)
{
    pos.rot = quatFromAxisAngle(angle, Vector<float, 3>(0.0, 0.0, 1.0)) * pos.rot;
}

void Camera::yaw(float angle)
{
    pos.rot = quatFromAxisAngle(angle, Vector<float, 3>(0.0, 1.0, 0.0)) * pos.rot;
}

void Camera::pitchd(float angle)
{
    pos.rot = quatFromAxisAngle(toRadians(angle), Vector<float, 3>(1.0, 0.0, 0.0)) * pos.rot;
}

void Camera::rolld(float angle)
{
    pos.rot = quatFromAxisAngle(toRadians(angle), Vector<float, 3>(0.0, 0.0, 1.0)) * pos.rot;
}

void Camera::yawd(float angle)
{
    pos.rot = quatFromAxisAngle(toRadians(angle), Vector<float, 3>(0.0, 1.0, 0.0)) * pos.rot;
}

Matrix<float, 4, 4> Camera::getMatrix()
{
    return pos.getMatrix();
}

} // namespace Inugami
