#include "interface.h"

#include <algorithm>

namespace Inugami {

std::list<char> Interface::keyBuffer;
Interface::State<GLFW_KEY_LAST+1> Interface::keyStates;
Interface::State<GLFW_MOUSE_BUTTON_LAST+1> Interface::mouseStates;
decltype(Interface::mousePos) Interface::mousePos; //NOTE LOL I DIDN'T THINK THIS WOULD ACTUALLY WORK

bool Interface::callbacksRegistered;

Interface::Key::Key(Interface *const inIface, int k) :
    iface(inIface)
{
    i = iface->getWatch(k);
}

Interface::Key::~Key()
{
    iface->dropWatch(i);
}

bool Interface::Key::down()
{
    return i->down;
}

bool Interface::Key::pressed()
{
    return i->pressed;
}

void Interface::Key::setKey(int k)
{
    iface->dropWatch(i);
    i = iface->getWatch(k);
}

Interface::Key::Index::Index(int k) :
    key(k),
    down(false), pressed(false),
    users(1)
{}

bool Interface::Key::Index::operator==(int k)
{
    return (key==k);
}

bool Interface::Key::Index::operator<(int k)
{
    return (key<k);
}

void Interface::verifyStates()
{

}

Interface::Interface()
{
    if (!callbacksRegistered)
    {
        glfwSetKeyCallback(keyboardCallback);
        glfwSetMouseButtonCallback(mouseButtonCallback);
        glfwSetMousePosCallback(mousePositionCallback);
        glfwSetMouseWheelCallback(mouseWheelCallback);
        callbacksRegistered = true;

        glfwDisable(GLFW_AUTO_POLL_EVENTS); // VERY IMPORTANT
    }
}

Interface::~Interface()
{}

void Interface::poll() //static
{
    clearPresses();
    glfwPollEvents();
}

bool Interface::keyDown(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    return keyStates.states[key];
}

bool Interface::keyPressed(int key) //static
{
    if (key<0 || key>GLFW_KEY_LAST) return false;
    if (keyStates.presses.test(key))
    {
        keyStates.presses.reset(key);
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
        //mouseStates.presses[button] = false;
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
        keyStates.states.set(key);
        keyStates.presses.set(key);
        if (key<256) keyBuffer.push_back(static_cast<char>(key));
        if (key>=GLFW_KEY_KP_0 && key<=GLFW_KEY_KP_9) keyBuffer.push_back(static_cast<char>(key-GLFW_KEY_KP_0+'0'));
    }
    else if (action == GLFW_RELEASE)
    {
        keyStates.states.reset(key);
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

auto Interface::getWatch(int k) -> Key::List::iterator
{
    auto i = std::lower_bound(watches.begin(), watches.end(), k);
    if (i != watches.end() && *i == k)
    {
        ++i->users;
    }
    else
    {
        i = watches.insert(i, Key::Index(k));
    }
    return i;
}

void Interface::dropWatch(Key::List::iterator &i)
{
    if (--i->users == 0)
    {
        watches.erase(i);
    }
}

} // namespace Inugami
