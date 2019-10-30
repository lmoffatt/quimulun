#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H

#include "qm_value.h"
#include <map>
#include <vector>
#include <algorithm>


template<class...> struct vec;
template<class... T>
struct get_Field_Indexes;



template<class... T>
struct get_All_Indexes;

template<class T>
struct get_Field_Indexes<T>{
  typedef vec<> type;
};


template<class T>
struct get_All_Indexes<T>{
  typedef vec<> type;
};

template<class... Ts>
using get_All_Indexes_t=typename get_All_Indexes<Ts...>::type;


template<> struct vec<>
{
  template <class T>
  using to_vector=T;

  static inline constexpr auto index_size=0;

  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& ,ValueType&& e)
  {
    x=std::move(e);
  }
  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& p,typename ValueType::row_type&& e)
  {
    x.insert_at(p,std::move(e));
  }

  template <class Vector, class Position>
  static auto& get(Vector& x, const Position&)
  {
    return x;
  }
  template <class Vector, class Position>
  static auto& get(const Vector& x,const Position&)
  {
    return x;
  }
  template <class Vector, class Position>
  static bool next(const Vector& ,Position& ) {return false;}

};
template<class I0, class...I> struct vec<I0,I...>{

  static inline constexpr auto index_size=sizeof... (I)+1;

  template <class T>
  using to_vector=std::vector<typename vec<I...>::template to_vector<T>>;

  template <class ValueType, class Positionx>
  static auto& get(std::vector<ValueType>& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }
  template <class ValueType, class Positionx>
  static auto& get(const std::vector<ValueType>& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }



  template <class ValueType, class Positionx, class elementType>
  static void insert_at(std::vector<ValueType>& x,const Positionx& p,elementType&& e)
  {
    if (x.size()==p[I0{}]())
      push_back(x,std::forward<elementType>(e));
    else
      vec<I...>::insert_at(x.at(p[I0{}]()),p, std::forward<elementType>(e));
  }


  template <class ValueType>
  static std::vector<ValueType>& push_back(std::vector<ValueType>& x,ValueType&& e)
  {
    x.push_back(std::forward<ValueType>(e));
    return x;
  }
  template <class ValueType>
  static std::vector<ValueType> push_back(std::vector<ValueType>&& x,ValueType&& e)
  {
    x.push_back(std::forward<ValueType>(e));
    return x;
  }

  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,typename ValueType::row_type >,std::vector<ValueType>&>{

    x.emplace_back(std::move(e));
    return x;
  }
  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>&& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,typename ValueType::row_type >,std::vector<ValueType>>{

    x.emplace_back(std::move(e));
    return x;
  }




  template <class ValueType, class elementType, typename=std::enable_if_t<!std::is_same_v<ValueType,elementType >,int>>
  static std::vector<ValueType>&  push_back(std::vector<ValueType>& x,elementType&& e)
  {
    auto v=ValueType{};
    return push_back(x,push_back(std::move(v),std::forward<elementType>(e)));
  }
  template <class ValueType, class elementType, typename=std::enable_if_t<!std::is_same_v<ValueType,elementType >,int>>
  static std::vector<ValueType>  push_back(std::vector<ValueType>&& x,elementType&& e)
  {
    auto v=ValueType{};
    return push_back(x,push_back(std::move(v),std::forward<elementType>(e)));
  }







  template <class ValueType, class Positionx>
  static auto& get(ValueType& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }
  template <class ValueType, class Positionx>
  static auto& get(const ValueType& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }




  template <class Vector, class Position>
  static auto& get_I(I0,const Vector& x,const Position& )
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i,const Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }



  template <class Vector, class Position>
  static auto& get_I(I0,Vector& x,const Position& )
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i, Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }


  template <class Vector, class Position>
  static auto size(I0,const Vector& x,const Position& )
  {
    return x.size();
  }

  template <class In,class Vector, class Position>
  static auto size(In i,const Vector& x,const Position& p)->std::enable_if_t<is_in_pack<In,I...>(),std::size_t>
  {
    return vec<I...>::size(i,x.at(p[I0{}]()),p);
  }


  template <class Vector, class Position>
  static bool next(const Vector& x,Position& p)
  {
    if(vec<I...>::next(x.at(p[I0{}]()),p))
      return true;
    else {
      ++p[I0{}]();
      if (p[I0{}]()<x.size())
        return true;
      else {
        p[I0{}]()=0;
        return false;
      }
    }
  }
};


template<class... Ts>
using get_Field_Indexes_t=typename get_Field_Indexes<Ts...>::type;



