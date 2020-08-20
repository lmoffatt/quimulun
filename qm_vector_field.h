#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H

#include "qm_vector_device.h"
#include "qm_vector_index.h"
#include "qm_vector_position.h"


template<class Value_type,class indice_type>
class v_f
{
public:
  typedef Value_type element_type;

  typedef vector_device<element_type> value_type;


private:
  indice_type i_;
  value_type value_;
  __host__ __device__
    constexpr  auto& value(){return value_;}


  __host__ __device__
     constexpr auto& value()const {return value_;}

public:
  __host__ __device__
     constexpr explicit v_f(const indice_type& i):i_{i},value_{i.size()}{}


  __host__ __device__
     constexpr auto& index()const {return i_;}


  //__host__ __device__
  constexpr v_f()=default;

  __host__ __device__
    constexpr  auto size()const{return value().size();}

  constexpr auto byte_size()const {return value().byte_size() +index().byte_size();}

  template<class...Xs>
  __host__ __device__
  constexpr    decltype(auto) operator()(const Position<Xs...>& p)const { return value()[index()(p)()];}

  template<class...Xs>
  __host__ __device__
    constexpr  decltype(auto) operator()(const Position<Xs...>& p) { return value()[index()(p)()];}


  friend
      __host__ __device__
      void my_print(const v_f& me)
  {
    printf("{");
    for (auto& e:me.value()) { my_print(e);printf(", ");}
    printf("}");
  }

};





#endif // QM_VECTOR_FIELD_H
