#ifndef INUGAMI_PROFILER_H
#define INUGAMI_PROFILER_H

#include "utility.h"

#include <map>
#include <string>

namespace Inugami {

class Profiler
{
public:
    class Profile
    {
        friend class Profiler;
    public:
        Profile();

        double min, max;
        double average;
        double samples;

    private:
        double start;
    };

    typedef std::map<std::string, Profile> PMap;

    Profiler();

    void start(const std::string& in);
    void stop(const std::string& in);

    const Profile& get(const std::string& in);
    ConstMap<PMap> getAll();

private:
    PMap profiles;
};

class ScopedProfile
{
public:
    ScopedProfile() = delete;
    ScopedProfile(Profiler& in, const std::string& str);
    ~ScopedProfile();

private:
    Profiler& profiler;
    std::string name;
};

} // namespace Inugami

#endif // INUGAMI_PROFILER_H
