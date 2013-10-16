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

#ifndef INUGAMI_PROFILER_H
#define INUGAMI_PROFILER_H

#include "utility.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Inugami {

/*! @brief Nesting profiler.
 *
 *  This class is an automatic nesting profiler. It can be used for high-
 *  precision timing for use in tracking down bottlenecks.
 */
class Profiler
{
public:

    /*! @brief Profile data.
     */
    class Profile
    {
        friend class Profiler;
    public:

        using Ptr = std::shared_ptr<Profile>;
        using PMap = std::map<std::string, Ptr>;

        Profile();

        double min;     //!< Minimum duration.
        double max;     //!< Maximum duration.
        double average; //!< Average duration.
        double samples; //!< Number of durations recorded.

        PMap const& getChildren() const;

    private:
        double start;
        PMap children;
    };

    /*! @brief Map of names to Profile%s.
     */
    using PMap = Profile::PMap;

    /*! @brief Start the specified Profile.
     *
     *  Starts the given Profile. If another Profile is already active, the
     *  given Profile is nested inside of the active Profile.
     *
     *  @param in Name of the Profile to start.
     */
    void start(const std::string& in);

    /*! @brief Stops the active Profile.
     *
     *  Stops the currently active Profile. If the active Profile is nested,
     *  the parent Profile becomes active.
     */
    void stop();

    /*! @brief Gets the top-level profiles.
     *
     *  @return Top-level profiles.
     */
    PMap const& getAll() const;

private:
    PMap profiles;
    std::vector<Profile::Ptr> current;
};

/*! @brief Automatic profile manager.
 *
 *  When created, this object will start the Profiler::Profile that's
 *  specified. When destroyed, the Profiler::Profile will be stopped.
 */
class ScopedProfile final
{
public:
    ScopedProfile() = delete;
    ScopedProfile(const ScopedProfile&) = delete;

    /*! @brief Reference constructor.
     *
     *  Starts the named Profiler::Profile in the given Profiler.
     *
     *  @param in Profiler.
     *  @param str Name of Profiler::Profile.
     */
    ScopedProfile(Profiler& in, const std::string& str);

    /*! @brief Pointer constructor.
     *
     *  Starts the named Profiler::Profile in the given Profiler.
     *
     *  @param in Profiler.
     *  @param str Name of Profiler::Profile.
     */
    ScopedProfile(Profiler* in, const std::string& str);

    /*! @brief Destructor.
     *
     *  The managed Profiler::Profile will be stopped.
     */
    ~ScopedProfile();

private:
    Profiler& profiler;
    std::string name;
};

} // namespace Inugami

#endif // INUGAMI_PROFILER_H
