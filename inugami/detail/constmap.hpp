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

#ifndef INUGAMI_DETAIL_CONSTMAP_HPP
#define INUGAMI_DETAIL_CONSTMAP_HPP

namespace Inugami {

/*! @brief Constant proxy to a map.
 *
 *  @tparam T Map type to represent.
 */
template <typename T>
class ConstMap
{
public:
    using Iterator = typename T::const_iterator;

    /*! @brief Primary constructor.
     *
     *  Construct a constant proxy to the given map.
     *
     *  @param in Map to represent.
     */
    ConstMap(const T& in)
        : data(in)
    {}

    /*! @brief Copy constructor.
     */
    ConstMap(const ConstMap& in)
        : data(in.data)
    {}

    /*! @brief Cast to represented type.
     */
    operator const T&()
    {
        return data;
    }

    /*! @brief Get beginning of map.
     *
     *  @return Constant iterator to map's beginning.
     */
    Iterator begin()
    {
        return data.begin();
    }

    /*! @brief Get end of map.
     *
     *  @return Constant iterator to map's end.
     */
    Iterator end()
    {
        return data.end();
    }

    /*! @brief Subscript operator.
     *
     *  @param in Key to retreive value for.
     *
     *  @return Value corresponding to the given key.
     */
    const typename T::mapped_type& operator[](const typename T::key_type& in) const
    {
        auto i = data.find(in);
        if (i == data.end()) throw std::logic_error("Key not found!");
        return i->second;
    }

private:
    const T& data;
};

} // namespace Inugami

#endif // INUGAMI_DETAIL_CONSTMAP_HPP
