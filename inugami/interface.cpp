#include "interface.h"

namespace Inugami {

std::list<char> Interface::keyBuffer;
Interface::State<GLFW_KEY_LAST+1> Interface::keyStates;
Interface::State<GLFW_MOUSE_BUTTON_LAST+1> Interface::mouseStates;
decltype(Interface::mousePos) Interface::mousePos; //NOTE LOL I DIDN'T THINK THIS WOULD ACTUALLY WORK

bool Interface::callbacksRegistered;

Interface::Interface()
{
    if (!callbacksRegistered)
    {
        glfwSetKeyCallback(keyboardCallback);
        glfwSetMouseButtonCallback(mouseButtonCallback);
        glfwSetMousePosCallback(mousePositionCallback);
        glfwSetMouseWheelCallback(mouseWheelCallback);
        callbacksRegistered = true;
    }
}

Interface::~Interface()
{
    //dtor
}

void Interface::poll() //static
{
    clearPresses();
    glfwPollEvents();
}

bool Interface::keyState(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    return keyStates.states[key];
}

bool Interface::keyPressed(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    if (keyStates.presses[key])
    {
        keyStates.presses[key] = false;
        return true;
    }
    return false;
}

bool Interface::getBuffer(std::string *target) //static
{
    if (keyBuffer.size() < 1) return false;
    target->clear();
    for (auto i : keyBuffer) *target += i;
    keyBuffer.clear();
    return true;
}

bool Interface::mouseState(int button) //static
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    return mouseStates.states[button];
}

bool Interface::mousePressed(int button) //static
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    if (mouseStates.presses[button])
    {
        mouseStates.presses[button] = false;
        return true;
    }
    return false;
}

int Interface::mouseX() //static
{
    return mousePos.x;
}

int Interface::mouseY() //static
{
    return mousePos.y;
}

int Interface::mouseWheel() //static
{
    return mousePos.w;
}

void Interface::setMousePos(int x, int y) //static
{
    mousePos.x = x;
    mousePos.y = y;
    glfwSetMousePos(x, y);
}

void Interface::setMouseWheel(int pos) //static
{
    mousePos.w = pos;
}

void Interface::showMouse(bool show)
{
    if (show) glfwEnable(GLFW_MOUSE_CURSOR);
    else glfwDisable(GLFW_MOUSE_CURSOR);
}

void Interface::clearPresses() //static
{
    keyStates.presses.reset();
    mouseStates.presses.reset();
}

void GLFWCALL Interface::keyboardCallback(int key, int action) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS)
    {
        keyStates.states[key] = true;
        keyStates.presses[key] = true;
        if (key<256) keyBuffer.push_back(static_cast<char>(key));
        if (key>=GLFW_KEY_KP_0 && key<=GLFW_KEY_KP_9) keyBuffer.push_back(static_cast<char>(key-GLFW_KEY_KP_0+'0'));
    }
    else if (action == GLFW_RELEASE)
    {
        keyStates.states[key] = false;
    }
}

void GLFWCALL Interface::mouseButtonCallback(int button, int action) //static
{
    //if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return; //NOTE Not necessary?
    if (action == GLFW_PRESS)
    {
        mouseStates.states[button] = true;
        mouseStates.presses[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        mouseStates.states[button] = false;
    }
}

void GLFWCALL Interface::mousePositionCallback(int x, int y) //static
{
    mousePos.x = x;
    mousePos.y = y;
}

void GLFWCALL Interface::mouseWheelCallback(int pos) //static
{
    mousePos.w = pos;
}

} // namespace Inugami
