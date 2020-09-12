#ifndef QM_QUIMULUN_H
#define QM_QUIMULUN_H
#include "static_string.h"
#include "qm_function.h"
#include "qm_distribution.h"
#include "qm_mapu.h"
#include "qm_operator.h"
#include "qm_span.h"

template <class...xs>
struct quimulun: public xs...
{
  static inline constexpr auto quimulun_name=my_static_string("\nquimulun{");
  static inline constexpr auto name=(quimulun_name+...+xs::name)+my_static_string("\n}");
  using xs::operator[]...;

  constexpr quimulun(xs&&...x):xs{x}...{}

  constexpr quimulun(){}


};
template<class ...xs,class... xs2>
 constexpr auto operator&&(quimulun<xs...>&& , quimulun<xs2...>&& )
{return quimulun<xs...,xs2...>{} ;}


template <class...xs>
quimulun(xs&&...)->quimulun<xs...>;


template <class...xs>
constexpr auto get_distributions(quimulun<xs...>)
{
    return (distributions<>{}&&...&&get_distribution(xs{}));
}

template <class...xs>
constexpr auto get_functions(quimulun<xs...>)
{
    return (functions<>{}&&...&&get_function(xs{}));
}


template<class... Ids> struct variables{

    static constexpr auto size(){return sizeof... (Ids);}

    template<class ...Id2>
    friend __host__ __device__ constexpr auto operator&&(variables,variables<Id2...>) {return variables<Ids...,Id2...>{};}


    template<class ...Id2>
    friend __host__ __device__ constexpr auto join(variables,variables<Id2...>) {return variables<Ids...,Id2...>{};}


    template<template<class...>class distributions,class Id>
    friend __host__ __device__ constexpr auto operator- (distributions<Id>,variables)
    {
         if constexpr ((std::is_same_v<Id,Ids>||...||false))
            return distributions<>{};
         else
            return distributions<Id>{};
    }

    template<template<class...>class distributions,class Id,class... Id2>
    friend __host__ __device__ constexpr auto operator- (distributions<Id,Id2...>,variables)
    {
        return ((distributions<Id>{}-variables{})&&...&&(distributions<Id2>{}-variables{}));
    }

    template<template<class...>class distributions>
    friend __host__ __device__ constexpr auto operator- (distributions<>,variables) {return distributions<>{};}

 };


constexpr auto  get_variables(Nothing) {return variables<>{};}

template<class... errors>
constexpr auto  get_variables(Error<errors...>) {return variables<>{};}



template<class Id, typename T>
constexpr auto  get_variables(Type<x_i<Id,T>>) {return variables<Id>{};}

template <class T, class unit>
constexpr auto  get_variables(Type<v<T,unit>>) {return variables<>{};}

template <class T, class unit>
constexpr auto  get_variables(Type<logv<T,unit>>) {return variables<>{};}

template<class... xs>
constexpr auto  get_variables(Type<vs<xs...>>) {return (variables<>{}&&...&&get_variables(Type<xs>{}));}

template<class Value_type,class...  Idxs>
constexpr auto  get_variables(Type<v_f<index_prod<Idxs...>,Value_type>>) {return get_variables(Type<Value_type>{});}

template<class... Indexes, class ...data>
constexpr auto get_variables(Type<mapu<index_table<Indexes...>,data...>>) {return (variables<>{}&&...&&get_variables(Type<data>{}));}

template <class Id, class T, class ...Idxs>
constexpr auto get_variables(d_i<Id,index_prod<Idxs...>,T>) {return variables<Id>{};}

template<class... ts>
constexpr auto get_variables(Domain<ts...>) {return join(variables<>{},get_variables(ts{})...);}



template<class F,class... Args>
constexpr auto collect_arguments(Op<F,Args...>){return (collect_arguments(Args{})+...);}

template<class Id>
constexpr auto collect_arguments(Id){
    return arguments<Id>{};
}







template<class... Fs>
constexpr auto collect_arguments(quimulun<Fs...>){return (collect_arguments(Fs{})+...);}

template<class Id, class G>
constexpr auto collect_arguments(F<Id,G>){return collect_arguments(G{});}

template<class Id, class Distr>
constexpr auto collect_arguments(D<Id,Distr>){return collect_arguments(Distr{});}




#endif // QM_QUIMULUN_H
