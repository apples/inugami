/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
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

#ifndef INUGAMI_LOGGER_H
#define INUGAMI_LOGGER_H

#include "utility.hpp"

#include <ostream>
#include <fstream>
#include <string>

namespace Inugami {

/*! @brief An automatic prefix manager and proxy for a Logger.
 *
 *  For the lifetime of this object, the referenced Logger will have a prefix
 *  applied to any lines that are sent from it.
 *
 *  @tparam Type of Logger.
 */
template <typename Log>
class StackLog
{
public:

    /*! @brief Reference constructor.
     *
     *  Constructs the StackLog using the given Logger and applies the given
     *  prefix to the logger.
     *
     *  @param logIn The Logger to manage.
     *  @param str The prefix to apply.
     */
    StackLog(Log& logIn, std::string str)
        : logger(logIn)
        , before(logIn.prefix)
    {
        logger.prefix = stringify(str, ": ");
    }

    /*! @brief Pointer constructor.
     *
     *  Constructs the StackLog using the given Logger and applies the given
     *  prefix to the logger.
     *
     *  @param logIn The Logger to manage.
     *  @param str The prefix to apply.
     */
    StackLog(Log *const logIn, const std::string& str)
        : logger(*logIn)
        , before(logger.prefix)
    {
        logger.prefix = stringify(str, ": ");
    }

    /*! @brief Destructor.
     */
    ~StackLog()
    {
        logger.prefix = before;
    }

    /*! @brief Proxy to Logger::log().
     */
    template <unsigned int PRIORITY, typename... T>
    Log& log(const T&... args)
    {
        return logger.log<PRIORITY>(args...);
    }

private:
    Log& logger;
    std::string before;
};

/*! @brief Template-optimized logger.
 *
 *  This logger can be used for priority-based logging. This works under the
 *  assumption that the compiler will fully optimize away calls to
 *  Logger::log() that will not be sent to the output.
 *
 *  @tparam MAXPRIORITY The maximum priority allowed for logging.
 *  @tparam SECONDARY The minimum priority required for secondary logging.
 */
template <unsigned int MAXPRIORITY, unsigned int SECONDARY = MAXPRIORITY+1>
class Logger
{
    friend class StackLog<Logger>;
public:
    Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    /*! @brief Primary constructor.
     *
     *  Constructs the Logger to have a primary output and no secondary output.
     *
     *  @param streamIn The primary output stream.
     */
    Logger(std::ostream& streamIn)
        : stream1(&streamIn)
        , stream2(nullptr)
        , delStreams(false)
        , prefix("")
    {}

    /*! @brief Secondary constructor.
     *
     *  Constructs the Logger to have a primary and secondary output.
     *
     *  @param stream1In The primary output stream.
     *  @param stream2In The secondary output stream.
     */
    Logger(std::ostream& stream1In, std::ostream& stream2In)
        : stream1(&stream1In)
        , stream2(&stream2In)
        , delStreams(false)
        , prefix("")
    {}

    /*! @brief File constructor.
     *
     *  Constructs the Logger to have a primary output to the given file and no
     *  secondary output.
     *
     *  @param fileName Name of file to use as primary output.
     */
    Logger(const std::string& fileName)
        : stream1(new std::ofstream(fileName))
        , stream2(nullptr)
        , delStreams(true)
        , prefix("")
    {}

    /*! @brief Destructor.
     */
    ~Logger()
    {
        if (delStreams)
        {
            delete stream1;
            delete stream2;
        }
    }

    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    /*! @brief Sends a line of text to the outputs.
     *
     *  If the given priority is less than the max priority, the text will be
     *  written to the primary output. If the given priority is greater than
     *  the minimum secondary priority and less than the max priority, the text
     *  will also be written to the secondary output.
     *
     *  @tparam PRIORITY Priority of this line of text.
     *  @param args Variadic list of items that can be inserted into a stream.
     *
     *  @return *this
     */
    template <unsigned int PRIORITY, typename... T>
    Logger& log(const T&... args)
    {
        if (PRIORITY <= MAXPRIORITY)
        {
            if (PRIORITY >= SECONDARY)
            {
                if (stream2) print2("[", PRIORITY, "] ", prefix, args...);
            }
            if (stream1) print1("[", PRIORITY, "] ", prefix, args...);
        }
        return *this;
    }

private:
    std::ostream *stream1;
    std::ostream *stream2;

    bool delStreams;

    std::string prefix;

    template <typename T>
    void print1(const T& a)
    {
        *stream1 << a;
        std::endl(*stream1);
    }

    template <typename T, typename... VT>
    void print1(const T& a, const VT&... args)
    {
        *stream1 << a;
        print1(args...);
    }

    template <typename T>
    void print2(const T& a)
    {
        *stream2 << a;
        std::endl(*stream2);
    }

    template <typename T, typename... VT>
    void print2(const T& a, const VT&... args)
    {
        *stream2 << a;
        print2(args...);
    }
};

} // namespace Inugami

#endif // INUGAMI_LOGGER_H
