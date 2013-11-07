/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef INUGAMI_TRANSFORM_H
#define INUGAMI_TRANSFORM_H

#include "mathtypes.hpp"

#include <vector>

namespace Inugami {

class Transform
{
    typedef std::vector<Mat4> Stack;
public:

    /*! @brief Default constructor.
     */
    Transform();

    /*! @brief Mat4 cast.
     */
    operator Mat4() const;

    /*! @brief Translate.
     *
     *  @param pos Vector to translate along.
     */
    Transform& translate(const Vec3& pos);

    /*! @brief Scale.
     *
     *  @param vec Vector to scale along.
     */
    Transform& scale(const Vec3& vec);

    /*! @brief Rotate.
     *
     *  @param deg Degrees to rotate.
     *  @param axis Axis to rotate along.
     */
    Transform& rotate(float deg, const Vec3& axis);

    /*! @brief Push the stack.
     *
     *  Pushes a copy of the current matrix on the stack.
     */
    Transform& push();

    /*! @brief Multiply and push the stack.
     *
     *  Multiplies the given matrix by the current matrix and pushes the result
     *  onto the stack.
     *
     *  @param mat Matrix to multiply.
     */
    Transform& push(const Mat4& mat);

    /*! @brief Pop the stack.
     *
     *  Pops the current matrix off the stack.
     */
    Transform& pop();

    /*! @brief Resets the stack.
     *
     *  Restores the stack to a single identity matrix.
     */
    Transform& reset();

private:
    Stack stack;
};

} // namespace Inugami

#endif // INUGAMI_TRANSFORM_H
