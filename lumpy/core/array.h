#pragma once

#include <lumpy/core/type.h>

namespace lumpy
{

namespace core
{

template<class T>
struct T$
{
    template<class _T>
    constexpr auto operator-(const _T& value) const noexcept
    {
        return T(-1) - T(value);
    }

    constexpr operator T() const { return T(-1); }
};

constexpr auto $ = T$<size_t>{};


template<class Index>
constexpr auto shrink$(Index index, Index $) noexcept
{
    return to_signed(index) >= 0 ? index : $ + to_signed(index);
}

template<class Index>
constexpr auto length$(Index first, Index last, Index $) noexcept
{
    return shrink$(last, $) - shrink$(first, $) + 1;
}

template<class T>
struct array_iterator
{
public:
    constexpr explicit array_iterator(T* ptr, size_t size = 0)
        : _ptr(ptr + size)
    {}

    constexpr auto operator->() const noexcept { return  _ptr; }
    constexpr auto operator* () const noexcept { return *_ptr; }

    constexpr bool operator==(const array_iterator& other) const noexcept
    {
        return _ptr == other._ptr;
    }

    auto& operator++() { ++_ptr; return *this; }
    auto& operator--() { ++_ptr; return *this; }

    auto  operator++(int) { auto tmp = *this; ++*this; return tmp; }
    auto  operator--(int) { auto tmp = *this; --*this; return tmp; }

private:
    T*  _ptr;
};

template<class T>
struct array_view
{
    using type = T;
    using iterator = array_iterator<T>;
    using const_iterator = array_iterator<const T>;

    T*      _elements;
    size_t  _size;

    constexpr array_view(T* elements, size_t size) : _elements(elements), _size(size) {}

    constexpr auto&  operator[](size_t i)   const noexcept { return _elements[i]; }
    auto&            operator[](size_t i)         noexcept { return _elements[i]; }

    constexpr auto size()  const  noexcept { return _size; }
    constexpr auto begin() const  noexcept { return const_iterator(_elements); }
    constexpr auto end()   const  noexcept { return const_iterator(_elements, _size); }

    auto begin()        noexcept { return iterator(_elements); }
    auto end()          noexcept { return iterator(_elements, _size); }

    constexpr array_view slice(size_t first, size_t last) const noexcept
    {
        return{
            _elements + shrink$(first, _size),
            shrink$(last, _size) - shrink$(first, _size) + 1
        };
    }
};

template<class T>
struct array_iota
{
    using type = T;

    struct iterator
    {
        type value;

        constexpr auto operator*() const { return value; }

        auto& operator++()      noexcept { ++value; return *this; }
        auto& operator--()      noexcept { ++value; return *this; }

        auto& operator++(int)   noexcept { auto tmp = *this; ++*this; return tmp; }
        auto& operator--(int)   noexcept { auto tmp = *this; ++*this; return tmp; }
    };

    explicit constexpr  array_iota(type size) : _first(0), _last(size - 1) {}
    constexpr           array_iota(type first, type last) : _first(first), _last(last) {}

    constexpr type      operator[](size_t i)const noexcept { return _first + T(i); }
    constexpr size_t    size()              const noexcept { return size_t(_last - _first + 1); }
    constexpr iterator  begin()             const noexcept { return iterator{ _first }; }
    constexpr iterator  end()               const noexcept { return iterator{ _last }; }

    constexpr array_iota slice(size_t first, size_t last) const noexcept
    {
        return{
            _first + shrink$(type(first), type(size())),
            _first + shrink$(type(last), type(size()))
        };
    }

private:
    type    _first;
    type    _last;
};

template<class T> constexpr array_iota<T> iota(T size) { return{ 0, size - 1 }; }
template<class T> constexpr array_iota<T> iota(T first, T last) { return{ first, last }; }

template<class T, size_t N>
struct array
{
    using type = T;
    using array_t = T[N];
    using iterator = array_iterator<T>;
    using const_iterator = array_iterator<const T>;

    T   _elements[N];

    constexpr operator const array_t& ()    const   noexcept { return _elements; }

    auto& operator[](size_t index)                  noexcept { return _elements[index]; }
    auto& operator[](size_t index)          const   noexcept { return _elements[index]; }

    constexpr auto size()  const  noexcept { return N; }
    constexpr auto begin() const  noexcept { return const_iterator(_elements); }
    constexpr auto end()   const  noexcept { return const_iterator(_elements, N); }

    auto begin()    noexcept { return iterator(_elements); }
    auto end()      noexcept { return iterator(_elements, N); }

    constexpr array_view<T> slice(size_t first, size_t last) const noexcept
    {
        return array_view<T>{_elements, N}.slice(first, last);
    }

};


namespace detail
{
template <class T, size_t N, size_t... I>
constexpr array<T, N> to_array_impl(const T(&a)[N], indexs_t<I...>)
{
    return{ {a[I]...} };
}
}

template <class T, size_t N>
constexpr auto to_array(const T(&a)[N])
{
    return detail::to_array_impl(a, to_indexs<N>{});
}

#pragma endregion

}

}
