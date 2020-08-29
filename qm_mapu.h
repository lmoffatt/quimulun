#ifndef QM_MAPU_H
#define QM_MAPU_H

#include "qm_vector_index.h"

template<class...> class mapu;


template<class... Indexes, class ...data>
class mapu<index_table<Indexes...>,data...>: public data...
{
private:
  index_table<Indexes...> i_;

public:
  using data::operator[]...;

  __host__ __device__ constexpr auto& index()const{ return i_;}

  __host__ __device__ constexpr auto& index(){ return i_;}

  __host__ __device__ constexpr mapu(index_table<Indexes...>&& i ,data&& ...d): data{std::move(d)}...,i_{std::move(i)}{}

  constexpr mapu()=default;


};

template<class... Indexes, class ...data>
mapu(index_table<Indexes...>,data...)->mapu<index_table<Indexes...>,data...>;



#endif // QM_MAPU_H
