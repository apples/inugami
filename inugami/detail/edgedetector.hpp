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

#ifndef INUGAMI_DETAIL_EDGEDETECTOR_HPP
#define INUGAMI_DETAIL_EDGEDETECTOR_HPP

#include <functional>

class EdgeDetector
{
public:
    EdgeDetector() = default;
    EdgeDetector(const EdgeDetector&) = default;
    EdgeDetector(EdgeDetector&&) = default;

    template <typename F>
    EdgeDetector(F&& f)
        : func(std::forward<F>(f))
        , prev(false)
    {}

    explicit operator bool() const
    {
        return func();
    }

    int edge()
    {
        int rval;
        bool next = func();
        rval = int(next)-int(prev);
        prev = next;
        return rval;
    }

private:
    std::function<bool()> func;
    bool prev;
};

#endif // INUGAMI_DETAIL_EDGEDETECTOR_HPP
