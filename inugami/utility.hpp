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

#ifndef UTILITY_HPP_INCLUDED
#define UTILITY_HPP_INCLUDED

#include <iomanip>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <string>
#include <functional>
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
typename C::iterator addOnce(C &container, const T &target)
{
    typename C::iterator i;
    for (i=container.begin(); i!=container.end(); ++i)
    {
        if (*i == target) return i;
    }
    return container.insert(i, target);
}

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
template <typename T>
typename std::vector<T>::size_type addOnce(std::vector<T> &container, const T &target)
{
    typename std::vector<T>::size_type i, e=container.size();
    for (i=0; i<e; ++i)
    {
        if (container[i] == target) return i;
    }
    container.push_back(target);
    return e;
}

/*! @brief Turns an object into a string.
 *
 *  The object is turned into a string using a stream.
 *
 *  @param in Object to stringify.
 *
 *  @return String representation of object.
 */
template <typename T>
std::string stringify(const T &in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

/*! @brief Turns an arithmetic object into a hexadecimal string.
 *
 *  The object is turned into a hexadecimal string using a stream. The object
 *  must be an arithmetic type.
 *
 *  @param in Object to hexify.
 *
 *  @return Hexadecimal representation of object.
 */
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
    hexify(const T &in)
{
    std::stringstream ss;
    ss.fill('0');
    ss << "0x" << std::hex << std::setw(8) << in;
    return ss.str();
}

/*! @brief Turns an object pointer into a hexadecimal string.
 *
 *  The object pointer is turned into a hexadecimal string using a stream.
 *
 *  @param in Object pointer to hexify.
 *
 *  @return Hexadecimal representation of object pointer.
 */
template <typename T>
std::string hexify(const T *in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

/*! @brief Turns a hexadecimal string into an integer.
 *
 *  @param in String to dehexify.
 *
 *  @return Integer represented by the string.
 */
inline unsigned int dehexify(std::string in)
{
    unsigned int rval;
    std::stringstream ss(in);
    ss >> std::hex >> rval;
    return rval;
}

template <typename T1, typename T2>
bool chainComp(const T1& first, const T2& second)
{
    return (first<second);
}

/*! @brief Compares a sequence of pairs of objects.
 *
 *  Continuously compares the pairs of objects. If a the first object in the
 *  pair is less than the second object, return @c true immediately. If they
 *  are equal, drop them from the sequence and continue. If the first is
 *  greater than the second, return false.
 *
 *  @param first First object of first pair.
 *  @param second Second object of first pair.
 *  @param args Other pairs.
 *
 *  @return @c True or @c false based on a series of comparisons.
 */
template <typename T1, typename T2, typename... A>
bool chainComp(const T1& first, const T2& second, const A&... args)
{
    if (first<second) return true;
    if (first == second) return chainComp(args...);
    return false;
}

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

/*! @todo Fix and document comprehend().
 */
//Abandon all hope, ye who enter here.
template <
    typename R = std::vector<int>,
    typename L = int,
    typename V = typename R::value_type
//    typename I = std::function< void(V&)       >,
//    typename F = std::function< bool(const V&) >,
//    typename T = std::function<    V(const V&) >
    >
R comprehend(
    const L& start, const L& stop,
    std::function< void(L&)       > advance,// =   [](V& i)      {++i;},
    std::function< bool(const L&) > condition,// = [](const V&)  {return true;},
    std::function<    V(const L&) > transform// = [](const V& i){return i;}
    )
{
    //NOTE This is a workaround for a g++ bug as of g++ 4.7.1 (tdm64-1)
    if (!advance)     advance = [](L& i)      {++i;}         ;
    if (!condition) condition = [](const L&)  {return true;} ;
    //if (!transform) transform = [](const L& i){return V(i);} ;

    R rval;
    for (L i=start; i!=stop; advance(i))
    {
        if (condition(i))
        {
            rval.push_back(transform(i));
        }
    }
    return rval;
}

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

    /*! @brief Default constructor.
     */
    ConstAttr()
        : data()
    {}

    /*! @brief Copy constructor.
     */
    ConstAttr(const ConstAttr& in)
        : data(in.data)
    {}

    /*! @brief Move constructor.
     */
    ConstAttr(ConstAttr&& in)
        : data(std::move(in.data))
    {}

    /*! @brief Template constructor.
     */
    template <typename... A>
    ConstAttr(A&&... in)
        : data(in...)
    {}

    /*! @brief Copy assignment.
     */
    ConstAttr& operator=(const ConstAttr& in)
    {
        data = in.data;
        return *this;
    }

    /*! @brief Move assignment.
     */
    ConstAttr& operator=(ConstAttr&& in)
    {
        data = std::move(in.data);
        return *this;
    }

    /*! @brief Copy data assignment.
     */
    ConstAttr& operator=(const Data& in)
    {
        data = in;
        return *this;
    }

    /*! @brief Move data assignment.
     */
    ConstAttr& operator=(Data&& in)
    {
        data = std::move(in);
        return *this;
    }

    /*! @brief Template assignment.
     */
    template <typename A>
    ConstAttr& operator=(A&& in)
    {
        data = in;
        return *this;
    }

    Data data;
};

/*! @brief Determines if two objects are the same instance.
 *
 *  @param a First object.
 *  @param b Second object.
 *
 *  @return @c True if the objects are the same instance.
 */
template <typename T>
typename std::enable_if<!std::is_pointer<T>::value, bool>::type
sameInstance(const T& a, const T& b)
{
    return (&a == &b);
}

/*! @brief Determines if two objects are the same instance.
 *
 *  @param a Pointer to first object.
 *  @param b Pointer to second object.
 *
 *  @return @c True if the objects are the same instance.
 */
template <typename T>
typename std::enable_if<std::is_pointer<T>::value, bool>::type
sameInstance(T a, T b)
{
    return (a == b);
}

} // namespace Inugami

#endif // UTILITY_HPP_INCLUDED
