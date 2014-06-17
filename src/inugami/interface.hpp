/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
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

#include <array>
#include <list>
#include <vector>
#include <unordered_map>

namespace Inugami {

/*! @brief Human input device interface.
 *
 *  This is where all the events from mouse and keyboard are aggregated. Every
 *  Core will have an Interface.
 */
class Interface
{
    friend class Core;

    class ProxyData
    {
    public:
        void update(bool s);

        unsigned char edge;
        bool state;
    };

public:
    using Window = Core::Window;

    /*! @brief Key proxy.
     */
    class Proxy
    {
        friend Interface;
        Proxy(const ProxyData* d);
    public:
        Proxy(const Proxy&) = default;
        Proxy(Proxy&&) = default;

        /*! @brief Evaluate state.
         *
         *  @return @a True if the key is down, @a false otherwise.
         */
        explicit operator bool() const;

        /*! @brief Down edge detection.
         *
         *  @return @a True if the key was just pressed, @a false otherwise.
         */
        bool pressed() const;

        /*! @brief Up edge detection.
         *
         *  @return @a True if the key was just released, @a false otherwise.
         */
        bool released() const;

        /*! @brief Edge detection.
         *
         *  @return 1 if the key was just pressed, -1 if released, 0 otherwise.
         */
        int edge() const;

    private:
        const ProxyData* data;
    };

    /*! @brief Coordinate template.
     */
    template <typename T> struct Coord
    {
        T x, y;
    };

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

    /*! @brief Checks for new input events.
     *
     *  It is recommended to call this once every frame.
     */
    static void poll();

    /*! @brief Get the key's state.
     *
     *  @param key Keycode.
     *
     *  @return A Proxy to the key.
     *
     *  @sa ivk_literals
     */
    Proxy key(int key) const;

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
     *  @return A Proxy to the mouse button.
     *
     *  @sa ivk_literals
     */
    Proxy mouse(int button) const;

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

    /*! @brief Get key state directly.
     *
     *  @param key Keycode.
     *
     *  @return @a True if the key is down, @a false otherwise.
     *
     *  @sa ivk_literals
     */
    bool getRawKeyState(int key) const;

    /*! @brief Set key state directly.
     *
     *  Simulates a key press or release.
     *
     *  @param key Key code.
     *  @param s New key state.
     *
     *  @sa ivk_literals
     */
    void setRawKeyState(int key, bool s);

    /*! @brief Get mouse button state directly.
     *
     *  @param button Mouse button.
     *
     *  @return @a True if the button is down, @a false otherwise.
     *
     *  @sa ivk_literals
     */
    bool getRawMouseState(int button) const;

    /*! @brief Set mouse button state directly.
     *
     *  Simulates a mouse button press or release.
     *
     *  @param button Mouse button.
     *  @param s New button state.
     *
     *  @sa ivk_literals
     */
    void setRawMouseState(int button, bool s);

private:
    static bool callbacksRegistered;
    static std::unordered_map<Window, Interface*> windowMap;

    static void keyboardCallback(Window win, int key, int, int action, int);
    static void unicodeCallback(Window win, unsigned int key);
    static void mouseButtonCallback(Window win, int button, int action, int);
    static void mousePositionCallback(Window win, double x, double y);
    static void mouseWheelCallback(Window win, double x, double y);

    void clearPresses();

    Window window;

    std::string keyBuffer;
    std::array<ProxyData, GLFW_KEY_LAST+1>            keyStates;
    std::array<ProxyData, GLFW_MOUSE_BUTTON_LAST+1> mouseStates;

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
