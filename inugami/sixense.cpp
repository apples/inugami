#include "sixense.hpp"

#include "exception.hpp"
#include "opengl.hpp"

#include <sixense.h>

namespace Inugami {

class SixenseE_Init
    : public Exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "Sixense Error: Failed to initialize!";
    }
};

class SixenseE_Exit
    : public Exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "Sixense Error: Failed to exit!";
    }
};

class SixenseE_NoBase
    : public Exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "Sixense Error: No bases connected!";
    }
};

Sixense& Sixense::inst() //static
{
    static Sixense instance;
    return instance;
}

Sixense::Sixense()
{
    if (sixenseInit() != SIXENSE_SUCCESS) throw SixenseE_Init();

    if (sixenseSetActiveBase(0) != SIXENSE_SUCCESS)
    {
        if (sixenseExit() != SIXENSE_SUCCESS) throw SixenseE_Exit();
        throw SixenseE_NoBase();
    }

    sixenseSetActiveBase(0);
    sixenseGetAllNewestData(&data);
}

Sixense::~Sixense()
{
    if (sixenseExit() != SIXENSE_SUCCESS) throw SixenseE_Exit();
}

void Sixense::setFilter(bool on) const
{
    sixenseSetFilterEnabled(on);
    sixenseSetFilterParams(500.f, 0.f, 5000.f, 1.f);
}

void Sixense::poll()
{
    sixenseGetAllNewestData(&data);
}

glm::vec3 Sixense::getPos(int c) const
{
    return glm::vec3{
          data.controllers[c].pos[0]
        , data.controllers[c].pos[1]
        , data.controllers[c].pos[2]
    };
}

glm::quat Sixense::getRot(int c) const
{
    return glm::quat{
          data.controllers[c].rot_quat[0]
        , data.controllers[c].rot_quat[1]
        , data.controllers[c].rot_quat[2]
        , data.controllers[c].rot_quat[3]
    };
}

bool Sixense::getButton(int c, unsigned int b) const
{
    return (data.controllers[c].buttons&b != 0);
}

glm::vec3 Sixense::getForward(int c) const
{
    return -glm::vec3{
          data.controllers[c].rot_mat[2][0]
        , data.controllers[c].rot_mat[2][1]
        , data.controllers[c].rot_mat[2][2]
    };
}

glm::vec3 Sixense::getUp(int c) const
{
    return glm::vec3{
          data.controllers[c].rot_mat[1][0]
        , data.controllers[c].rot_mat[1][1]
        , data.controllers[c].rot_mat[1][2]
    };
}

float Sixense::getTrigger(int c) const
{
    return data.controllers[c].trigger;
}

float Sixense::getAxisX(int c) const
{
    return data.controllers[c].joystick_x;
}

float Sixense::getAxisY(int c) const
{
    return data.controllers[c].joystick_y;
}

} // namespace Inugami
