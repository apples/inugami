/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef INUGAMI_LOGGER_H
#define INUGAMI_LOGGER_H

#include <ostream>
#include <string>

namespace Inugami {

template <typename Log>
class StackLog
{
public:
    StackLog(Log& logIn, const std::string& str) :
        logger(logIn),
        before(logIn.prefix)
    {
        logger.prefix += str;
        logger.prefix += ": ";
    }

    ~StackLog()
    {
        logger.prefix = before;
    }

    template <unsigned int PRIORITY, typename... T>
    Log& log(const T&... args)
    {
        return logger.log<PRIORITY>(args...);
    }

private:
    Log& logger;
    std::string before;
};

template <unsigned int MAXPRIORITY, unsigned int SECONDARY = MAXPRIORITY+1>
class Logger
{
    friend class StackLog<Logger>;
public:
    Logger() = delete;

    Logger(std::ostream& streamIn) :
        stream1(&streamIn), stream2(nullptr),
        prefix("")
    {}

    Logger(std::ostream& stream1In, std::ostream& stream2In) :
        stream1(&stream1In), stream2(&stream2In),
        prefix("")
    {}

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

    std::string prefix;

    template <typename T>
    void print1(const T& a)
    {
        *stream1 << a;
        stream1->flush();
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
        stream2->flush();
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
