/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "interface.h"

#include <algorithm>

namespace Inugami {

bool Interface::callbacksRegistered;
std::map<GLFWwindow, Interface*> Interface::windowMap;

Interface::Proxy::Proxy() :
    iface(nullptr),
    key(0)
{}

Interface::Proxy::Proxy(Interface *const inIface, int k) :
    iface(inIface),
    key(k)
{}

Interface::Proxy::operator bool() const
{
    return down();
}

bool Interface::Proxy::down() const
{
    if (!iface) return false;
    return iface->keyDown(key);
}

bool Interface::Proxy::pressed() const
{
    if (!iface) return false;
    return iface->keyPressed(key);
}

void Interface::Proxy::reassign(int k)
{
    key = k;
}

Interface::Interface(GLFWwindow windowIN) :
    window(windowIN)
{
    windowMap[window] = this;

    if (!callbacksRegistered)
    {
        glfwSetKeyCallback(window, keyboardCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, mousePositionCallback);
        glfwSetScrollCallback(window, mouseWheelCallback);
        callbacksRegistered = true;
    }
}

Interface::~Interface()
{}

void Interface::poll() //static
{
    for (auto& p : windowMap) if (p.second) p.second->clearPresses();
    glfwPollEvents();
}

bool Interface::keyDown(int key) const
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    return keyStates.states[key];
}

bool Interface::keyPressed(int key, bool clr)
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    if (keyStates.presses.test(key))
    {
        if (clr) keyStates.presses.reset(key);
        return true;
    }
    return false;
}

std::string Interface::getBuffer()
{
    std::string tmp = keyBuffer;
    keyBuffer = "";
    return tmp;
}

bool Interface::mouseState(int button) const
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    return mouseStates.states[button];
}

bool Interface::mousePressed(int button, bool clr)
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    if (mouseStates.presses[button])
    {
        if (clr) mouseStates.presses[button] = false;
        return true;
    }
    return false;
}

auto Interface::getMousePos() const -> Coord<int>
{
    return mousePos;
}

auto Interface::getMouseWheel() const -> Coord<double>
{
    return mouseWheel;
}

void Interface::setMousePos(int x, int y)
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
    if (show) glfwSetInputMode(window, GLFW_CURSOR_MODE, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode(window, GLFW_CURSOR_MODE, GLFW_CURSOR_HIDDEN);
}

auto Interface::getProxy(int k) -> Proxy
{
    return Proxy(this, k);
}

void Interface::clearPresses()
{
    keyStates.presses.reset();
    mouseStates.presses.reset();
}

void Interface::keyboardCallback(GLFWwindow win, int key, int action) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return;
    if (key<0 || key>GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS)
    {
        iface->keyStates.states.set(key);
        iface->keyStates.presses.set(key);
        if (key<256) iface->keyBuffer += static_cast<char>(key);
        if (key>=GLFW_KEY_KP_0 && key<=GLFW_KEY_KP_9) iface->keyBuffer += static_cast<char>(key-GLFW_KEY_KP_0+'0');
    }
    else if (action == GLFW_RELEASE)
    {
        iface->keyStates.states.reset(key);
    }
}

void Interface::mouseButtonCallback(GLFWwindow win, int button, int action) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return;
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return;
    if (action == GLFW_PRESS)
    {
        iface->mouseStates.states[button] = true;
        iface->mouseStates.presses[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        iface->mouseStates.states[button] = false;
    }
}

void Interface::mousePositionCallback(GLFWwindow win, int x, int y) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return;
    iface->mousePos.x = x;
    iface->mousePos.y = y;
}

void Interface::mouseWheelCallback(GLFWwindow win, double x, double y) //static
{
    Interface* iface = windowMap[win];
    if (!iface) return;
    iface->mouseWheel.x = x;
    iface->mouseWheel.y = y;
}

} // namespace Inugami
