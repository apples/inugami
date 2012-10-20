#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vector>

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

} // namespace Inugami

#endif // UTILITY_H_INCLUDED
