/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.2.0
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

#ifndef INUGAMI_INTERFACE_H
#define INUGAMI_INTERFACE_H

#include "opengl.hpp"

#include "core.hpp"

#include <bitset>
#include <list>
#include <vector>
#include <map>

namespace Inugami {

/*! @brief Human input device interface.
 *
 *  This is where all the events from mouse and keyboard are aggregated. Every
 *  Core will have an Interface.
 */
class Interface
{
    friend class Core;
public:
    using Window = Core::Window;

    Interface() = delete;
    Interface(const Interface&) = delete;
    Interface(Interface&&) = delete;

    /*! @brief Primary constructor.
     *
     *  @param windowIn Window to attach.
     */
    Interface(Window windowIn);

    /*! @brief Destructor.
     */
    virtual ~Interface();

    /*! @brief Coordinate template.
     */
    template <typename T> struct Coord
    {
        T x, y;
    };

    /*! @brief Keyboard proxy.
     *
     *  A Proxy can be thought of as a handle to a specific key on the keyboard.
     */
    class Proxy
    {
        friend class Interface;
    public:

        /*! @brief Default constructor.
         */
        Proxy();

        /*! @brief Primary constructor.
         *
         *  Constructs the Proxy to connect to the given Interface and monitor
         *  the given key.
         *
         *  @param inIface Interface to connect.
         *  @param k Key to monitor.
         *
         *  @sa ivk_literals
         */
        Proxy(Interface *const inIface, int k);

        /*! @brief Evaluates to @a true if key is down.
         */
        operator bool() const;

        /*! @brief Returns @a true if key is down.
         *
         *  @return @a True if key is down, else @a false.
         */
        bool down() const;

        /*! @brief Returns @a true if key was just pressed.
         *
         *  @return @a True if key was just pressed, else @a false.
         */
        bool pressed() const;

        /*! @brief Changes the key that is monitored.
         *
         *  @param k New key to monitor.
         *
         *  @sa ivk_literals
         */
        void reassign(int k);

    private:
        Interface *iface;
        int key;
    };

    /*! @brief Checks for new input events.
     *
     *  It is recommended to call this once every frame.
     */
    static void poll();

    /*! @brief Get the key's state.
     *
     *  @param key Keycode.
     *
     *  @return @a True if the key is down, else @a false.
     *
     *  @sa ivk_literals
     */
    bool keyDown(int key) const;

    /*! @brief Get the key's edge state.
     *
     *  @param key Keycode.
     *  @param clr If @a true, resets edge state.
     *
     *  @return @a True if the key was just pressed, else @a false.
     *
     *  @sa ivk_literals
     */
    bool keyPressed(int key, bool clr = false);

    /*! @brief Gets the string buffer of printable keys.
     *
     *  Includes only printable keys that have been pressed since the last call
     *  to getBuffer().
     *
     *  @return Printable keys pressed.
     */
    std::string getBuffer();

    /*! @brief Get the mouse button's state.
     *
     *  @param button Mouse button.
     *
     *  @return @a True if the button is down, else @a false.
     *
     *  @sa ivk_literals
     */
    bool mouseState(int button) const;

    /*! @brief Get the mouse button's edge state.
     *
     *  @param button Mouse button.
     *  @param clr If @a true, resets edge state.
     *
     *
     *  @return @a True if the button is down, else @a false.
     *
     *  @sa ivk_literals
     */
    bool mousePressed(int button, bool clr = false);

    /*! @brief Get the mouse cursor's position.
     *
     *  @return Position in window coordinates.
     */
    Coord<double> getMousePos() const;

    /*! @brief Get the mouse wheel's state.
     *
     *  @return Mouse wheel coordinates.
     */
    Coord<double> getMouseWheel() const;

    /*! @brief Set the mouse cursor's position.
     *
     *  @param x X coordinate.
     *  @param y Y coordinate.
     */
    void setMousePos(double x, double y);

    /*! @brief Set the mouse wheel's position.
     *
     *  @param x X coordinate.
     *  @param y Y coordinate.
     */
    void setMouseWheel(double x, double y);

