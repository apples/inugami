/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_INTERFACE_H
#define INUGAMI_INTERFACE_H

#include "opengl.h"

#include <bitset>
#include <list>
#include <vector>
#include <map>

namespace Inugami {

class Interface
{
public:
    template <typename T> struct Coord
    {
        T x, y;
    };

    class Proxy
    {
        friend class Interface;
    public:
        Proxy();
        Proxy(Interface *const inIface, int k);

        operator bool() const;

        bool down() const;
        bool pressed() const;
        void reassign(int k);

    private:
        Interface *iface;
        int key;
    };

    static void poll();

    Interface() = delete;
    Interface(GLFWwindow windowIN);
    virtual ~Interface();

    bool keyDown(int key) const;
    bool keyPressed(int key, bool clr = false);
    std::string getBuffer();

    bool mouseState(int button) const;
    bool mousePressed(int button, bool clr = false);

    Coord<int> getMousePos() const;
    Coord<double> getMouseWheel() const;

    void setMousePos(int x, int y);
    void setMouseWheel(double x, double y);

    void showMouse(bool show) const;

    Proxy getProxy(int k);

private:
    template <int m> struct State
    {
        std::bitset<m> states, presses;
    };

    static bool callbacksRegistered;
    static std::map<GLFWwindow, Interface*> windowMap;

    static void keyboardCallback(GLFWwindow win, int key, int action);
    static void mouseButtonCallback(GLFWwindow win, int button, int action);
    static void mousePositionCallback(GLFWwindow win, int x, int y);
    static void mouseWheelCallback(GLFWwindow win, double x, double y);

    void clearPresses();

    GLFWwindow window;

    std::string keyBuffer;
    State<GLFW_KEY_LAST+1> keyStates;
    State<GLFW_MOUSE_BUTTON_LAST+1> mouseStates;

    Coord<int> mousePos;
    Coord<double> mouseWheel;
};

} // namespace Inugami

#endif // INUGAMI_INTERFACE_H
