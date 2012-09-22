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
