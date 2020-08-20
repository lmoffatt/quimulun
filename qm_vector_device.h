#ifndef QM_VECTOR_DEVICE_H
#define QM_VECTOR_DEVICE_H


#include "static_string.h"

//#include <type_traits>
//#include <utility>
//#include <vector>
//#include <cstdio>


template<class T>
class vector_device
{
private:
  T* v_=nullptr;
  std::size_t n_=0;

public:
  __host__ __device__
      auto& operator[](std::size_t i){return *(v_+i);}
  __host__ __device__
      auto& operator[](std::size_t i)const {return *(v_+i);}

  __host__ __device__
      explicit vector_device(vector_device&& other):v_{&other[0]},n_{other.size()}
  {
    other.v_=nullptr;
    other.n_=0;
  }

   __host__ __device__  constexpr explicit vector_device(std::size_t n):v_{new T[n]},n_{n}{}

  __host__ __device__
      vector_device& operator=(vector_device&& other)const
  {
    (*this)=other;
    other.v_=nullptr;
    other.n_=0;
  }

 // __host__ __device__
  vector_device()=default;

  __host__ __device__
      ~vector_device()
  {
    delete[] v_;
  }

#ifndef __CUDA_ARCH__
//host code here
#else
//device code here
#endif

  __host__ __device__
      constexpr  auto size()const{return n_;}
  __host__ __device__
      constexpr  auto byte_size()const  {return sizeof (std::size_t)+size()*sizeof (T);}


  friend
      __host__ __device__
      void my_print(const vector_device& me)
  {
    printf("{");
    for (std::size_t i=0; i<me.size(); ++i)
      my_print(me[i]);
    printf("}");
  }



};


#endif // QM_VECTOR_DEVICE_H
