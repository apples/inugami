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

#include "opengl.hpp"

#include "core.hpp"

#include <bitset>
#include <list>
#include <vector>
#include <map>

namespace Inugami {

class Interface
{
    friend class Core;
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

    using Window = Core::Window;

    static void poll();

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
    Interface() = delete;
    Interface(Window windowIN);
    virtual ~Interface();

    template <int m> struct State
    {
        std::bitset<m> states, presses;
    };

    static bool callbacksRegistered;
    static std::map<Window, Interface*> windowMap;

    static void keyboardCallback(Window win, int key, int action);
    static void unicodeCallback(Window win, int key);
    static void mouseButtonCallback(Window win, int button, int action);
    static void mousePositionCallback(Window win, int x, int y);
    static void mouseWheelCallback(Window win, double x, double y);

    void clearPresses();

    Window window;

    std::string keyBuffer;
    State<GLFW_KEY_LAST+1> keyStates;
    State<GLFW_MOUSE_BUTTON_LAST+1> mouseStates;

    Coord<int> mousePos;
    Coord<double> mouseWheel;
};

} // namespace Inugami

int operator "" _ivk(char in);
int operator "" _ivkFunc(unsigned long long in);
int operator "" _ivkNumpad(char in);
int operator "" _ivkNumpad(unsigned long long in);
int operator "" _ivkShift(char in);
int operator "" _ivkControl(char in);
int operator "" _ivkAlt(char in);
int operator "" _ivkArrow(char in);

int operator "" _ivm(char in);

#endif // INUGAMI_INTERFACE_H
