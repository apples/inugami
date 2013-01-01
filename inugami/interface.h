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

namespace Inugami {

class Interface
{
    friend class Key;
public:
    class Key
    {
        friend class Interface;
    public:
        Key(Interface *const inIface, int k);
        ~Key();
        bool down();
        bool pressed();
        void setKey(int k);
    private:
        class Index
        {
        public:
            Index(int k);
            bool operator==(int k);
            bool operator<(int k);
            int key;
            bool down, pressed;
            int users;
        };
        typedef std::vector<Index> List;
        Interface *iface;
        List::iterator i;
    };

    Interface();
    virtual ~Interface();

    static void poll();

    static bool keyDown(int key);
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

    Key::List::iterator getWatch(int k);
    void dropWatch(Key::List::iterator &i);

    Key::List watches;
};

} // namespace Inugami

#endif // INUGAMI_INTERFACE_H
