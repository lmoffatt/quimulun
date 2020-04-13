#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H

#include "qm_value.h"
#include <map>
#include <vector>
#include <algorithm>


template<class...Args>
struct Arguments{
  explicit Arguments(Args&&...){}
  Arguments()=default;
};
template<>struct Arguments<>{};



template<class...Args>
struct Index_struct{
  Index_struct(Args&&...){}
  Index_struct()=default;
};
template<>struct Index_struct<>{};


template<class...Args>
struct Outputs{};

template<class Id_of_vector, class sub_Id> struct sub{};

template<class IdField, class IdPosValue> struct subElement{};

template<class Id> struct pos_new{};

template<class Id> struct index_new{};




template<class Id> struct temp{};

template<class Id> struct pass_new{};


template<class Id, class Index> struct Start_new{};

template<class Id, class Index> struct Next_new{};

template<class Index> struct End_new;

template<class Id, class Index> struct Size_at_Index_new{};


template<class Id> struct From{};
template<class Id> struct To{};

template<class Id> struct Out{};



template <class F, class G>
class FoG{
  F f_;
  G g_;
public:
  auto& f()const{return f_;}
  auto& g()const{return g_;}

  FoG(F&& af, G&&ag):f_{std::move(af)},g_{std::move(ag)}{}

};





template <class F, class... Args>
struct Arguments_not_found{

  Arguments_not_found(const F&, const Not_found<Args...>&){}

};


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
using get_Field_Indexes_t=typename get_Field_Indexes<std::decay_t<Ts>...>::type;



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

template< class Value_type,class myIndex,class...Datas>
auto consolidate(vec<myIndex>,const Datas&...d);
template< class Value_type,class...Datas>
auto consolidate(vec<>,const Datas&...d);

template <class Vector, class X, class... X1, class Position, class Value, class... Datum1>
void fill_vector_field(Vector&v,Position& p,vec<X,X1...> ,const Value& one,const Datum1&... two);

template<class...> class vector_field;


template<class Value_type,class... Xs> class vector_field<vec<Xs...>,Value_type>
{
public:
  typedef Value_type element_type;

  typedef typename vec<Xs...>::template to_vector<Value_type> value_type;

  typedef vec<Xs...> myIndexes;



private:
  value_type value_;



public:
  vector_field(value_type&& x):value_{std::move(x)}{}
  // vector_field(up<Xs...>,value_type&& x):value_{std::move(x)}{}


  vector_field()=default;
  //vector_field(vector_field&&)=default;


  template<class value_type2>
  vector_field<vec<Xs...>,value_type2> create()const {
    typedef vector_field<vec<Xs...>,value_type2> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type out;
    auto p=myField::begin();

    fill_vector_field(out,p,vec<Xs...>{},*this);

    return myField(std::move(out));

  }

  vector_field create()const {
    value_type out;
    auto p=begin();

    fill_vector_field(out,p,vec<Xs...>{},*this);

    return vector_field(std::move(out));

  }


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

  template<class I,class Position>
  auto size(I,const Position& )const->std::enable_if_t<!is_in_pack(I{},Cs<Xs...>{}),std::size_t>
  {
    return 0ul;
  }

  auto& value() {return value_;}
  auto& value()const {return value_;}


  inline friend
      bool operator==(const vector_field& me, const vector_field& ti)
  {
    return me.value()==ti.value();
  }

  static constexpr auto begin() {return Position<Xs...>{};}
  static constexpr auto rec_begin() {return begin() && Value_type::rec_begin();}

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

template<class...> struct vector_field_size;

template<class Value_type,class... Xs, class Index>
struct vector_field_size<vector_field<vec<Xs...>,Value_type>, Index>
{
  vector_field<vec<Xs...>,Value_type>const & v_;

  vector_field_size(vector_field<vec<Xs...>,Value_type>const & v, Index): v_{v}{}

  template<class Position>
  auto operator()(const Position& p)const
  {
    return v(v_.size(Index{},p),dimension_less{});
  }
  auto& operator()()const {return *this;}

};

template<class...> struct x_i_vector_field_const;

template<class Id,class... xs,class... Xs>
struct x_i_vector_field_const<vector_field<vec<Xs...>,vector_space<xs...>>, Id>
{
  vector_field<vec<Xs...>,vector_space<xs...>>const & v_;

  x_i_vector_field_const(vector_field<vec<Xs...>,vector_space<xs...>>const & v, Id): v_{v}{}

  template<class Position>
  auto operator()(const Position& p)const->decltype (v_(p)[Id{}]())
  {
    return v_(p)[Id{}]();
  }



  auto& operator()()const {return *this;}

};





template <class Vector, class Position, class... Datum>
void fill_vector_field(Vector&,Position& ,vec<>, const Datum&...  )
{}

template <class Vector, class Position, class X1>
void fill_vector_field(Vector& v,Position& ,vec<X1>)
{
  v.resize(1);

}


template <class Vector, class X, class... X1, class Position, class Value, class... Datum1>
void fill_vector_field(Vector&v,Position& p,vec<X,X1...> ,const Value& one,const Datum1&... two)
{
  if constexpr (is_in_pack(X{},get_Field_Indexes_t<Value>{}))
  {
    auto n=one.size(X{},p);
    v.resize(n);
    auto& i=p[X{}]();
    for (i=0; i<n; ++i)
      fill_vector_field(v[i],p,vec<X1...>{},one,two...);
  }
  else fill_vector_field(v,p,vec<X,X1...>{},two...);
}



template< class Value_type,class... myIndex,class...Datas>
auto consolidate_old(vec<myIndex...>,const Datas&...d)
{
  typedef decltype ((...<<get_Field_Indexes_t<Datas>{})<<vec<myIndex...>{}) myvec;

  if constexpr (myvec::index_size==0)
  {
    return Value_type{};
  }
  else
  {
    typedef vector_field<myvec,Value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type out{};
    auto p=myField::begin();

    fill_vector_field(out,p,myvec{},d...);

    return myField(std::move(out));
  }
}



template< class Value_type,class...Datas>
auto consolidate(vec<>,const Datas&...d)
{
  typedef decltype ((...<<get_Field_Indexes_t<Datas>{})) myvec;

  if constexpr (myvec::index_size==0)
  {
    return Value_type{};
  }
  else
  {
    typedef vector_field<myvec,Value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type out{};
    auto p=myField::begin();

    fill_vector_field(out,p,myvec{},d...);

    return myField(std::move(out));
  }
}

template< class Value_type,class myIndex,class...Datas>
auto consolidate(vec<myIndex>,const Datas&...d)
{
  typedef decltype ((...<<get_Field_Indexes_t<Datas>{})<<vec<myIndex>{}) myvec;
  //typedef decltype ((...<<get_Field_Indexes_t<Datas>{})) myvecs;

  if constexpr (myvec::index_size==0)
  {
    return Value_type{};
  }
  else
  {
    typedef vector_field<myvec,Value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type out{};
    auto p=myField::begin();

    fill_vector_field(out,p,myvec{},d...);

    return myField(std::move(out));
  }
}





template<class Value_type,class... Xs> struct get_Field_Indexes <vector_field< vec<Xs...>,Value_type>>
{
  typedef vec<Xs...> type;
};




#endif // QM_VECTOR_FIELD_H
