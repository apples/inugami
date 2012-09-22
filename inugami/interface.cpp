#include "interface.h"

namespace Inugami {

std::list<char> Interface::keyBuffer;
Interface::State<GLFW_KEY_LAST+1> Interface::kbStates;
Interface::State<GLFW_MOUSE_BUTTON_LAST+1> Interface::mouseState;
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
    verifyStates();
    glfwPollEvents();
}

bool Interface::keyState(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    return kbStates.states[key];
}

bool Interface::keyPressed(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    if (kbStates.presses[key])
    {
        kbStates.presses[key] = false;
        kbStates.pressResets[key] = false;
        return true;
    }
    return false;
}

bool Interface::kbGetBuffer(std::string *target) //static
{
    if (keyBuffer.size() < 1) return false;
    target->clear();
    for (auto i : keyBuffer) *target += i;
    keyBuffer.clear();
    return true;
}

bool Interface::mbState(int button) //static
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    return mouseState.states[button];
}

bool Interface::mbPressed(int button) //static
{
    if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return false;
    if (mouseState.presses[button])
    {
        mouseState.presses[button] = false;
        mouseState.pressResets[button] = false;
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
    kbStates.presses.reset();
    mouseState.presses.reset();
}

void Interface::verifyStates() //static
{
    kbStates.verify();
    mouseState.verify();
}

void GLFWCALL Interface::keyboardCallback(int key, int action) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS)
    {
        kbStates.states[key] = true;
        kbStates.presses[key] = true;
        kbStates.pressResets[key] = true;
        if (key<256) keyBuffer.push_back(static_cast<char>(key));
        if (key>=GLFW_KEY_KP_0 && key<=GLFW_KEY_KP_9) keyBuffer.push_back(static_cast<char>(key-GLFW_KEY_KP_0+'0'));
    }
    else if (action == GLFW_RELEASE)
    {
        kbStates.stateResets[key] = true;
    }
}

void GLFWCALL Interface::mouseButtonCallback(int button, int action) //static
{
    //if (button<0 || button>GLFW_MOUSE_BUTTON_LAST) return; //NOTE Not necessary?
    if (action == GLFW_PRESS)
    {
        mouseState.states[button] = true;
        mouseState.presses[button] = true;
        mouseState.pressResets[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        mouseState.stateResets[button] = true;
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
