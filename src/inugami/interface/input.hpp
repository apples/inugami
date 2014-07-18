#ifndef INUGAMI_INTERFACE_INPUT_HPP
#define INUGAMI_INTERFACE_INPUT_HPP

#include "../opengl.hpp"
#include <string>

namespace Inugami {
namespace Interface_detail {

using namespace std;

struct MouseCoord
{
    double x;
    double y;
};

struct Mouse
{
    MouseCoord pos;
    MouseCoord wheel;
};

struct KeyData
{
    int presses = 0;
    int releases = 0;
    bool is_down = false;
};

class KeyHandle
{
    const KeyData* data = nullptr;

    public:

        KeyHandle() = default;
        KeyHandle(const KeyData* kd);

        bool down() const;
        bool pressed() const;
        bool released() const;

        explicit operator bool() const;
};

struct Key
{
    enum Type
    {
        KEYBOARD,
        MOUSE,
    };

    int value = GLFW_KEY_UNKNOWN;
    Type type = KEYBOARD;

    static Key fromName(const string& str);
    static const string& getName(const Key& k);

    static Key character(char c);
    static Key numpad(int i);
    static Key numpad(string const& str);
    static Key function(int i);
    static Key mouse(int i);
    static Key mouse(string const& str);
    static Key esc();
};

} // namespace Interface_detail

using Interface_detail::Mouse;
using Interface_detail::KeyHandle;
using Interface_detail::Key;

} // namespace Inugami

#endif // INUGAMI_INTERFACE_INPUT_HPP
