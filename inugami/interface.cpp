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

#include "interface.hpp"

#include "exception.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace Inugami {

bool Interface::callbacksRegistered = false;
std::unordered_map<Interface::Window, Interface*> Interface::windowMap;

class InterfaceE_InvalidKey
    : public Exception
{
    std::string err;
public:
    InterfaceE_InvalidKey(int k)
        : err()
    {
        std::stringstream ss;
        ss << "Inugami::Interface Exception: Invalid key code: " << k;
        err = ss.str();
    }

    virtual const char* what() const noexcept override
    {
        return err.c_str();
    }
};

class InterfaceE_InvalidMB
    : public Exception
{
    std::string err;
public:
    InterfaceE_InvalidMB(int k)
        : err()
    {
        std::stringstream ss;
        ss << "Inugami::Interface Exception: Invalid mouse button: " << k;
        err = ss.str();
    }

    virtual const char* what() const noexcept override
    {
        return err.c_str();
    }
};

void Interface::ProxyData::update(bool s)
{
    edge = int(s)-int(state);
    state = s;
}

Interface::Proxy::Proxy(const ProxyData* d)
    : data(d)
{}

Interface::Proxy::operator bool() const
{
    return data->state;
}

bool Interface::Proxy::pressed() const
{
    return (data->edge == 1);
}

bool Interface::Proxy::released() const
{
    return (data->edge == -1);
}

int Interface::Proxy::edge() const
{
    return data->edge;
}

Interface::Interface(Window windowIN)
    : window(windowIN)
    , keyBuffer("")
    , keyStates()
    , mouseStates()
    , mousePos{0.0,0.0}
    , mouseWheel{0.0,0.0}
{
    windowMap[window] = this;

    glfwSetKeyCallback         (window, keyboardCallback);
    glfwSetCharCallback        (window, unicodeCallback);
    glfwSetMouseButtonCallback (window, mouseButtonCallback);
    glfwSetCursorPosCallback   (window, mousePositionCallback);
    glfwSetScrollCallback      (window, mouseWheelCallback);
}

Interface::~Interface()
{
    glfwSetKeyCallback         (window, nullptr);
    glfwSetCharCallback        (window, nullptr);
    glfwSetMouseButtonCallback (window, nullptr);
    glfwSetCursorPosCallback   (window, nullptr);
    glfwSetScrollCallback      (window, nullptr);

    windowMap.erase(windowMap.find(window));
}

void Interface::poll() //static
{
    for (auto&& p : windowMap)
    {
        for (auto&& d : p.second->  keyStates) d.edge = 0;
        for (auto&& d : p.second->mouseStates) d.edge = 0;
    }

    glfwPollEvents();
}

Interface::Proxy Interface::key(int key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        throw InterfaceE_InvalidKey(key);
    }

    return {&keyStates[key]};
}

Interface::Proxy Interface::mouse(int button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        throw InterfaceE_InvalidMB(button);
    }

    return {&mouseStates[button]};
}

auto Interface::getMousePos() const -> Coord<double>
{
    return mousePos;
}

auto Interface::getMouseWheel() const -> Coord<double>
{
    return mouseWheel;
}

void Interface::setMousePos(double x, double y)
{
    mousePos.x = x;
    mousePos.y = y;
    glfwSetCursorPos(window, x, y);
}

void Interface::setMouseWheel(double x, double y)
{
    mouseWheel.x = x;
    mouseWheel.y = y;
}

