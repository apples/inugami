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

#ifndef INUGAMI_DETAIL_STREAMUTILS_HPP
#define INUGAMI_DETAIL_STREAMUTILS_HPP

#include <sstream>
#include <string>

namespace Inugami {

namespace stringifyDetail {

template <typename S, typename T>
void append(S&& ss, T&& first)
{
    ss << first;
}

template <typename S, typename T, typename... P>
void append(S&& ss, T&& first, P&&... vals)
{
    ss << first << ' ';
    return append(ss, vals...);
}

} // namespace stringifyDetail

template <typename T, typename... P>
std::string stringify(T&& first, P&&... vals)
{
    std::stringstream ss;
    stringifyDetail::append(ss, std::forward<T>(first), std::forward<P>(vals)...);
    return ss.str();
}

} // namespace Inugami

#endif // INUGAMI_DETAIL_STREAMUTILS_HPP