    /*! @brief Set the mouse cursor's visibility.
     *
     *  If @a true, the system cursor will be shown, and coordinates will be
     *  real. If @a false, system cursor will be hidden, and coordinates will
     *  be virtual.
     *
     *  @param show Visibility.
     */
    void showMouse(bool show) const;

    /*! @brief Creates a Proxy for the given key.
     *
     *  @param k Keycode to monitor.
     *
     *  @return a Proxy for the given keycode.
     *
     *  @sa ivk_literals
     */
    Proxy getProxy(int k);

private:
    template <int m> struct State
    {
        std::bitset<m> states, presses;
    };

    static bool callbacksRegistered;
    static std::map<Window, Interface*> windowMap;

    static void keyboardCallback(Window win, int key, int, int action, int);
    static void unicodeCallback(Window win, unsigned int key);
    static void mouseButtonCallback(Window win, int button, int action, int);
    static void mousePositionCallback(Window win, double x, double y);
    static void mouseWheelCallback(Window win, double x, double y);

    void clearPresses();

    Window window;

    std::string keyBuffer;
    State<GLFW_KEY_LAST+1> keyStates;
    State<GLFW_MOUSE_BUTTON_LAST+1> mouseStates;

    Coord<double> mousePos;
    Coord<double> mouseWheel;
};

} // namespace Inugami

/*! @defgroup ivk_literals Virtual Keycode Literals
 *  @{
 */

/*! @brief Keycode for a printable character.
 *
 *  Converts any printable character, which includes a-z, Enter, Backspace, and
 *  Tab, into a keycode.
 *
 *  @param in Printable character.
 *
 *  @return Keycode.
 */
int operator "" _ivk(char in);

/*! @brief Keycode for keys F1-F25 and ESC.
 *
 *  Converts an integer into a keycode for the Function keys (1-25) and ESC (0).
 *
 *  @param in Function key index.
 *
 *  @return Keycode.
 */
int operator "" _ivkFunc(unsigned long long in);

/*! @brief Keycode for a key on the numpad.
 *
 *  Converts any number and punctuation available on the numpad into a keycode.
 *
 *  @param in Numpad character.
 *
 *  @return Keycode.
 */
int operator "" _ivkNumpad(char in);

/*! @brief Keycode for a number on the numpad.
 *
 *  Converts any number available on the numpad into a keycode.
 *
 *  @param in Numpad number.
 *
 *  @return Keycode.
 */
int operator "" _ivkNumpad(unsigned long long in);

/*! @brief Keycode for Shift keys.
 *
 *  Converts 'L' and 'R' into keycodes for left and right Shift, respectively.
 *
 *  @param in One of 'L' or 'R'.
 *
 *  @return Keycode for Shift key.
 */
int operator "" _ivkShift(char in);

/*! @brief Keycode for Control keys.
 *
 *  Converts 'L' and 'R' into keycodes for left and right Control, respectively.
 *
 *  @param in One of 'L' or 'R'.
 *
 *  @return Keycode for Control key.
 */
int operator "" _ivkControl(char in);

/*! @brief Keycode for Alt keys.
 *
 *  Converts 'L' and 'R' into keycodes for left and right Alt, respectively.
 *
 *  @param in One of 'L' or 'R'.
 *
 *  @return Keycode for Alt key.
 */
int operator "" _ivkAlt(char in);

/*! @brief Keycode for Arrow keys.
 *
 *  Converts as follows:
 *
 *  |Character  | Keycode |
 *  |-----------|---------|
 *  |'N' or 'U' | Up      |
 *  |'S' or 'D' | Down    |
 *  |'W' or 'L' | Left    |
 *  |'E' or 'R' | Right   |
 *
 *  @param in One of the chracters according to the table.
 *
 *  @return Keycode for arrow key.
 */
int operator "" _ivkArrow(char in);

/*! @brief Buttoncode for a mouse button.
 *
 *  Converts 'L', 'R', and 'M' into left, right, and middle mouse buttoncodes,
 *  respectively.
 *
 *  @param in One of 'L', 'R', or 'M'.
 */
int operator "" _ivm(char in);

/*! @}
 */

#endif // INUGAMI_INTERFACE_H
