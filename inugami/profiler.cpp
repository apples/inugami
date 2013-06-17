/*******************************************************************************
 * Inugami - An OpenGL framwork designed for rapid game development
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

#include "profiler.hpp"

#include "opengl.hpp"

#include <limits>
#include <functional>

namespace Inugami {

Profiler::Profile::Profile()
    : min(std::numeric_limits<double>::max())
    , max(std::numeric_limits<double>::min())
    , average(0.0)
    , samples(0.0)
    , start(0.0)
    , children()
{}

auto Profiler::Profile::getChildren() const -> ConstMap<PMap>
{
    return children;
}

Profiler::Profiler()
    : profiles()
    , current()
{}

Profiler::~Profiler()
{
    std::function<void(Profile*&)> delChildren;
    delChildren = [&delChildren](Profile*& prof){
        for (auto& p : prof->children)
        {
            delChildren(p.second);
            delete p.second;
        }
    };

    for (auto& p : profiles)
    {
        delChildren(p.second);
        delete p.second;
    }
}

void Profiler::start(const std::string& in)
{
    if (current.begin() != current.end())
    {
        auto p = current.back();
        auto& c = p->children[in];
        if (!c) c = new Profile;
        current.push_back(c);
        c->start = glfwGetTime();
    }
    else
    {
        auto& p = profiles[in];
        if (!p) p = new Profile;
        current.push_back(p);
        p->start = glfwGetTime();
    }
}

void Profiler::stop()
{
    if (current.begin() != current.end())
    {
        Profile& p = *current.back();
        double dur = glfwGetTime() - p.start;
        if (dur < p.min) p.min = dur;
        if (dur > p.max) p.max = dur;
        p.average = (p.average * p.samples + dur) / (p.samples + 1.0);
        p.samples += 1.0;
        current.pop_back();
    }
    else
    {
        throw std::logic_error("No active profile!");
    }
}

auto Profiler::getAll() -> ConstMap<PMap>
{
    return ConstMap<PMap>(profiles);
}

ScopedProfile::ScopedProfile(Profiler& in, const std::string& str)
    : profiler(in)
    , name(str)
{
    profiler.start(name);
}

ScopedProfile::ScopedProfile(Profiler* in, const std::string& str)
    : profiler(*in)
    , name(str)
{
    profiler.start(name);
}

ScopedProfile::~ScopedProfile()
{
    profiler.stop();
}

} // namespace Inugami
