#pragma once

namespace lumpy
{
namespace math
{

template<class T, size_t>
struct ndslice;

template<class F, class ...Ts>
struct ndview;

template<class F, class A>
struct ndview<F, A>
{
    A a;

    template<class..._Is>
    constexpr auto operator()(_Is&& ...is) const
    {
        return F::run(a(is...));
    }
};

template<class F, class A, class B>
struct ndview<F, A, B>
{
    A a;
    B b;

    template<class..._Is>
    constexpr auto operator()(_Is&& ...is) const
    {
        return F::run(a(is...), b(is...));
    }
};


#pragma region expressions
template<class T, class=void>
struct _IsExpr: false_type{};

template<class T>
constexpr bool is_expr = _IsExpr<T>::value;

template<class F, class... Ts>
struct _IsExpr<ndview<F, Ts...>>: true_type{};

#pragma region operators
struct f_add { template<class A, class B> auto run(A&& a, B&&b) { return a + b; } };
struct f_sub { template<class A, class B> auto run(A&& a, B&&b) { return a - b; } };
struct f_mul { template<class A, class B> auto run(A&& a, B&&b) { return a * b; } };
struct f_div { template<class A, class B> auto run(A&& a, B&&b) { return a / b; } };
struct f_mod { template<class A, class B> auto run(A&& a, B&&b) { return a % b; } };

template<class A, class B, class=static_if<is_expr<A> && is_expr<B> > >
ndview<f_add, A, B> operator+(const A& a, const B& b)   { return{ a, b };}

template<class A, class B, class=static_if<is_expr<A> && is_expr<B> > >
ndview<f_sub, A, B> operator-(const A& a, const B& b)   { return{ a, b };}

template<class A, class B, class=static_if<is_expr<A> && is_expr<B> > >
ndview<f_mul, A, B> operator*(const A& a, const B& b)   { return{ a, b };}

template<class A, class B, class=static_if<is_expr<A> && is_expr<B> > >
ndview<f_div, A, B> operator/(const A& a, const B& b)   { return{ a, b };}

template<class A, class B, class=static_if<is_expr<A> && is_expr<B> > >
ndview<f_mod, A, B> operator%(const A& a, const B& b)   { return{ a, b };}


#pragma endregion

#pragma endregion

}

}
