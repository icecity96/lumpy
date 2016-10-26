#pragma once

#include <lumpy/core/type.h>
#include <lumpy/core/array.h>

namespace lumpy
{

namespace math
{

namespace detail
{
template<class T, size_t N, size_t ...Is>
constexpr array<T, N> to_stride_impl(const T(&shape)[N], indexs_t<Is...>)
{
    return{ { T(1), product_array(shape, to_indexs<1 + Is>{})... } };

}
}

template<class T, size_t N>
constexpr auto to_stride(const T(&shape)[N])
{
    return detail::to_stride_impl(shape, to_indexs<N-1>{});
}

template<class T, size_t N>
struct ndslice
{
public:
    constexpr ndslice(T data, const size_t(&shape)[N], const size_t(&stride)[N])
        : _data(data)
        , _shape(to_array(shape))
        , _stride(to_array(stride))
    {}

    constexpr ndslice(T data, const size_t(&shape)[N])
        : _data(data)
        , _shape(to_array(shape))
        , _stride(to_stride(shape))
    {}

public:
    constexpr auto& shape()               const { return _shape; }
    constexpr auto& stride()              const { return _stride; }

    template<size_t ..._Ns, class = static_if<sizeof...(_Ns) == N && if_all((_Ns <= 2)...) > >
    constexpr auto slice(const size_t(&...sections)[_Ns]) const
    {
        return slice(to_indexs<N>{}, sections...);
    }

    template<class..._Is, class = static_if<sizeof...(_Is) == N> >
    constexpr auto operator()(_Is ...indexs) const
    {
        return _data[indexOf({ indexs... }, to_indexs<N>{})];
    }

protected:
    T                   _data;
    array<size_t, N>    _shape;
    array<size_t, N>    _stride;

private:
    template<size_t ..._Is>
    constexpr auto indexOf(std::initializer_list<size_t> indexs, indexs_t<_Is...>) const
    {
        return sum((indexs.begin()[_Is] * _stride[_Is])...);
    }

    template<size_t _I, size_t _N>
    constexpr array<size_t, 2> make_section(const size_t(&section)[_N]) const
    {
        return{ { shrink$(section[0], _shape[_I]), shrink$(section[_N - 1], _shape[_I]) } };

    }

    template<size_t ..._Is, size_t ..._Ns>
    constexpr ndslice<T,select_indexs<2, _Ns...>::size> slice_impl(std::initializer_list<array<size_t, 2>> sections, indexs_t<_Ns...>, indexs_t<_Is...>) const
    {
        return{
            _data.slice(indexOf({ (sections.begin()[_Is][0])... }, indexs_t<_Is...>{}),     // first
                        indexOf({ (sections.begin()[_Is][1])... }, indexs_t<_Is...>{})),    // last
            select<size_t>({ (sections.begin()[_Is][1] - sections.begin()[_Is][0] + 1)... },    select_indexs<2, _Ns...>{}),
            select<size_t>({ (_stride[_Is])... },                                               select_indexs<2, _Ns...>{})
        };
    }

    template<size_t ..._Is, size_t ..._Ns>
    constexpr auto slice(indexs_t<_Is...>, const size_t(&...sections)[_Ns]) const
    {
        return slice_impl({ make_section<_Is>(sections)... }, indexs_t<_Ns...>{}, to_indexs<N>{});
    }
};

template<class T, class=void>
struct _IsExpr;

template<class T, size_t N>
struct _IsExpr<ndslice<T, N>> : true_type{};


template<class _T, size_t... _Ns>
auto slice(_T& value, const size_t(&...sections)[_Ns])
{
    return value.slice(sections...);
}

#pragma region reshape

template<class T, size_t S>
auto reshape(const array_view<T>& arr, const size_t(&shape)[S])
{
    return ndslice<array_view<T>, S>(arr, shape);
}

template<class T, size_t S>
auto reshape(const array_iota<T>& arr, const size_t(&shape)[S])
{
    return ndslice<array_iota<T>, S>(arr, shape);
}

template<class T, size_t N, size_t S>
auto reshape(T(&arr)[N], const size_t(&shape)[S])
{
    return reshape(array_view<T>(arr, N), shape);
}

template<class T, size_t N, size_t S>
auto reshape(const array<T, N>& arr, const size_t(&shape)[S])
{
    return reshape(arr._elements, shape);
}


#pragma endregion

}

}

