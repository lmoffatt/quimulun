#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H

#include "qm_vector_basis.h"


template<class... xs>
struct vs :public xs...
{
   using xs::operator[]...;
  __host__ __device__ constexpr explicit vs(xs&&... x):xs{std::move(x)}...{}

  template<class otherId>
  __host__ __device__ constexpr auto operator[](otherId)const {return Nothing{};}


};


#endif // QM_VECTOR_SPACE_H
