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

#ifndef INUGAMI_DETAIL_RANGE_HPP
#define INUGAMI_DETAIL_RANGE_HPP

#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Inugami {

/*----------------------------------------------------------------------------*/
/* rangeDetail --                      --                      -- rangeDetail */
/*----------------------------------------------------------------------------*/

namespace rangeDetail {

/* HELPERS  --                         --                          -- HELPERS */

/** @todo Move these helpers to their own module?
 */

template <typename T>
using Decay = typename std::decay<T>::type;

template <unsigned N, typename T>
using TupleElement = typename std::tuple_element<N, T>::type;

template <typename T>
using ResultOf = typename std::result_of<T>::type;

template <bool B>
using Require = typename std::enable_if<B, void>::type;

template <typename A>
static constexpr bool IsBool()
{
    return std::is_same<A, bool>::value;
}

// Stolen from Johannes Schaub <http://stackoverflow.com/a/7858971/640397>

template <int...>
struct Sequence
{};

template<int N, int... P>
struct GenSequence
    : GenSequence<N-1, N-1, P...>
{};

template<int... P>
struct GenSequence<0, P...>
{
    using type = Sequence<P...>;
};

/* Funcs --                            --                            -- Funcs */

template <typename P>
struct Funcs
{
    using PipeType = Decay<P>;
    static constexpr unsigned TupleMax = std::tuple_size<PipeType>::value - 1;

    template <unsigned A>
    using FuncType = TupleElement<A, PipeType>;

    template <unsigned A, typename B>
    using FuncResult = ResultOf<FuncType<A>(B)>;

    template <unsigned A, typename B>
    using    IsFilter = Require< IsBool<FuncResult<A,B>>()>;
    template <unsigned A, typename B>
    using IsNotFilter = Require<!IsBool<FuncResult<A,B>>()>;

    template <unsigned A>
    using    IsLast = Require<A==TupleMax>;
    template <unsigned A>
    using IsNotLast = Require<A!=TupleMax>;

    template<int N = 0, typename T
        , typename FinalType  = FuncResult<N, T>
        , typename PtrType    = std::shared_ptr<FinalType>
        , typename = IsNotFilter<N, T>
        , typename = IsLast<N>
    >
    static PtrType pump(T&& val, const P& pipe)
    {
        auto&& transform = std::get<N>(pipe);
        return PtrType(new FinalType(transform(std::forward<T>(val))));
    }

    template<int N = 0, typename T
        , typename FinalType  = Decay<T>
        , typename PtrType    = std::shared_ptr<FinalType>
        , typename = IsFilter<N, T>
        , typename = IsLast<N>
        , typename = void
    >
    static PtrType pump(T&& val, const P& pipe)
    {
        auto&& filter = std::get<N>(pipe);

        if (filter(std::forward<T>(val)))
        {
            return PtrType{new FinalType(std::forward<T>(val))};
        }
        else
        {
            return PtrType{};
        }
    }

    template<int N = 0, typename T
        , typename = IsNotFilter<N, T>
        , typename = IsNotLast<N>
    >
    static auto pump(T&& val, const P& pipe)
    -> decltype(pump<N+1>(std::get<N>(pipe)(std::forward<T>(val)), pipe))
    {
        auto&& transform = std::get<N>(pipe);

        return pump<N+1>(transform(std::forward<T>(val)), pipe);
    }

    template<int N = 0, typename T
        , typename = IsFilter<N, T>
        , typename = IsNotLast<N>
    >
    static auto pump(T&& val, const P& pipe)
    -> decltype(pump<N+1>(std::forward<T>(val), pipe))
    {
        auto&& filter = std::get<N>(pipe);
        auto&&   next = [&]{return pump<N+1>(std::forward<T>(val), pipe);};

        if (filter(std::forward<T>(val)))
        {
            return next();
        }
        else
        {
            return decltype(next()){};
        }
    }
};

/* Range (pipelined)  --              --                 -- Range (pipelined) */

template <typename T, typename... P>
class Range
{
public:
    using Pipeline = std::tuple<P...>;
    using FuncStruct = Funcs<Pipeline>;
    using ApplyFunc = decltype((FuncStruct::template pump<0,T>));
    using ValPtr = ResultOf<ApplyFunc&(T, Pipeline)>;
    using Value = Decay<decltype(*(ValPtr{}))>;

