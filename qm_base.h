#ifndef QM_BASE_H
#define QM_BASE_H

#include <functional>


struct Nothing{
    template<class Something>
    friend decltype (auto) operator&&(Nothing, Something&& a){return std::forward<Something>(a);}
    template<class Something>
    friend decltype (auto) operator&&(Something&& a,Nothing){return std::forward<Something>(a);}

    friend Nothing operator&&(Nothing,Nothing){return Nothing{};}

    template<class Something>
    friend decltype (auto) operator+(Nothing, Something&& a){return std::forward<Something>(a);}
    template<class Something>
    friend decltype (auto) operator+(Something&& a,Nothing){return std::forward<Something>(a);}

    friend Nothing operator+(Nothing,Nothing){return Nothing{};}

    template<class Something>
    friend decltype (auto) join(Nothing, Something&& a){return std::forward<Something>(a);}
    template<class Something>
    friend decltype (auto) join(Something&& a,Nothing){return std::forward<Something>(a);}

    friend Nothing join(Nothing,Nothing){return Nothing{};}

    static constexpr bool is_empty(){return true;}

};





struct Anything
{
    template<class Id> constexpr Anything(Id){}
};


template<class... T> struct Cs{};

template<class T> struct Type{

    template<class... Args>
    auto constexpr operator()(Type<Args>...)
    {
        return Type<decltype(std::declval<T>()(std::declval<Args>()...))>{};
    }

    template<class Id>
    auto operator[](Id)
    {
        if constexpr (std::is_same_v<Nothing,decltype (std::declval<T>()[Id{}]) >)
                return Nothing{};
        else
                return Type<std::decay_t<decltype (std::declval<T>()[Id{}])>>{};
    }

    template<class Id>
    auto slice(Id)
    {
        if constexpr (std::is_same_v<Nothing,decltype (std::declval<T>().slice(Id{})) >)
                return Nothing{};
        else
                return Type<std::decay_t<decltype (std::declval<T>().slice(Id{}))>>{};
    }



    using type=T;

    template<class R>
    friend constexpr auto operator+(Type,Type<R>) { return sum_impl(Type{},Type<R>{});}

};


template<template <class...>class Ts, class>
struct is_of_this_template_class
{
    static inline constexpr bool value=false;
};

template<template <class...> class Ts, class...T>
struct is_of_this_template_class<Ts,Ts<T...>>
{
    static inline constexpr bool value=true;
};

template<template <class...>class Ts, class T>
inline constexpr bool is_of_this_template_class_v=is_of_this_template_class<Ts,T>::value;


template<template <class...> class... Ts>
struct is_of_any_of_these_template_classes
{
    template <class T>
    static inline constexpr bool value=(is_of_this_template_class_v<Ts,T>||...||false);

};
template<class... errors>
struct Error;


template<class... errors>
struct Error{




};

template<class ...errors,class... more_errors>
 Error<errors...,more_errors...> join(Error<errors...>,Error<more_errors...>){return Error<errors...,more_errors...>{};}

 template<class ...errors,class Something>
  decltype (auto) join(Error<errors...>, Something&& a){return std::forward<Something>(a);}
 template<class ...errors,class Something>
  decltype (auto) join(Something&& a,Error<errors...>){return std::forward<Something>(a);}


 constexpr auto join(){return Nothing{};}

template<class T>
constexpr decltype (auto) join(T&& x){return std::forward<T>(x);}


template<class T, class S, class U,class... Z>
constexpr decltype (auto) join(T&& x,S&& y, U&& u, Z&&...z)
{
       return join(join(std::forward<T>(x), std::forward<S>(y)),std::forward<U>(u),std::forward<Z>(z)...);
}



template<class T>
auto compose(T&& x)
{ return x;}

template<class T,class F, class... G>
auto compose(T&& x, F&& f, G&&...g)
{
  return compose(std::invoke(std::forward<F>(f), std::forward<T>(x)), std::forward<G>(g)...);
}



#endif // QM_BASE_H
