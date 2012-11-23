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
    static bool kbGetBuffer(std::string *target);

    static bool mbState(int button);
    static bool mbPressed(int button);

    static int mouseX();
    static int mouseY();
    static int mouseWheel();

    static void setMousePos(int x, int y);
    static void setMouseWheel(int pos);

    static void showMouse(bool show);

private:
    template <int m> struct State
    {
        std::bitset<m> states, presses, stateResets, pressResets;
        void verify()
        {
            states ^= stateResets;
            presses ^= pressResets;
            stateResets.reset();
            pressResets.reset();
        }
    };

    static void clearPresses();
    static void verifyStates();

    static std::list<char> keyBuffer;
    static State<GLFW_KEY_LAST+1> kbStates;
    static State<GLFW_MOUSE_BUTTON_LAST+1> mouseState;
    static struct {int x, y, w;} mousePos;

    static bool callbacksRegistered;

    static void GLFWCALL keyboardCallback(int key, int action);
    static void GLFWCALL mouseButtonCallback(int button, int action);
    static void GLFWCALL mousePositionCallback(int x, int y);
    static void GLFWCALL mouseWheelCallback(int pos);
};

} // namespace Inugami

#endif // INUGAMI_INTERFACE_H
