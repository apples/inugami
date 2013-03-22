/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_TRANSFORM_H
#define INUGAMI_TRANSFORM_H

#include "opengl.hpp"

#include <vector>

namespace Inugami {

using Vec3 = ::glm::vec3;
using Mat4 = ::glm::mat4;

class Transform
{
    typedef std::vector<Mat4> Stack;

public:
    Transform();
    Transform(const Transform& in);
    virtual ~Transform();

    operator Mat4() const;

    Transform& translate(const Vec3& pos);
    Transform& scale(const Vec3& vec);
    Transform& rotate(float deg, const Vec3& axis);

    Transform& push();
    Transform& pop();
    Transform& reset();

    Mat4 toMat4() const;

private:
    Stack stack;
};

} // namespace Inugami

#endif // INUGAMI_TRANSFORM_H