void Interface::showMouse(bool show) const
{
    if (show) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

bool Interface::getRawKeyState(int key) const
{
    return keyStates[key].state;
}

void Interface::setRawKeyState(int key, bool s)
{
    keyStates[key].update(s);
}

bool Interface::getRawMouseState(int key) const
{
    return mouseStates[key].state;
}

void Interface::setRawMouseState(int key, bool s)
{
    mouseStates[key].update(s);
}

void Interface::keyboardCallback(Window win, int key, int, int action, int) //static
{
    Interface* iface = windowMap[win];

    if (!iface) return; //! @todo Properly check this.

    iface->keyStates[key].update(action != GLFW_RELEASE);
}

void Interface::unicodeCallback(Window win, unsigned int key) //static
{
    if (key > 255) return; //! @todo Unicode support.

    Interface* iface = windowMap[win];
    if (!iface) return; //! @todo Properly check this.

    iface->keyBuffer += char(key);
}

void Interface::mouseButtonCallback(Window win, int button, int action, int) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return; //! @todo Properly check this.

    iface->mouseStates[button].update(action != GLFW_RELEASE);
}

void Interface::mousePositionCallback(Window win, double x, double y) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return; //! @todo Properly check this.

    iface->mousePos.x = x;
    iface->mousePos.y = y;
}

void Interface::mouseWheelCallback(Window win, double x, double y) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return; //! @todo Properly check this.

    iface->mouseWheel.x = x;
    iface->mouseWheel.y = y;
}

} // namespace Inugami

int operator "" _ivk(char in)
{
    if (in >= 'a' && in <= 'z') in = in-'a'+'A';
    if (in == '\n') return GLFW_KEY_ENTER;
    if (in == '\b') return GLFW_KEY_BACKSPACE;
    if (in == '\t') return GLFW_KEY_TAB;
    return int(in);
}

int operator "" _ivkFunc(unsigned long long in)
{
    if (in == 0) return GLFW_KEY_ESCAPE;
    if (in <= 25) return GLFW_KEY_F1-1+in;
    return 0;
}

int operator "" _ivkNumpad(char in)
{
    if (in >= '0' && in <= '9') return GLFW_KEY_KP_0+(in-'0');
    if (in == '+') return GLFW_KEY_KP_ADD;
    if (in == '-') return GLFW_KEY_KP_SUBTRACT;
    if (in == '*') return GLFW_KEY_KP_MULTIPLY;
    if (in == '/') return GLFW_KEY_KP_DIVIDE;
    if (in == '.') return GLFW_KEY_KP_DECIMAL;
    if (in == '=') return GLFW_KEY_KP_EQUAL;
    if (in == '\n') return GLFW_KEY_KP_ENTER;
    return 0;
}

int operator "" _ivkNumpad(unsigned long long in)
{
    if (in <= 9) return GLFW_KEY_KP_0+in;
    return 0;
}

int operator "" _ivkShift(char in)
{
    if (in == 'L' || in == 'l') return GLFW_KEY_LEFT_SHIFT;
    if (in == 'R' || in == 'r') return GLFW_KEY_RIGHT_SHIFT;
    return 0;
}

int operator "" _ivkControl(char in)
{
    if (in == 'L' || in == 'l') return GLFW_KEY_LEFT_CONTROL;
    if (in == 'R' || in == 'r') return GLFW_KEY_RIGHT_CONTROL;
    return 0;
}

int operator "" _ivkAlt(char in)
{
    if (in == 'L' || in == 'l') return GLFW_KEY_LEFT_ALT;
    if (in == 'R' || in == 'r') return GLFW_KEY_RIGHT_ALT;
    return 0;
}

int operator "" _ivkArrow(char in)
{
    if (in >= 'a' && in <= 'z') in = in-'a'+'A';
    if (in == 'U' || in == 'N') return GLFW_KEY_UP;
    if (in == 'D' || in == 'S') return GLFW_KEY_DOWN;
    if (in == 'L' || in == 'W') return GLFW_KEY_LEFT;
    if (in == 'R' || in == 'E') return GLFW_KEY_RIGHT;
    return 0;
}

int operator "" _ivm(char in)
{
    if (in >= 'a' && in <= 'z') in = in-'a'+'A';
    if (in == 'L') return GLFW_MOUSE_BUTTON_LEFT;
    if (in == 'R') return GLFW_MOUSE_BUTTON_RIGHT;
    if (in == 'M') return GLFW_MOUSE_BUTTON_MIDDLE;
    return 0;
}