    class const_iterator
    {
    public:
        using value_type = Value;
        using pointer = Value*;
        using reference = Value&;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using const_reference = const Value&;
        using const_pointer = const Value*;

        const_iterator(const T& v, const Range& c)
            : pos(v)
            , container(c)
            , val(FuncStruct::pump(T(pos), container.pipeline))
        {
            advance();
        }

        const_iterator& operator++()
        {
            val.reset();
            advance();
            return *this;
        }

        const_iterator operator++(int)
        {
            auto tmp = *this;
            val.reset();
            advance();
            return tmp;
        }

        const_reference operator*() const
        {
            return *val;
        }

        const_pointer operator->() const
        {
            return val;
        }

        bool operator==(const const_iterator& other) const
        {
            return (pos == other.pos);
        }

        bool operator!=(const const_iterator& other) const
        {
            return (pos != other.pos);
        }

        difference_type operator-(const_iterator other) const
        {
            difference_type rval = 0;
            while (other != *this) ++other, ++rval;
            return rval;
        }

    private:
        void advance()
        {
            while (!val && pos!=container.endVal)
            {
                val = FuncStruct::pump(T(++pos), container.pipeline);
            }
        }

        T pos;
        const Range& container;
        ValPtr val;
    };

    Range(const T& s, const T& e, const P&... p)
        : beginVal(s)
        , endVal(e)
        , pipeline(p...)
    {}

    const_iterator begin() const
    {
        return const_iterator(beginVal, *this);
    }

    const_iterator end() const
    {
        return const_iterator(endVal, *this);
    }

    template <typename A
        , typename RVal = Range<T, P..., Decay<A>>
    >
    RVal operator|(A&& a) const
    {
        using Sequence = typename GenSequence<sizeof...(P)>::type;
        return unroll(std::forward<A>(a), Sequence{});
    }

private:
    T beginVal;
    T endVal;
    Pipeline pipeline;

    template <typename A, int... B
        , typename RVal = Range<T, P..., Decay<A>>
    >
    RVal unroll(A&& last, Sequence<B...>) const
    {
        return RVal(beginVal, endVal, std::get<B>(pipeline)..., std::forward<A>(last));
    }
};

/* Range --                            --                            -- Range */

template <typename T>
class Range<T>
{
public:
    class const_iterator
    {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using const_reference = const T&;
        using const_pointer = const T*;

        const_iterator(const T& v)
            : pos(v)
        {}

        const_iterator& operator++()
        {
            ++pos;
            return *this;
        }

        const_iterator operator++(int)
        {
            auto tmp = *this;
            ++pos;
            return tmp;
        }

        const_reference operator*() const
        {
            return pos;
        }

        const_pointer operator->() const
        {
            return &pos;
        }

        bool operator==(const const_iterator& other) const
        {
            return (pos == other.pos);
        }

        bool operator!=(const const_iterator& other) const
        {
            return (pos != other.pos);
        }

        difference_type operator-(const_iterator other) const
        {
            difference_type rval = 0;
            while (other != *this) ++other, ++rval;
            return rval;
        }

    private:
        T pos;
    };

    Range(const T& s, const T& e)
        : beginVal(s)
        , endVal(e)
    {}

    const_iterator begin() const
    {
        return const_iterator(beginVal);
    }

    const_iterator end() const
    {
        return const_iterator(endVal);
    }

    template <typename A
        , typename RVal = Range<T, Decay<A>>
    >
    RVal operator|(A&& a) const
    {
        return RVal(beginVal, endVal, std::forward<A>(a));
    }

private:
    T beginVal;
    T endVal;
};

/* begin() and end() --                --                -- begin() and end() */

template <typename T, typename... P>
typename Range<T, P...>::const_iterator begin(const Range<T, P...>& val)
{
    return val.begin();
}

template <typename T, typename... P>
typename Range<T, P...>::const_iterator end(const Range<T, P...>& val)
{
    return val.end();
}

} // namespace rangeDetail

/** @todo Document range().
 */

template <typename T, typename... P
    , typename RVal = rangeDetail::Range<T, rangeDetail::Decay<P>...>
>
RVal range(T&& begin, T&& end, P&&... pipeline)
{
    return RVal(std::forward<T>(begin), std::forward<T>(end), std::forward<P>(pipeline)...);
}

} // namespac Inugami

#endif // INUGAMI_DETAIL_RANGE_HPP
