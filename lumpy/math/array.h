#pragma once

#include <lumpy/core.h>
#include <lumpy/math/slice.h>

namespace lumpy
{

namespace math
{

template <class T, size_t N>
class ndarray
    : public ndslice<array_view<T>,N>
{
public:
    using base = ndslice<array_view<T>, N>;

    constexpr ndarray(const base& value, std::shared_ptr<T> sdata)
        : base(value)
        , _sdata(sdata)
    {}

    explicit ndarray(const size_t(&shape)[N])
        : base(array_view<T>(new T[sum_array(shape)], sum_array(shape)), shape)
        , _sdata(base::_data._elements)
    {}

    template<size_t ..._Ns, class = static_if<sizeof...(_Ns) == N && if_all((_Ns <= 2)...) > >
    constexpr ndarray< T, select_indexs<2, _Ns...>::size> slice(const size_t(&...sections)[_Ns]) const
    {
        return{ base::slice(sections...),  _sdata};
    }

protected:
    std::shared_ptr<T>  _sdata;
};

}
}
