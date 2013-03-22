/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_PROFILER_H
#define INUGAMI_PROFILER_H

#include "utility.hpp"

#include <map>
#include <string>
#include <vector>

namespace Inugami {

class Profiler final
{
public:
    class Profile final
    {
        friend class Profiler;
        using PMap = std::map<std::string, Profile*>;
    public:
        Profile();

        double min, max;
        double average;
        double samples;

        ConstMap<PMap> getChildren() const;

    private:
        double start;
        PMap children;
    };

    using PMap = Profile::PMap;

    Profiler();
    ~Profiler();

    void start(const std::string& in);
    void stop();

    ConstMap<PMap> getAll();

private:
    PMap profiles;
    std::vector<Profile*> current;
};

class ScopedProfile final
{
public:
    ScopedProfile() = delete;
    ScopedProfile(const ScopedProfile&) = delete;
    ScopedProfile(Profiler& in, const std::string& str);
    ScopedProfile(Profiler* in, const std::string& str);
    ~ScopedProfile();

private:
    Profiler& profiler;
    std::string name;
};

} // namespace Inugami

#endif // INUGAMI_PROFILER_H
