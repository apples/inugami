/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef UTILITY_HPP_INCLUDED
#define UTILITY_HPP_INCLUDED

#include <iomanip>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <string>
#include <functional>
#include <type_traits>

namespace Inugami {

template <typename T>
class TNode final
{
public:
    typedef T ValueType;

    template <typename... A>
    TNode(A... args) : data(new T(args...)), prev(this), next(this) {}

    TNode(T *in) : data(in), prev(this), next(this) {}
    TNode(decltype(nullptr)) : data(nullptr), prev(this), next(this) {}

    ~TNode() {delete data;}

    void detach()
    {
        if (prev) prev->next = next;
        if (next) next->prev = prev;
        prev = nullptr;
        next = nullptr;
    }

    void attach(TNode* const in)
    {
        if (in == this) return;
        detach();
        prev = in->prev;
        next = in;
        prev->next = this;
        next->prev = this;
    }

    TNode *prev, *next;
    T *data;
};

template <typename N>
class TIterator final
{
    typedef N Node;
public:
    TIterator()                     : target(nullptr)   {}
    TIterator(const TIterator &in)  : target(in.target) {}
    TIterator(Node *in)             : target(in)        {}

    bool operator!=(const TIterator &in) const
    {
        return (target != in.target);
    }

    TIterator &operator++()
    {
        target = target->next;
        return *this;
    }

    typename Node::ValueType &operator*()
    {
        return *target->data;
    }

    typename Node::ValueType *operator->()
    {
        return target->data;
    }

private:
    Node *target;
};

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

template <typename T>
std::string stringify(const T &in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
    hexify(const T &in)
{
    std::stringstream ss;
    ss.fill('0');
    ss << "0x" << std::hex << std::setw(8) << in;
    return ss.str();
}

template <typename T>
std::string hexify(const T *in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

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

template <typename T1, typename T2, typename... A>
bool chainComp(const T1& first, const T2& second, const A&... args)
{
    if (first<second) return true;
    if (first == second) return chainComp(args...);
    return false;
}

template <typename T>
class ConstMap
{
public:
    using Iterator = typename T::const_iterator;

    ConstMap(const T& in) : data(in) {}
    ConstMap(const ConstMap& in) : data(in.data) {}

    operator const T& () { return data; }

    Iterator begin() { return data.begin(); }
    Iterator end() { return data.end(); }

    const typename T::mapped_type& operator[](const typename T::key_type& in) const
    {
        auto i = data.find(in);
        if (i == data.end()) throw std::logic_error("Key not found!");
        return i->second;
    }

private:
    const T& data;
};

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

//ConstAttr allows Owner to set it, but outsiders can only cast.
template <typename T, class Owner>
class ConstAttr
{
    friend Owner;
public:
    operator const T& () const {return data;}
    T* operator->() {return &data;}
private:
    ConstAttr() : data() {}
    ConstAttr(const T& in) : data(in) {}
    T data;
};

template <typename T, class Owner>
class ConstAttr <T*, Owner>
{
    friend Owner;
public:
    operator T* () const {return data;}
    T* operator->() {return data;}
private:
    ConstAttr() : data() {}
    ConstAttr(T* in) : data(in) {}
    T* data;
};

template <typename T>
typename std::enable_if<!std::is_pointer<T>::value, bool>::type
equalInstance(const T& a, const T& b)
{
    return (&a == &b);
}

template <typename T>
typename std::enable_if<std::is_pointer<T>::value, bool>::type
equalInstance(T a, T b)
{
    return (a == b);
}

} // namespace Inugami

#endif // UTILITY_HPP_INCLUDED
