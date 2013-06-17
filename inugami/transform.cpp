/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
 * Version: 0.2.0
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

#include "transform.hpp"

namespace Inugami {

Transform::Transform()
    : stack{Mat4(1.f)}
{}

Transform::Transform(const Transform& in)
    : stack(in.stack)
{}

Transform::Transform(Transform&& in)
    : stack(move(in.stack))
{}

Transform::~Transform()
{}

Transform::operator Mat4() const
{
    return stack.back();
}

Transform& Transform::translate(const Vec3& pos)
{
    stack.back() = ::glm::translate(stack.back(), pos);
    return *this;
}

Transform& Transform::scale(const Vec3& vec)
{
    stack.back() = ::glm::scale(stack.back(), vec);
    return *this;
}

Transform& Transform::rotate(float deg, const Vec3& axis)
{
    stack.back() = ::glm::rotate(stack.back(), deg, axis);
    return *this;
}

Transform& Transform::push()
{
    stack.push_back(stack.back());
    return *this;
}

Transform& Transform::pop()
{
    stack.pop_back();
    return *this;
}

Transform& Transform::reset()
{
    stack.clear();
    stack.emplace_back(1.f);
    return *this;
}

} // namespace Inugami
