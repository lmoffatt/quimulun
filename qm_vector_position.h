#ifndef QM_VECTOR_POSITION_H
#define QM_VECTOR_POSITION_H
#include "static_string.h"

template <class...> struct Position;

template< >struct Position<>{};

template <class Id> struct Position<Id>{
private :
  std::size_t i;
public:
  static constexpr auto pos_name=my_static_string("pos_");
  static constexpr auto name=pos_name+Id::name;
  __device__ __host__ Position(std::size_t i_v): i{i_v}{}
  Position()=default;
  using innerId=Id;
  __host__ __device__ auto& operator[](Id)const {return *this;}
  __host__ __device__ auto& operator[](Id) {return *this;}
  __host__ __device__ auto& operator()()const {return  i;}

  __device__ __host__ auto& operator()() {return  i;}

  friend   __host__ __device__  void my_print(const Position& me) {
    printf(name.c_str()),
        printf("=");
    my_print(me());
  }

};


template <class Id, class... Ids> struct Position<Id,Ids...>:Position<Id>,Position<Ids>...
{
  static constexpr auto name=(Position<Id>::name+...+Position<Ids>::className);


  template<typename... sizes, typename=std::enable_if_t<(std::is_same_v<sizes,std::size_t >&&...)&&sizeof... (sizes)==sizeof... (Ids)>>
  __host__ __device__ constexpr explicit Position (std::size_t i, sizes...is):Position<Id>{i},Position<Ids>{is}...{}

  constexpr Position()=default;

  template<class ...Xs>
  __host__ __device__ constexpr auto& operator()(const Position<Xs...>& ) {return *this;}

  template<class ...Xs>
  __host__ __device__ constexpr auto& operator()(const Position<Xs...>& )const {return *this;}

  using Position<Id>::operator[];
  using Position<Ids>::operator[]...;

  friend   __host__ __device__  void my_print( const Position& me) {
    my_print(me[Id{}]);
    if constexpr (sizeof... (Ids)>0)
      ((my_print("_"),my_print(me[Ids{}])),...);
  }

};


#endif // QM_VECTOR_POSITION_H
