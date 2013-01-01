/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vector>
#include <stdexcept>

namespace Inugami {

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

} // namespace Inugami

#endif // UTILITY_H_INCLUDED
