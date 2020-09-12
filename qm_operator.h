#ifndef QM_OPERATOR_H
#define QM_OPERATOR_H

#include "static_string.h"

template<class F,class... Args>
struct Op
{
  static constexpr auto name=F::template get_name<Args...>();
  static constexpr auto get_name(){return name;}

  __host__ __device__ Op(F,Args...){}
  __host__ __device__ Op(){}
};

template<class F>
struct Op<F>
{
  inline static constexpr auto is_identifier=true;
  inline static constexpr auto is_operator=true;
  static constexpr auto name=F::name;
  static constexpr auto get_name(){return name;}

  __host__ __device__ Op(F){}
  __host__ __device__ Op(){}
};


template<class... Ids> struct arguments{
    template<class ...Id2>
    friend __host__ __device__ auto operator&&(arguments,arguments<Id2...>) {return arguments<Ids...,Id2...>{};}

    friend __host__ __device__ auto operator+(arguments,arguments<>)
    {
        return arguments{};
    }
    template<class Id2>
    friend __host__ __device__ auto operator+(arguments,arguments<Id2>)
    {
        if constexpr ((true&&...&&(!std::is_same_v<Ids,Id2 >)))
                return arguments<Ids...,Id2>{};
        else
                return arguments{};
    }

    template<class Id2, class Id3, class... Id4>
    friend __host__ __device__ auto operator+(arguments,arguments<Id2,Id3,Id4...>)
    {
        return ((arguments{}+arguments<Id2>{}+arguments<Id3>{})+...+arguments<Id4>{});
    }



};

template <class Id, class Arg> struct missing{};





#endif // QM_OPERATOR_H
