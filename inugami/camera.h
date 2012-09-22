#ifndef INUGAMI_CAMERA_H
#define INUGAMI_CAMERA_H

#include "mathtypes.h"

namespace Inugami {

class Camera
{
public:
    Camera();
    virtual ~Camera();

    void translate(float x, float y, float z);
    void relocate(float x, float y, float z);

    void rotate(float angle, float x, float y, float z);
    void pitch(float angle);
    void roll(float angle);
    void yaw(float angle);
    void pitchd(float angle);
    void rolld(float angle);
    void yawd(float angle);

    Matrix<float, 4, 4> getMatrix();

protected:
private:
    Transformation<float> pos;
};

} // namespace Inugami

#endif // INUGAMI_CAMERA_H
