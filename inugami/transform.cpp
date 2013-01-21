#include "transform.h"

namespace Inugami {

Transform::Transform() :
    stack()
{}

Transform::~Transform()
{}

Transform::operator Mat4() const
{
    return toMat4();
}

Transform& Transform::translate(const Vec3& pos)
{
    stack.back() = ::glm::translate(toMat4(), pos);
    return *this;
}

Transform& Transform::scale(const Vec3& vec)
{
    stack.back() = ::glm::scale(toMat4(), vec);
    return *this;
}

Transform& Transform::rotate(float deg, const Vec3& axis)
{
    stack.back() = ::glm::rotate(toMat4(), deg, axis);
    return *this;
}

Transform& Transform::push()
{
    stack.push_back(toMat4());
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
    return *this;
}

auto Transform::toMat4() const -> Mat4
{
    if (stack.size() == 0) return Mat4(1.f);
    return stack.back();
}

} // namespace Inugami
