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

#ifndef INUGAMI_DETAIL_CONSTATTR_HPP
#define INUGAMI_DETAIL_CONSTATTR_HPP

namespace Inugami {

/*! @brief Constant attribute.
 *
 *  This object is designed such that the parent class can freely manipulate
 *  the contained data, but outside classes can only access the data. It is
 *  intended to be a replacement for a getter when outsiders need to access
 *  the data as if it were a public member.
 *
 *  @tparam T The type of data to hold.
 *  @tparam Owner The owner.
 */
template <typename T, class Owner>
class ConstAttr
{
    using Data = T;
    friend Owner;
public:
    /*! @brief Cast to constant T.
     */
    operator const Data& () const
    {
        return data;
    }

    /*! @brief Drill to data.
     */
    const Data& operator->()
    {
        return data;
    }

private:
    /*! @brief Template constructor.
     */
    template <typename... A>
    ConstAttr(A&&... in)
        : data(std::forward<A>(in)...)
    {}

    /*! @brief Template assignment.
     */
    template <typename A>
    ConstAttr& operator=(A&& in)
    {
        data = std::forward<A>(in);
        return *this;
    }

    Data data;
};

} // namespace Inugami

#endif // INUGAMI_DETAIL_CONSTATTR_HPP
