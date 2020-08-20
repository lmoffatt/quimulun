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



#endif // QM_OPERATOR_H
