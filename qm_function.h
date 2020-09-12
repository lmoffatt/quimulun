#ifndef QM_FUNCTION_H
#define QM_FUNCTION_H

#include "static_string.h"
#include "qm_base.h"


struct Calculate{};


template<class Id,class G>
class  F
{
public:
  typedef   Id myId;
  inline static constexpr auto name_impl=my_static_string("\n");
  inline static constexpr auto name=name_impl+Id::name+my_static_string("->")+G::name;

  constexpr auto operator[](Id)const {return *this;}

  constexpr F(Id ,G&& ){}
  constexpr F(){}
};


template<class... Ids> struct functions{
    template<class ...Id2>
    friend __host__ __device__ auto operator&&(functions,functions<Id2...>) {return functions<Ids...,Id2...>{};}

    template<template<class...>class arguments,class Id>
    friend __host__ __device__ constexpr auto operator- (arguments<Id>,functions)
    {
         if constexpr ((std::is_same_v<Id,Ids>||...||false))
            return arguments<>{};
         else
            return arguments<Id>{};
    }

    template<template<class...>class arguments,class Id,class... Id2>
    friend __host__ __device__ constexpr auto operator- (arguments<Id,Id2...>,functions)
    {
        return ((arguments<Id>{}-functions{})&&...&&(arguments<Id2>{}-functions{}));
    }

    template<template<class...>class arguments>
    friend __host__ __device__ constexpr auto operator- (arguments<>,functions) {return arguments<>{};}


};

template<class Id,class G>
auto get_function(F<Id,G>){ return functions<Id>{};}

template <class D>
auto get_function(D){return functions<>{};}


#endif // QM_FUNCTION_H
