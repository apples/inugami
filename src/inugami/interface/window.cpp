#include "window.hpp"

#include <utility>
#include <stdexcept>

namespace Inugami {
namespace Interface_detail {

namespace { // globals

namespace Errors {

constexpr char FAILED_TO_INIT_GLFW[] = "Inugami::Window: Failed to initialize!";
constexpr char FAILED_TO_OPEN[] = "Inugami::Window: Failed to open window!";

} // namespace Errors

Window& getWindow(GLFWwindow* handle)
{
    return *static_cast<Window*>(glfwGetWindowUserPointer(handle));
}

} // globals

void Window::keyCB(GLFWwindow* handle, int key, int, int action, int) // static
{
    if (action == GLFW_REPEAT)
        return;

    Window& window = getWindow(handle);
    auto& k = window.keys[key];
    bool pressed = (action == GLFW_PRESS);
    if (pressed)
        ++k.presses;
    else
        ++k.releases;
    k.is_down = pressed;
}

void Window::charCB(GLFWwindow* handle, unsigned key) // static
{
    Window& window = getWindow(handle);
    window.characterBuffer.push_back(key);
}

void Window::mouseButtonCB(GLFWwindow* handle, int button, int action, int) // static
{
    Window& window = getWindow(handle);
    auto& k = window.mouseButtons[button];
    bool pressed = (action == GLFW_PRESS);
    if (pressed)
        ++k.presses;
    else
        ++k.releases;
    k.is_down = pressed;
}

void Window::cursorPosCB(GLFWwindow* handle, double x, double y) // static
{
    Window& window = getWindow(handle);
    window.mouse.pos.x = x;
    window.mouse.pos.y = y;
}

void Window::scrollCB(GLFWwindow* handle, double x, double y) // static
{
    Window& window = getWindow(handle);
    window.mouse.wheel.x += x;
    window.mouse.wheel.y += y;
}

Window::Window(WindowParams wp, Tracker* tp)
    : parameters(wp)
    , handle(nullptr, glfwDestroyWindow)
    , characterBuffer()
    , keys(GLFW_KEY_LAST+1)
    , mouseButtons(GLFW_MOUSE_BUTTON_LAST+1)
    , mouse()
    , tracker(tp, this)
{
    if (glfwInit() != GL_TRUE)
        throw std::runtime_error(Errors::FAILED_TO_INIT_GLFW);

    GLFWmonitor* monitor = nullptr;

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, wp.samples);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, wp.gl_version.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, wp.gl_version.minor);

    if (wp.fullscreen)
        monitor = glfwGetPrimaryMonitor();

    handle.reset(glfwCreateWindow(wp.width, wp.height, wp.title.c_str(), monitor, nullptr));

    if (!handle)
        throw std::runtime_error(Errors::FAILED_TO_OPEN);

    auto raw = handle.get();

    glfwSetWindowUserPointer(raw, this);

    glfwSetKeyCallback(raw, keyCB);
    glfwSetCharCallback(raw, charCB);
    glfwSetMouseButtonCallback(raw, mouseButtonCB);
    glfwSetCursorPosCallback(raw, cursorPosCB);
    glfwSetScrollCallback(raw, scrollCB);

    glewExperimental = GL_TRUE;
    glewInit();

    tp->window = this;
}

void Window::setTitle(string const& title)
{
    glfwSetWindowTitle(handle.get(), title.c_str());
}

KeyHandle Window::getKey(Key k) const
{
    switch (k.type)
    {
        case Key::Type::KEYBOARD:
        {
            return {&keys[k.value]};
        } break;

        case Key::Type::MOUSE:
        {
            return {&mouseButtons[k.value]};
        } break;
    }

    return {nullptr};
}

Mouse const& Window::getMouse() const
{
    return mouse;
}

void Window::setMouse(Mouse m)
{
    mouse = m;
    glfwSetCursorPos(handle.get(), m.pos.x, m.pos.y);
}

void Window::activate()
{
    glfwMakeContextCurrent(handle.get());
}

void Window::swapBuffers()
{
    glfwSwapBuffers(handle.get());
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(handle.get());
}

} // namespace Interface_detail
} // namespace Inugami
