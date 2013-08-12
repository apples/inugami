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

#ifndef INUGAMI_DETAIL_ADDONCE_HPP
#define INUGAMI_DETAIL_ADDONCE_HPP

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

namespace Inugami {

/*! @brief Adds an element to a container only once.
 *
 *  Adds the given object to the container only if it does not already exist.
 *  The return value is the location of the newly added or already existing
 *  element.
 *
 *  @param container Container.
 *  @param target Object to add to container.
 *
 *  @return Position of element.
 */
template <class C, typename T>
auto addOnce(C&& container, T&& target)
-> decltype(begin(container))
{
    auto i = std::find(begin(container), end(container), target);
    if (i != end(container)) return i;
    return container.insert(end(container), std::forward<T>(target));
}

/*! @brief Adds an element to a vector only once.
 *
 *  Adds the given object to the container only if it does not already exist.
 *  The return value is the location of the newly added or already existing
 *  element.
 *
 *  @param container Container.
 *  @param target Object to add to container.
 *
 *  @return Position of element.
 */
template <typename C, typename T
    , typename RVal = typename std::decay<C>::type::size_type
>
RVal addOnceVec(C&& container, T&& target)
{
    auto i = std::find(begin(container), end(container), target);
    if (i != end(container)) return (i - begin(container));
    container.push_back(std::forward<T>(target));
    return (container.size() - 1);
}

} // namespace Inugami

#endif // INUGAMI_DETAIL_ADDONCE_HPP
