#include "windowsystem.hpp"

#include "tracker.hpp"

#include "../opengl.hpp"

#include <algorithm>
#include <memory>
#include <vector>

namespace Inugami {
namespace Interface_detail {

namespace { // globals

std::vector<std::unique_ptr<Tracker>> windows;

} // globals

Window makeWindow(WindowParams wp)
{
    auto tp = std::make_unique<Tracker>();
    Window rv (wp, tp.get());
    windows.emplace_back(std::move(tp));
    return rv;
}

void pollEvents()
{
    auto is_dead = [](const auto& tp)
    {
        return !tp->alive;
    };

    {
        auto b = begin(windows);
        auto e = end(windows);
        windows.erase(std::remove_if(b, e, is_dead), e);
    }

    for (auto& tp : windows)
    {
        Window& window = *tp->window;

        window.characterBuffer.clear();
        for (auto& kd : window.keys)
        {
            kd.presses = 0;
            kd.releases = 0;
        }
        for (auto& kd : window.mouseButtons)
        {
            kd.presses = 0;
            kd.releases = 0;
        }
    }

    glfwPollEvents();
}

} // namespace Interface_detail
} // namespace Inugami