template<class...x_is> struct get_Field_Indexes
{
  typedef decltype((vec<>{}<<...<<typename get_Field_Indexes<x_is>::type{})) type;
};

template<class...x_is> struct get_All_Indexes
{
  typedef decltype((vec<>{}<<...<<typename get_All_Indexes<x_is>::type{})) type;
};

template <class V>
constexpr auto size_Field_Indexes(V) {return get_Field_Indexes_t<V>::index_size;}



template<class...I0, class...I1>
auto operator<<(vec<I0...>, vec<I1...>)
{

  return transfer_t<pack_difference_t<Cs<I1...>,Cs<I0...>>,vec<I0...>>{};


}





/*
template<class T, class U>
void copy_size(const U& ,T& )
{
}

template<class T, class U>
void copy_size(const std::vector<U>& source,std::vector<T>& destination)
{
  destination.resize(source.size());
}

template<class T, class U>
void copy_size(const std::vector<std::vector<U>>& source,std::vector<std::vector<T>>& destination)
{
  destination.resize(source.size());
  for (std::size_t i=0; i<source.size(); ++i) copy_size(source[i],destination[i]);

}
*/


template<class...> class vector_field;


template<class Value_type,class... Xs> class vector_field<vec<Xs...>,Value_type>
{
public:
  typedef Value_type element_type;

  typedef typename vec<Xs...>::template to_vector<Value_type> value_type;

  typedef vec<Xs...> myIndexes;

  using cols=typename element_type::cols;
  using row_type=typename element_type::row_type;

private:
  value_type value_;



public:
  vector_field(value_type&& x):value_{std::move(x)}{}
  // vector_field(up<Xs...>,value_type&& x):value_{std::move(x)}{}


  vector_field()=default;

  template<class Position>
  auto& operator()(const Position& p){ return vec<Xs...>::get(value_,p);}

  template<class Position>
  auto& operator()(const Position& p)const { return vec<Xs...>::get(value_,p);}


  template<class I,class Position>
  auto& operator()(I i,const Position& p){ return vec<Xs...>::get_I(i,value_,p);}

  template<class I,class Position>
  auto& operator()(I i,const Position& p)const { return vec<Xs...>::get_I(i,value_,p);}

  template<class...Is>
  void insert_at(const Position<Is...>& p, element_type&& r)
  {
    vec<Xs...>::insert_at(value_,p,std::move(r));
  }

  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {
    vec<Xs...>::insert_at(value_,p,std::move(r));
  }


  template<class I,class Position>
  auto size(I,const Position& p)const->std::enable_if_t<is_in_pack(I{},Cs<Xs...>{}),std::size_t>
  {
    return vec<Xs...>::size(I{},value_,p);
  }

  auto& value() {return value_;}
  auto& value()const {return value_;}


  inline friend
      bool operator==(const vector_field& me, const vector_field& ti)
  {
    return me.value()==ti.value();
  }

  static auto begin() {return Position<Xs...>{};}

  friend constexpr auto begin(const vector_field& ){ return vector_field::begin();}

  template<class I>
  static auto begin(I)->std::enable_if_t<is_in_pack(I{},Cs<Xs...>{}),pack_until_this_t<I,Position<Xs...>>>

  {return pack_until_this_t<I,Position<Xs...>>{};}





  template<class... Ts>
  auto next(Position<Ts...>& p)const->std::enable_if_t<is_contained_in(Cs<Ts...>{},Cs<Xs...>{}),bool>
  {
    return vec<Ts...>::next(value_,p);
  }

  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const vector_field& me)->std::enable_if_t<is_contained_in(Cs<Ts...>{},Cs<Xs...>{}),bool>
  {
    return vec<Ts...>::next(me.value(),p);
  }

  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const vector_field& )->std::enable_if_t<!is_contained_in(Cs<Ts...>{},Cs<Xs...>{}),Position<Ts...>&>
  {
    return p;
  }

  friend  bool operator +(bool p,const vector_field& )
  {
    return p;
  }



  bool next(Position<Xs...>& p)const
  {
    return vec<Xs...>::next(value_,p);
  }


  friend std::ostream& operator<<(std::ostream& os, const vector_field& me)
  {
    auto p=me.begin();
    do{
      os<<me(p)<<" ";
    }  while(me.next(p));
    return os;
  }
};




template<class Value_type,class... Xs> struct get_Field_Indexes <vector_field< vec<Xs...>,Value_type>>
{
  typedef vec<Xs...> type;
};




#endif // QM_VECTOR_FIELD_H
