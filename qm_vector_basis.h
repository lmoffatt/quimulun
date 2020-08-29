#ifndef QM_VECTOR_BASIS_H
#define QM_VECTOR_BASIS_H

#include "qm_value.h"


template<class Id, typename T>
class x_i
{

private:
  T x_;

public:

  inline static constexpr const auto x_iname=my_static_string("x_i(");
  inline static constexpr const auto name=x_iname+Id::name+my_static_string(")");
  using myId=Id;


  __host__ __device__ static constexpr auto title(){return Id::name;}


  __host__ __device__
      constexpr x_i(Id, T&& x):x_{std::move(x)}{}

  __host__ __device__
      constexpr x_i(Id, T const& x):x_{x}{}

  constexpr x_i()=default;


  __host__ __device__
      constexpr auto& operator[](Id)const {return *this;}
  __host__ __device__
      constexpr auto& operator[](Id) {return *this;}
  __host__ __device__
      constexpr auto& operator()()const {return x_;}
  __host__ __device__
      constexpr auto& operator()() {return x_;}


  friend
      __host__ __device__
      void my_print(const x_i& me)
  {
    my_print("x_i(");
    my_print(Id::my_name());
    my_print("=");
    my_print(me());
    my_print(")");

  }



  friend
      std::ostream& operator<<(std::ostream& os, const x_i& me)
  {
    return os<<x_iname.str()<<Id::name.str()<<"="<<me()<<")";

  }
};







#endif // QM_VECTOR_BASIS_H
