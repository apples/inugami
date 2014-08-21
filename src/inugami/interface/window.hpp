#ifndef INUGAMI_INTERFACE_WINDOW_HPP
#define INUGAMI_INTERFACE_WINDOW_HPP

#include "../opengl.hpp"

#include "input.hpp"
#include "tracker.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Inugami {
namespace Interface_detail {

struct WindowParams
{
    int width = 800;
    int height = 600;
    int samples = 0;

    struct
    {
        int major = 3;
        int minor = 2;
    } gl_version;

    std::string title = "Inugami Window";

    bool fullscreen = false;
    bool vsync = false;
};

class Window
{
    friend Window makeWindow(WindowParams wp);
    friend void pollEvents();

    using WindowHandle = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>;

    static void keyCB(GLFWwindow* handle, int key, int, int action, int);
    static void charCB(GLFWwindow* handle, unsigned key);
    static void mouseButtonCB(GLFWwindow* handle, int button, int action, int);
    static void cursorPosCB(GLFWwindow* handle, double x, double y);
    static void scrollCB(GLFWwindow* handle, double x, double y);

    WindowParams parameters;
    WindowHandle handle;

    std::u32string characterBuffer;
    std::vector<KeyData> keys;
    std::vector<KeyData> mouseButtons;
    Mouse mouse;

    TrackerHandle tracker;

    public:

        Window(WindowParams wp, Tracker* tp);
        Window(const Window&) = delete;
        Window(Window&&) = default;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void setTitle(std::string const& title);

        KeyHandle getKey(Key k) const;
        Mouse const& getMouse() const;

        void setMouse(Mouse m);

        void activate();

        void swapBuffers();

        void clearBuffer();

        bool shouldClose() const;
};

} // namespace Interface_detail

using Interface_detail::WindowParams;
using Interface_detail::Window;

} // namespace Inugami

#endif // INUGAMI_WINDOW_HPP
