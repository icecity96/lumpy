#pragma once

#include <typeinfo>
#include <utility>
#include <memory>
#include <string>

namespace lumpy
{
namespace core
{

using   byte      = signed char;
using   llong     = long long;

using   uchar     = unsigned char;
using   ubyte     = unsigned char;
using   ushort    = unsigned short;
using   uint      = unsigned;
using   ulong     = unsigned long;
using   ullong    = unsigned long long;

using   std::size_t;
using   std::string;
using   std::declval;

template<class T>
constexpr auto to_signed(const T& value)
{
    return std::make_signed_t<T>(value);
}

template<class T>
constexpr auto to_unsigned(const T& value)
{
    return std::make_unsigned_t<T>(value);
}

#pragma region static_if
template<bool value>
using static_if = std::enable_if_t<value>;

using std::true_type;
using std::false_type;
constexpr true_type     true_value = {};
constexpr false_type    false_value = {};
#pragma endregion

#pragma region indexs
template<size_t ...Is>
struct indexs_t
{
    static constexpr size_t size = sizeof...(Is);
};

namespace detail
{
template<size_t IA, size_t IB = 0, size_t ...Is>
struct _Make_Index_t
    : _Make_Index_t<IA - 1, IB + 1, Is..., IB>
{};

template<size_t IB, size_t ...Is>
struct _Make_Index_t<0, IB, Is...>
{
    using type      = indexs_t<Is...>;
};

template<size_t I0, class Index_t>
struct _Join_Indexs;

template<size_t I0, size_t ...Is>
struct _Join_Indexs<I0, indexs_t<Is...>>
{
    using type      = indexs_t<I0, Is...>;
};

template<size_t X, size_t I, size_t ...Vs>
struct _Select_Index;

template<size_t X, size_t I, size_t V0>
struct _Select_Index<X, I, V0>
{
    using type      = std::conditional_t<V0 == X, indexs_t<I>, indexs_t<>>;
};

template<size_t X, size_t I, size_t V0, size_t ...Vs>
struct _Select_Index<X, I, V0, Vs...>
{
    using next_t    = typename _Select_Index<X, I + 1, Vs...> ::type;
    using type      = std::conditional_t<V0 == X, typename _Join_Indexs<I, next_t>::type, next_t>;
};

}

template<size_t N>
using to_indexs = typename detail::_Make_Index_t<N>::type;

template<size_t X, size_t ...I>
using select_indexs = typename detail::_Select_Index<X, 0, I...>::type;

#pragma endregion

#pragma region algorithm

#pragma region sum

template<class T>
constexpr auto sum(const T& value)
{
    return value;
}

template<class T0, class T1, class ...Ts>
constexpr auto sum(const T0& a, const T1& b, const Ts& ...ts)
{
    return a + sum(b, ts...);
}

template<class T, size_t N, size_t ...Is>
constexpr auto sum_array(const T(&arr)[N], indexs_t<Is...>)
{
    return sum(arr[Is]...);
}

template<class T, size_t N>
constexpr auto sum_array(const T(&arr)[N])
{
    return sum_array(arr, to_indexs<N>{});
}

#pragma endregion


#pragma region product
template<class T>
constexpr auto product(const T& value)
{
    return value;
}

template<class T0, class T1, class ...Ts>
constexpr auto product(const T0& a, const T1& b, const Ts& ...ts)
{
    return a * product(b, ts...);
}

template<class T, size_t N, size_t ...Is>
constexpr auto product_array(const T(&arr)[N], indexs_t<Is...>)
{
    return product(arr[Is]...);
}

template<class T, size_t N>
constexpr auto product_array(const T(&arr)[N])
{
    return product_array(arr, to_indexs<N>{});
}


#pragma endregion

#pragma region select
template<class T, size_t>
struct array;

template<class T, size_t ...Is>
constexpr array<T,sizeof...(Is)> select(std::initializer_list<T> list, indexs_t<Is...>)
{
    return{ { (list.begin()[Is])... } };
}

#pragma endregion

constexpr size_t count_if(std::initializer_list<bool> values)
{
    return values.size() == 1
        ? (values.begin()[0] ? 1 : 0)
        : (values.begin()[0] ? 1 : 0) + count_if(std::initializer_list<bool>(values.begin() + 1, values.end()));
}

template<class T>
constexpr bool if_all(const T& value)
{
    return value;
}

template<class T0, class T1, class ...Ts>
constexpr bool if_all(const T0& t0, const T1& t1, const Ts&...ts)
{
    return t0 && if_all(t1, ts...);
}

#pragma endregion

#pragma region is_???
template<class TExpect, class ...Types> constexpr bool is_same      = std::conjunction<std::is_same<TExpect, Types>...>::value;
template<               class ...Types> constexpr bool is_float     = std::conjunction<std::is_floating_point<Types>...>::value;
template<               class ...Types> constexpr bool is_integral  = std::conjunction<std::is_integral<Types>...>::value;
template<               class ...Types> constexpr bool is_int       = std::conjunction<std::is_signed<Types>...>::value;
template<               class ...Types> constexpr bool is_uint      = std::conjunction<std::is_unsigned<Types>...>::value;
template<               class ...Types> constexpr bool is_ptr       = std::conjunction<std::is_pointer<Types>...>::value;
#pragma endregion

#pragma region remove/add
template<class Type>
using remove_cv = std::remove_cv_t<Type>;
#pragma endregion

}

}
