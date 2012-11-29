#ifndef INUGAMI_INTERFACE_H
#define INUGAMI_INTERFACE_H

#include "opengl.h"

#include <bitset>
#include <list>

namespace Inugami {

class Interface
{
public:
    Interface();
    virtual ~Interface();

    static void poll();

    static bool keyState(int key);
    static bool keyPressed(int key);
    static bool getBuffer(std::string *target);

    static bool mouseState(int button);
    static bool mousePressed(int button);

    static int mouseX();
    static int mouseY();
    static int mouseWheel();

    static void setMousePos(int x, int y);
    static void setMouseWheel(int pos);

    static void showMouse(bool show);

private:
    template <int m> struct State
    {
        std::bitset<m> states, presses;
    };

    static void clearPresses();
    static void verifyStates();

    static std::list<char> keyBuffer;
    static State<GLFW_KEY_LAST+1> keyStates;
    static State<GLFW_MOUSE_BUTTON_LAST+1> mouseStates;
    static struct {int x, y, w;} mousePos;

    static bool callbacksRegistered;

    static void keyboardCallback(int key, int action);
    static void mouseButtonCallback(int button, int action);
    static void mousePositionCallback(int x, int y);
    static void mouseWheelCallback(int pos);
};

} // namespace Inugami

#endif // INUGAMI_INTERFACE_H
