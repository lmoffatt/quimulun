#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H


#include "qm_vector_space.h"

namespace ten {



template<class...> class vector_field;





template<class Value_type,class... Xs> class vector_field<vec<Xs...>,Value_type>
{
public:
  typedef Value_type element_type;

  typedef typename vec<Xs...>::template to_vector<Value_type> value_type;

  typedef vec<Xs...> myIndexes;
  constexpr static  auto numIndex=sizeof... (Xs);
private:
  value_type value_;



public:
  vector_field(value_type&& x):value_{std::move(x)}{}
  vector_field(up<Xs...>,value_type&& x):value_{std::move(x)}{}


  template<class Position>
  auto& operator()(const Position& p){ return vec<Xs...>::get(value_,p);}

  template<class Position>
  auto& operator()(const Position& p)const { return vec<Xs...>::get(value_,p);}


  template<class I,class Position>
  auto& operator()(I i,const Position& p){ return vec<Xs...>::get_I(i,value_,p);}

  template<class I,class Position>
  auto& operator()(I i,const Position& p)const { return vec<Xs...>::get_I(i,value_,p);}


  template<class I,class Position>
  auto size(I,const Position& p)const->std::enable_if_t<is_in_pack(I{},Cs<Xs...>{}),std::size_t>
  {
    return vec<Xs...>::size(I{},value_,p);
  }

  auto& value() {return value_;}
  auto& value()const {return value_;}

  static auto begin() {return Position<Xs...>{};}



  template<class I>
  static auto begin(I)->std::enable_if_t<is_in_pack(I{},Cs<Xs...>{}),pack_until_this_t<I,Position<Xs...>>>

  {return pack_until_this_t<I,Position<Xs...>>{};}


  template<class... Ts>
  auto next(Position<Ts...>& p)const->std::enable_if_t<is_contained_in(Cs<Ts...>{},Cs<Xs...>{}),bool>
  {
    return vec<Ts...>::next(value_,p);
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


template<class Value_type,class... Xs> struct get_myIndexes <vector_field< vec<Xs...>,Value_type>>
{
  typedef vec<Xs...> type;
};


template <class Vector, class Position, class... Datum>
void fill_vector_field(Vector&,Position& ,vec<>, const Datum&...  )
{}

template <class Vector, class Position, class... X1>
void fill_vector_field(Vector&,Position& ,vec<X1...>)
{}


template <class Vector, class X, class... X1, class Position,class Id, class Value, class... Datum1>
void fill_vector_field(Vector&v,Position& p,vec<X,X1...> ,const x_i<Id,Value>& one,const Datum1&... two)
{
  if constexpr (is_in_pack(X{},typename x_i<Id,Value>::myIndexes{}))
  {
    auto n=one().size(X{},p);
    v.resize(n);
    auto& i=p[X{}]();
    for (i=0; i<n; ++i)
      fill_vector_field(v[i],p,vec<X1...>{},one,two...);
  }
  else fill_vector_field(v,p,vec<X,X1...>{},two...);
}




template< class Value_type,class...Datas>
auto consolidate(const Datas...d)
{
  typedef decltype ((vec<>{}<<...<<typename get_myIndexes<Datas>::type{})) myvec;

  typedef vector_field<myvec,Value_type> myField;



  typedef typename myField::value_type myValue_type;

  myValue_type out;
  auto p=myField::begin();

  fill_vector(out,p,myvec{},d...);

  return myField(std::move(out));
}

template< class Value_type,class... myIndex,class...Datas>
auto consolidate(vec<myIndex...>,const Datas...d)
{
  typedef decltype ((...<<typename Datas::myIndexes{})<<vec<myIndex...>{}) myvec;

  if constexpr (myvec::index_size==0)
  {
    return Value_type{};
  }
  else
  {
  typedef vector_field<myvec,Value_type> myField;

  typedef typename myField::value_type myValue_type;

  myValue_type out;
  auto p=myField::begin();

  fill_vector_field(out,p,myvec{},d...);

  return myField(std::move(out));
  }
}


} // namespace ten





#endif // QM_VECTOR_FIELD_H
