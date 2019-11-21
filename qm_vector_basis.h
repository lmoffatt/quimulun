#ifndef QM_VECTOR_BASIS_H
#define QM_VECTOR_BASIS_H
#include "qm_tensor_base.h"


template<class Id> struct non_const{};




template<class e_i, class Value_type>
struct x_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

 // using element_type= element_type_t<Value_type> ;


 // using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;



private:
  Value_type value_;
public:

  explicit x_i(const value_type& x):value_{x}{}
  explicit x_i(value_type&& x):value_{std::move(x)}{}
  explicit x_i(e_i,value_type&& x):value_{std::move(x)}{}
  explicit x_i(e_i,const value_type& x):value_{x}{}

  template<class...Index>
//  ,typename=std::enable_if_t<
//                                std::is_same_v<typename Value_type::myIndexes,
//                                               vec<Index...> >,int>>
  x_i(e_i,vec<Index...>):value_{}{}

  x_i()=default;
  x_i const& operator[](e_i)const & {return *this;}
  //x_i& operator[](e_i) & {return *this;}
  x_i& operator[](non_const<e_i>)  {return *this;}

  friend auto begin(const x_i& me){ return begin(me());}

  x_i operator[](e_i)&& {return *this;}

  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}



  inline friend
      bool operator==(const x_i& me, const x_i& ti)
  {
    return me()==ti();
  }


  auto& operator()()const &{ return value_;}
  auto& operator()()&{ return value_;}
  //auto operator()()&& {return value_;}




  template<class Position>
  auto& operator()(const Position& p){ return value_(p);}

  template<class Position>
  auto& operator()(const Position& p)const { return value_(p);}

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){return x_i<decltype (ei{}(u,d)),value_type>{ei{}(u,d),(*this)()};}

  inline friend std::ostream& operator<<(std::ostream& os, const x_i& me)
  {
    return os<<me();
  }
  inline friend std::istream& operator>>(std::istream& is,  x_i& me)
  {
    return is>>me();
  }


  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const x_i& me)//->decltype (p+me())
  {
    return p+me();
  }
  friend  bool operator +(bool p,const x_i& )
  {
    return p;
  }


  x_i& operator+=(const x_i& other){ (*this)()+=other(); return *this;}


  template<class Value_type_2>
  auto operator*=(const Value_type_2& x)->std::enable_if_t<std::is_same_v<Value_type,decltype (value_*x) >,x_i&>
  {
    (*this)()*=x;
    return *this;
  }

  template<class unit,class T>
  friend auto operator*(const x_i& me,const v<T,unit>& a)->x_i<e_i,decltype (Value_type{}*v<T,unit>{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},me()*a};
  }

  template<class unit,class T>
  friend auto operator*(const x_i& me,const logv<T,unit>& a)->x_i<e_i,decltype (Value_type{}*logv<T,unit>{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},me()*a};
  }


  template<class unit,class T>
  friend auto operator*(const v<T,unit>& a,const x_i& me)->x_i<e_i,decltype (v<T,unit>{}*Value_type{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},a*me()};
  }

  friend x_i operator*(const x_i& me,double a)
  {
    return x_i{e_i{},me()*a};
  }

  friend x_i operator*(double a,const x_i& me)
  {
    return x_i<e_i,std::decay_t<decltype (a*me())>>{e_i{},a*me()};
  }


  template< class e_i1,  class value_1>
  friend auto operator*(const x_i<e_i,Value_type> &one,const x_i<e_i1,value_1> &two )
  {
    if constexpr (std::is_same_v<Nothing,decltype (e_i{}*e_i1{}) >)
      return Nothing{};
    else
      return x_i<decltype (e_i{}*e_i1{}),std::decay_t<decltype(one()*two())>>(one()*two());
  }

  template<class Value_type_2>
  friend auto operator/(const x_i& me,const Value_type_2& a)->x_i<e_i,decltype (Value_type{}/Value_type_2{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()/a)>>{e_i{},me()/a};
  }



  friend x_i operator+(const x_i& me, const x_i& other)
  {
    x_i out{me};
    out()+=other();
    return out;
  }


  friend x_i operator-(const x_i& me, const x_i& other)
  {
    x_i out{me};
    out()-=other();
    return out;
  }

};



template <class varName, class...Index>
x_i(varName,vec<Index...>)->x_i<varName,vector_field<vec<Index...>,v_t<varName>>>;

template <class varName, class... indexName>
using field_t=x_i<varName,vector_field<vec<indexName...>,v_t<varName>>>;


/*
template<class Id, class Value_type,class Datas, class Rand>
x_i<Id,Value_type> sample(const x_i<Id,Value_type>& d, const Datas& , Rand& ){return d;}

template<class Id, class Value_type,class... Datas>
x_i<Id,Value_type> calculate(const x_i<Id,Value_type>& d, const Datas&... ){return d;}


template<class Id, class Value_type,class... Datas>
auto logP(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}

template<class Id, class Value_type,class... Datas>
auto FIM(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}
*/

template<class e_i,class Value_type,class... Xs> struct get_Field_Indexes <x_i<e_i,vector_field< vec<Xs...>,Value_type>>>
{
  typedef vec<Xs...> type;
};
template<class e_i,class Value_type> struct get_Field_Indexes <x_i<e_i,Value_type>>
{
  typedef vec<> type;
};


#endif // QM_VECTOR_BASIS_H
