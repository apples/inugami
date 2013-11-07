#ifndef INUGAMI_SIXENSE_HPP
#define INUGAMI_SIXENSE_HPP

#include "inugami.hpp"

#include "opengl.hpp"

#include <sixense.h>

namespace Inugami {

class Sixense
{
    Sixense();

public:
    static Sixense& inst();

    Sixense(const Sixense&) = delete;
    ~Sixense();

    void setFilter(bool on) const;

    void poll();

    glm::vec3 getPos(int c) const;
    glm::quat getRot(int c) const;
    bool getButton(int c, unsigned int b) const;

    glm::vec3 getForward(int c) const;
    glm::vec3 getUp(int c) const;

    float getTrigger(int c) const;
    float getAxisX(int c) const;
    float getAxisY(int c) const;

private:
    sixenseAllControllerData data;
};

} // namespace Inugami

#endif // INUGAMI_SIXENSE_HPP
