#include "input.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace Inugami {
namespace Interface_detail {

namespace { // globals

using KTEntry = pair<string, int>;

const KTEntry KEY_TABLE[] = {
    {"SPACE", GLFW_KEY_SPACE},
    {"APOSTROPHE", GLFW_KEY_APOSTROPHE},
    {"COMMA", GLFW_KEY_COMMA},
    {"MINUS", GLFW_KEY_MINUS},
    {"PERIOD", GLFW_KEY_PERIOD},
    {"SLASH", GLFW_KEY_SLASH},
    {"0", GLFW_KEY_0},
    {"1", GLFW_KEY_1}, {"2", GLFW_KEY_2}, {"3", GLFW_KEY_3},
    {"4", GLFW_KEY_4}, {"5", GLFW_KEY_5}, {"6", GLFW_KEY_6},
    {"7", GLFW_KEY_7}, {"8", GLFW_KEY_8}, {"9", GLFW_KEY_9},
    {"SEMICOLON", GLFW_KEY_SEMICOLON},
    {"EQUAL", GLFW_KEY_EQUAL},
    {"A", GLFW_KEY_A}, {"B", GLFW_KEY_B}, {"C", GLFW_KEY_C}, {"D", GLFW_KEY_D},
    {"E", GLFW_KEY_E}, {"F", GLFW_KEY_F}, {"G", GLFW_KEY_G}, {"H", GLFW_KEY_H},
    {"I", GLFW_KEY_I}, {"J", GLFW_KEY_J}, {"K", GLFW_KEY_K}, {"L", GLFW_KEY_L},
    {"M", GLFW_KEY_M}, {"N", GLFW_KEY_N}, {"O", GLFW_KEY_O}, {"P", GLFW_KEY_P},
    {"Q", GLFW_KEY_Q}, {"R", GLFW_KEY_R}, {"S", GLFW_KEY_S}, {"T", GLFW_KEY_T},
    {"U", GLFW_KEY_U}, {"V", GLFW_KEY_V}, {"W", GLFW_KEY_W}, {"X", GLFW_KEY_X},
    {"Y", GLFW_KEY_Y}, {"Z", GLFW_KEY_Z},
    {"LEFT BRACKET", GLFW_KEY_LEFT_BRACKET},
    {"BACKSLASH", GLFW_KEY_BACKSLASH},
    {"RIGHT BRACKET", GLFW_KEY_RIGHT_BRACKET},
    {"GRAVE ACCENT", GLFW_KEY_GRAVE_ACCENT},
    {"WORLD 1", GLFW_KEY_WORLD_1},
    {"WORLD 2", GLFW_KEY_WORLD_2},
    {"ESCAPE", GLFW_KEY_ESCAPE},
    {"ENTER", GLFW_KEY_ENTER},
    {"TAB", GLFW_KEY_TAB},
    {"BACKSPACE", GLFW_KEY_BACKSPACE},
    {"INSERT", GLFW_KEY_INSERT},
    {"DELETE", GLFW_KEY_DELETE},
    {"RIGHT", GLFW_KEY_RIGHT},
    {"LEFT", GLFW_KEY_LEFT},
    {"DOWN", GLFW_KEY_DOWN},
    {"UP", GLFW_KEY_UP},
    {"PAGE UP", GLFW_KEY_PAGE_UP},
    {"PAGE DOWN", GLFW_KEY_PAGE_DOWN},
    {"HOME", GLFW_KEY_HOME},
    {"END", GLFW_KEY_END},
    {"CAPS LOCK", GLFW_KEY_CAPS_LOCK},
    {"SCROLL LOCK", GLFW_KEY_SCROLL_LOCK},
    {"NUM LOCK", GLFW_KEY_NUM_LOCK},
    {"PRINT SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"PAUSE", GLFW_KEY_PAUSE},
    {"F1", GLFW_KEY_F1}, {"F2", GLFW_KEY_F2}, {"F3", GLFW_KEY_F3},
    {"F4", GLFW_KEY_F4}, {"F5", GLFW_KEY_F5}, {"F6", GLFW_KEY_F6},
    {"F7", GLFW_KEY_F7}, {"F8", GLFW_KEY_F8}, {"F9", GLFW_KEY_F9},
    {"F10", GLFW_KEY_F10}, {"F11", GLFW_KEY_F11}, {"F12", GLFW_KEY_F12},
    {"F13", GLFW_KEY_F13}, {"F14", GLFW_KEY_F14}, {"F15", GLFW_KEY_F15},
    {"F16", GLFW_KEY_F16}, {"F17", GLFW_KEY_F17}, {"F18", GLFW_KEY_F18},
    {"F19", GLFW_KEY_F19}, {"F20", GLFW_KEY_F20}, {"F21", GLFW_KEY_F21},
    {"F22", GLFW_KEY_F22}, {"F23", GLFW_KEY_F23}, {"F24", GLFW_KEY_F24},
    {"F25", GLFW_KEY_F25},
    {"KP 0", GLFW_KEY_KP_0},
    {"KP 1", GLFW_KEY_KP_1}, {"KP 2", GLFW_KEY_KP_2}, {"KP 3", GLFW_KEY_KP_3},
    {"KP 4", GLFW_KEY_KP_4}, {"KP 5", GLFW_KEY_KP_5}, {"KP 6", GLFW_KEY_KP_6},
    {"KP 7", GLFW_KEY_KP_7}, {"KP 8", GLFW_KEY_KP_8}, {"KP 9", GLFW_KEY_KP_9},
    {"KP DECIMAL", GLFW_KEY_KP_DECIMAL},
    {"KP DIVIDE", GLFW_KEY_KP_DIVIDE},
    {"KP MULTIPLY", GLFW_KEY_KP_MULTIPLY},
    {"KP SUBTRACT", GLFW_KEY_KP_SUBTRACT},
    {"KP ADD", GLFW_KEY_KP_ADD},
    {"KP ENTER", GLFW_KEY_KP_ENTER},
    {"KP EQUAL", GLFW_KEY_KP_EQUAL},
    {"LEFT SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"LEFT CONTROL", GLFW_KEY_LEFT_CONTROL},
    {"LEFT ALT", GLFW_KEY_LEFT_ALT},
    {"LEFT SUPER", GLFW_KEY_LEFT_SUPER},
    {"RIGHT SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"RIGHT CONTROL", GLFW_KEY_RIGHT_CONTROL},
    {"RIGHT ALT", GLFW_KEY_RIGHT_ALT},
    {"RIGHT SUPER", GLFW_KEY_RIGHT_SUPER},
    {"MENU", GLFW_KEY_MENU},
};

template <typename T, typename U, typename D>
const U& lookup(const T& param, T KTEntry::*m1, U KTEntry::*m2, const D& def)
{
    auto match = [&](const KTEntry& p)
    {
        return param == p.*m1;
    };

    auto iter = find_if(begin(KEY_TABLE), end(KEY_TABLE), match);

    if (iter == end(KEY_TABLE))
        return def;

    return iter->*m2;
}

int lookupName(const string& str)
{
    return lookup(str, &KTEntry::first, &KTEntry::second, GLFW_KEY_UNKNOWN);
}

const string& lookupValue(int value)
{
    return lookup(value, &KTEntry::second, &KTEntry::first, "UNKNOWN");
}

template <size_t N>
int lookupCustom(const KTEntry (&table)[N], const string& str)
{
    auto match = [&](const KTEntry& p)
    {
        return p.first == str;
    };

    auto iter = find_if(begin(table), end(table), match);

    if (iter != end(table))
        return iter->second;

    return GLFW_KEY_UNKNOWN;
}

} // globals

KeyHandle::KeyHandle(const KeyData* kd)
    : data(kd)
{}

bool KeyHandle::down() const
{
    return data->is_down;
}

bool KeyHandle::pressed() const
{
    return (data->presses > 0);
}

bool KeyHandle::released() const
{
    return (data->releases > 0);
}

KeyHandle::operator bool() const
{
    return down();
}

Key fromName(const string& str) // static
{
    Key rv;
    rv.value = lookupName(str);
    return rv;
}

const string& getName(const Key& k) // static
{
    return lookupValue(k.value);
}

Key Key::character(char c) // static
{
    Key rv;
    rv.value = c;
    return rv;
}

Key Key::numpad(int i) // static
{
    Key rv;
    rv.value = GLFW_KEY_KP_0 + i;
    return rv;
}

Key Key::numpad(string const& str) // static
{
    const KTEntry table[] = {
        {".", GLFW_KEY_KP_DECIMAL},
        {"/", GLFW_KEY_KP_DIVIDE},
        {"*", GLFW_KEY_KP_MULTIPLY},
        {"-", GLFW_KEY_KP_SUBTRACT},
        {"+", GLFW_KEY_KP_ADD},
        {"ENTER", GLFW_KEY_KP_ENTER},
        {"EQUAL", GLFW_KEY_KP_EQUAL},
    };

    Key rv;

    if (str.size() == 1)
        rv.value = GLFW_KEY_KP_0 + str[0];
    else
        rv.value = lookupCustom(table, str);

    return rv;
}

Key Key::function(int i) // static
{
    Key rv;
    rv.value = GLFW_KEY_F1 + i-1;
    return rv;
}

Key Key::mouse(int i) // static
{
    Key rv;
    rv.type = MOUSE;
    rv.value = GLFW_MOUSE_BUTTON_1 + i-1;
    return rv;
}

Key Key::mouse(string const& str) // static
{
    const KTEntry table[] = {
        {"LEFT", GLFW_MOUSE_BUTTON_LEFT},
        {"RIGHT", GLFW_MOUSE_BUTTON_RIGHT},
        {"MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE},
    };

    Key rv;
    rv.type = MOUSE;

    if (str.size() == 1)
        rv.value = GLFW_MOUSE_BUTTON_1 + str[0]-1;
    else
        rv.value = lookupCustom(table, str);

    return rv;
}

Key Key::esc() // static
{
    Key rv;
    rv.value = GLFW_KEY_ESCAPE;
    return rv;
}

} // namespace Interface_detail
} // namespace Inugami
