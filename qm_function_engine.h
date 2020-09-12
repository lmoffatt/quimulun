#ifndef QM_FUNCTION_ENGINE_H
#define QM_FUNCTION_ENGINE_H

#include "qm_function_standard.h"
#include "qm_base.h"
#include <cmath>

#include "qm_engine.h"
#include "qm_operator.h"

template<class...  X, class Y>
constexpr auto operator+ (Op<S,X...>,Y)
{
  return Op(S{},X{}...,Y{});
}
template<class  X, class... Y>
constexpr auto operator+ (X,Op<S,Y...>)
{
  return Op(S{},X{},Y{}...);
}

template<class...  X, class... Y>
constexpr auto operator+ (Op<S,X...>,Op<S,Y...>)
{
  return Op(S{},X{}...,Y{}...);
}


template<class > struct fn;


template<>
struct Op<Pi> {
    constexpr auto operator[] (Pi)const {return *this;}
    __host__ __device__ constexpr auto operator()()const { return PI;}
};




template <long v>
struct Op<N<v>>
{
    constexpr auto operator[] (N<v>)const {return *this;}
    __host__ __device__ constexpr auto operator()()const { return v;}
};




template<>
struct Op<Exp_10_f>{
    using F=Exp_10_f;
    constexpr auto operator[] (F)const {return *this;}
    template<class X>
   __host__ __device__ constexpr auto operator()(X x)const {using std::pow;
                                                      return pow(10.0,x);}
};


template <>
struct Op<Exp_f>{
    using F=Exp_f;
    constexpr auto operator[] (F)const {return *this;}
   template<class X>
   __host__ __device__ constexpr auto operator()(X x)const {using std::exp; return exp(x);}
};


template <>
struct Op<Cos_f>{
    constexpr auto operator[] (Cos_f)const {return *this;}
   template<class X>
   __host__ __device__ constexpr auto operator()(X x)const {using std::cos; return cos(x);}
};


template<>
struct Op<S>{
    constexpr auto operator[] (S)const {return *this;}
 template<class X, class... Y>
    __host__ __device__ constexpr auto operator()(X&& x, Y&&... y)const {return (std::forward<X>(x)+...+std::forward<Y>(y));}
};
template<>
struct Op<P>{
    constexpr auto operator[] (P)const {return *this;}
 template<class X, class... Y>
    __host__ __device__ constexpr auto operator()(X&& x, Y&&... y)const {return (std::forward<X>(x)*...*std::forward<Y>(y));}
};

template<>
struct Op<Div>{
    constexpr auto operator[] (Div)const {return *this;}
 template<class X, class Y>
   __host__ __device__  constexpr auto operator()(X&& x, Y&& y)const{return std::forward<X>(x)/std::forward<Y>(y);}
};



template<>
struct Op<Minus>{
    constexpr auto operator[] (Minus)const {return *this;}
 template<class X>
   __host__ __device__  constexpr auto operator()(X&& x)const {return -std::forward<X>(x);}
};




using  aritmetric_Ops=Ops_p<Op,Cs<S,P,Div,Minus>>;
using math_Ops=Ops_p<Op,Cs<Exp_10_f,Exp_f, Cos_f,Pi,N<2>>>;


struct Op_engine: public aritmetric_Ops, public math_Ops
{
    using aritmetric_Ops::operator[];
    using math_Ops::operator[];
};








#endif // QM_FUNCTION_ENGINE_H
