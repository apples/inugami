#include "profiler.h"

#include "opengl.h"

#include <limits>

namespace Inugami {

Profiler::Profile::Profile() :
    min(std::numeric_limits<double>::max()), max(-1.0),
    average(0.0),
    samples(0.0),
    start(0.0)
{}

Profiler::Profiler() :
    profiles()
{}

void Profiler::start(const std::string& in)
{
    profiles[in].start = glfwGetTime();
}

void Profiler::stop(const std::string& in)
{
    Profile& p = profiles[in];
    double dur = glfwGetTime() - p.start;
    if (dur < p.min) p.min = dur;
    if (dur > p.max) p.max = dur;
    p.average = (p.average * p.samples + dur) / (p.samples + 1.0);
    p.samples += 1.0;
}

auto Profiler::get(const std::string& in) -> const Profile&
{
    return profiles[in];
}

auto Profiler::getAll() -> ConstMap<PMap>
{
    return ConstMap<PMap>(profiles);
}

ScopedProfile::ScopedProfile(Profiler& in, const std::string& str) :
    profiler(in), name(str)
{
    profiler.start(name);
}

ScopedProfile::~ScopedProfile()
{
    profiler.stop(name);
}

} // namespace Inugami
