#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H

//#include "qm_data.h"
#include "qm_unit.h"
#include <map>
#include <vector>
#include <algorithm>

namespace ten {

struct Iu_{};
struct Ju_{};
struct Id_{};
struct Jd_{};


template<class T>
struct get_myIndexes{
  typedef vec<> type;
};


template<class ...Is> struct dn;

template<class ...Is> struct up;

template <class, class> struct te;



struct Scalar{};

template<class ...Is> struct dn{

  template<class... as>
  Nothing operator()(const up<as...>& ){return Nothing{};}

  Scalar operator()(const up<Is...>& ){return Scalar{};};
};

template<class ...Is> struct up{
  template<class... as>
  Nothing operator()(const dn<as...>& ){return Nothing{};}

  Scalar operator()(const dn<Is...>& ){return Scalar{};};

};


template<class ...>struct ein;

template<class te,class Up, class Down>
struct ein<te,Up,Down>{

};


template <class ...Ups, class ...Downs>
struct te<up<Ups...>,dn<Downs...>>{
//  template<class... as, class... bs>
//  Nothing operator()(const ve<as...>&, const co<bs...>& ){return Nothing{};}
//  Scalar operator()(ve<Ups...>,co<Downs...>){return Scalar{};};

friend auto &operator<<(std::ostream& os, te)
  {
  if constexpr (sizeof... (Downs)>0)
    ((os<<"d/d"<<Downs{}<<"_"),...);
  if constexpr (sizeof... (Ups)>0)
    ((os<<Ups{}<<"_"),...);
  return os;



  }


  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...>, dn<iDowns...>){return ein<te,up<iUps...>,dn<iDowns...>>{};}

  template <class ...Ups2, class ...Downs2>
  friend
      auto operator*(te,te<up<Ups2...>,dn<Downs2...>>){
    return te<up<Ups...,Ups2...>,dn<Downs...,Downs2...>>{};
  }
};



template <class ...Ups0, class ...iUps0,class a, class ia,
          class b>

constexpr auto operator * (ein<te<up<Ups0...>,dn<a>>,up<iUps0...>,dn<ia>>,
               ein<te<up<b>,dn<>>,up<ia>,dn<>>)
{

  /**
        -**         -A
       |          |
       |      *   |
       |          |
        -A

  **/
  if constexpr (std::is_same_v<a,b >)
      return ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>{};
  else
    return Nothing{};

}

template <class ...Ups0, class ...iUps0,
          class Up1, class iUp1>
constexpr auto operator * (ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>,
               ein<te<up<Up1>,dn<>>,up<iUp1>,dn<>>)
{

  /**
        -**         -A            -**A
       |          |              |
       |      *   |        -->   |
       |          |              |


  **/
      return ein<te<up<Ups0...,Up1>,dn<>>,up<iUps0...,iUp1>,dn<>>{};

}



template <class ...Ups0, class ...iUps0,class A, class iA,
          class B, class iB>

constexpr auto operator * (ein<te<up<Ups0...>,dn<A>>,up<iUps0...>,dn<iA>>,
               ein<te<up<B>,dn<>>,up<iB>,dn<>>)
{

  /**
        -**         -B            -**B
       |          |              |
       |      *   |        -->   |
       |          |              |
        -A                        -A

  **/
      return ein<te<up<Ups0...,B>,dn<A>>,up<iUps0...,iB>,dn<iA>>{};

}

template <class ...Downs0, class ...iDowns0>
constexpr Nothing join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,Nothing)
{
  return Nothing{};
}



template <class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,
          ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return ein<te<up<Ups1...>,dn<Downs0...,Downs1...>>,up<iUps1...>,dn<iDowns0...,iDowns1...>>{};
}



template <class Up0, class iUp0,class Up00, class iUp00,class ...Ups0, class ...iUps0,class Down1, class iDown1,class ...Downs0, class ...iDowns0>

constexpr auto operator * (ein<te<up<Up0,Up00,Ups0...>,dn<Downs0...>>,up<iUp0,iUp00,iUps0...>,dn<iDowns0...>>,
               ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>)
{

  /**
             -***                                                         -*      -*    -*
            |          |                        |             |          |       |     |
            |      *   |           --> join (   |      ,      |      *   |   *   |  *  |     )
            |          |                        |             |          |       |     |
             -***       -*                       -***          -*

  **/


  return  join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>{},
              ((ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{}*
 ein<te<up<Up0>,dn<>>,up<iUp0>,dn<>>{}*
 ein<te<up<Up00>,dn<>>,up<iUp00>,dn<>>{})*...*
                  ein<te<up<Ups0>,dn<>>,up<iUps0>,dn<>>{})
                  );

}




template <class ...Ups, class ...iUps,class Down0, class iDown0,class Down1, class iDown1,class ...Downs, class ...iDowns>

constexpr auto operator * (ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>,
               ein<te<up<>,dn<Down0,Down1,Downs...>>,up<>,dn<iDown0,iDown1,iDowns...>>)
{
  /**
             -***                                       -***
            |          |                                |          |       |     |
            |      *   |           -->                  |      *   |   *   |  *  |
            |          |                                |          |       |     |
                        -***                                       -*      -*    -*

  **/

  return ((ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>{}*
           ein<te<up<>,dn<Down0>>,up<>,dn<iDown0>>{}*ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{})
          *...*
          ein<te<up<>,dn<Downs>>,up<>,dn<iDowns>>{});
}


template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0>
constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
               Nothing)
{

      return Nothing{};

}

template <          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto operator * (Nothing,ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return Nothing{};

}




template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>

constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
               ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{

  /**
        -**         -**                     -****
       |          |                        |          |
       |      *   |            -->         |      *   |
       |          |                        |          |
        -**         -**                                -****

  **/
  return ein<te<up<Ups0...,Ups1...>,dn<>>,up<iUps0...,iUps1...>,dn<>>{}*
         ein<te<up<>,dn<Downs0...,Downs1...>>,up<>,dn<iDowns0...,iDowns1...>>{};

}

//template <class...> struct x_i;


template<class e_i, class Value_type>
struct x_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;

  typedef e_i myId;

  typedef typename Value_type::element_type element_type;

  typedef typename get_myIndexes<Value_type>::type myIndexes;


  typedef  Value_type value_type;
private:
  Value_type value_;
public:
  x_i(value_type&& x):value_{std::move(x)}{}
  x_i(e_i,value_type&& x):value_{std::move(x)}{}
  x_i(e_i,const value_type& x):value_{x}{}
  x_i()=default;
  x_i const& operator[](e_i)const & {return *this;}
  x_i& operator[](e_i) & {return *this;}
  x_i operator[](e_i)&& {return *this;}

  auto& operator()()const &{ return value_;}
  auto& operator()()&{ return value_;}
  auto operator()()&& {return value_;}


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
    return {e_i{},me()*a};
  }

  template<class unit,class T>
  friend auto operator*(const v<T,unit>& a,const x_i& me)->x_i<e_i,decltype (v<T,unit>{}*Value_type{})>
  {
    return {e_i{},a*me()};
  }

  friend x_i operator*(const x_i& me,double a)
  {
    return x_i{e_i{},me()*a};
  }

  friend x_i operator*(double a,const x_i& me)
  {
    return {e_i{},a*me()};
  }



  template<class Value_type_2>
  friend auto operator/(const x_i& me,const Value_type_2& a)->x_i<e_i,decltype (Value_type{}/Value_type_2{})>
  {
    return {e_i{},me()/a};
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


template<class Id, class Value_type,class Datas, class Rand>
x_i<Id,Value_type> sample(const x_i<Id,Value_type>& d, const Datas& , Rand& ){return d;}

template<class Id, class Value_type,class... Datas>
x_i<Id,Value_type> calculate(const x_i<Id,Value_type>& d, const Datas&... ){return d;}


template<class Id, class Value_type,class... Datas>
auto logP(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}

template<class Id, class Value_type,class... Datas>
auto FIM(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}



template< class e_i0,class e_i1, class value_0, class value_1>
auto operator*(const x_i<e_i0,value_0> &one,const x_i<e_i1,value_1> &two )
{
  if constexpr (std::is_same_v<Nothing,decltype (e_i0{}*e_i1{}) >)
    return Nothing{};
  else
    return x_i(e_i0{}*e_i1{},one()*two());
}



template <class...> struct vector_space;

template<> struct vector_space<>
{
  typedef Cs<> my_eis;
  typedef Cs<> myIds;
  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }









  vector_space()=default;

  template<class x_i>
  auto operator + (x_i&& x)&&
  {
    return vector_space<std::decay_t<x_i>>{std::forward<x_i>(x)};
  }
  template<class ...x_is2>
  auto operator +(const vector_space<x_is2...>& other)const &
  {
    return other;
  }
  template<class ...x_is2>
  auto operator +( vector_space<x_is2...>&& other)&&
  {
    return std::move(other);
  }


};



template<class...x_is> struct vector_space: public x_is...
{
  using x_is::operator[]...;
  typedef Cs<typename x_is::ei...> my_eis;
  typedef Cs<typename x_is::myId...> myIds;


  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }


  template<class Value_2>
  auto operator/(const Value_2& a)const
  {
    return vector_space<decltype (x_is{}/a)...>{(*this)[typename x_is::ei{}]/a...};
  }

  template<class x_j>
  auto operator * (const x_j&  two)const
  {
    return (vector_space<>{}|...|((*this)[typename x_is::ei{}]*two));
  }


  vector_space(x_is&&...xs):x_is{std::move(xs)}...{}


  auto operator+=(const vector_space& other)
  {
    (((*this)[typename x_is::ei{}]+=other[typename x_is::ei{}]),...);
    return *this;
  }

  template<class x_i>
  auto operator + (x_i&& x)&&
  {
    if constexpr (is_in_pack<x_i,x_is...>())
    {
      (*this)[typename x_i::ei{}]=std::forward<x_i>(x);
      return *this;
    }
    else
    {
      return    vector_space<x_is...,std::decay_t<x_i>>{std::move(*this)[typename x_is::ei{}]...,std::forward<x_i>(x)};
    }
  }


  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space(const vector_space<x_is_1...>&x,const vector_space<x_is_2...>& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{x[typename x_is_1::ei{}]}...,x_not_common{y[typename x_not_common::ei{}]}...{
    (((*this)[typename x_is_common::ei{}]+=y[typename x_is_common::ei{}]),...);
  }
  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space( vector_space<x_is_1...>&&x, vector_space<x_is_2...>&& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{std::move(x)[typename x_is_1::ei{}]}...,x_not_common{std::move(y)[typename x_not_common::ei{}]}...{
    (((*this)[typename x_is_common::ei{}]+=y[typename x_is_common::ei{}]),...);
  }

  vector_space()=default;


  auto operator-()&&
  {
    (((*this)[typename x_is::ei{}]=(*this)[typename x_is::ei{}]),...);
    return std::move(*this);
  }
  auto operator-()const&
  {
    auto out=*this;
    ((out[typename x_is::ei{}]=out[typename x_is::ei{}]),...);
    return out;
  }


  template<class ...x_is2>
  auto operator +(const vector_space<x_is2...>& other)const &
  {
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(*this,other,x_common{},x_not_common{});
  }
  template<class ...x_is2>
  auto operator +( vector_space<x_is2...>&& other)&&
  {
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(std::move(*this),std::move(other),x_common{},x_not_common{});
  }

  template<class ...x_is2>
  auto operator -(const vector_space<x_is2...>& other)const &
  {
    auto x=other*(-1.0);
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(*this,x,x_common{},x_not_common{});
  }
  template<class ...x_is2>
  auto operator -( vector_space<x_is2...>&& other)&&
  {
    other=std::move(other)*v(-1.0,dimension_less{});
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(std::move(*this),std::move(other),x_common{},x_not_common{});
  }




  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){
    return vector_space<std::decay_t<decltype ((*this)[typename x_is::ei{}](u,d))>...>
        {(*this)[typename x_is::ei{}](u,d)...};

  }


  friend std::ostream& operator<<(std::ostream& os, const vector_space& me)
  {
    ((os<<typename x_is::myId{}<<"="<<me[typename x_is::myId{}]<<"  ")&&...);
    return os;
  }



};






template<class x_i>
auto operator | (x_i&& one, Nothing)
{
    return vector_space<x_i>{std::move(one)};
}
template<class x_i>
auto operator | (Nothing,x_i&& one)
{
  return vector_space<x_i>{std::move(one)};
}

auto operator | (Nothing,Nothing)
{
  return Nothing{};
}

template<class... x_is>
auto operator | (vector_space<x_is...>&& one, Nothing)
{
  return std::move(one);
}


template<class e_i, class Value_type_i, class e_j, class Value_type_j>
auto operator | (x_i<e_i,Value_type_i>&& one, x_i<e_j,Value_type_j> && two)
{
  if constexpr (std::is_same_v<e_i,e_j >)
    return vector_space<x_i<e_i,Value_type_i>>(std::move(one)+std::move(two));
  else
    return vector_space<x_i<e_i,Value_type_i>,x_i<e_j,Value_type_j>>{std::move(one),std::move(two)};
}

template<class... x_is, class x_j>
auto operator | (vector_space<x_is...>&& one, x_j && two)
{
  if constexpr (is_in_pack<x_j,x_is... >())
  {
    one[typename x_j::ei{}]+=std::move(two);
    return std::move(one);
  }
  else
    return vector_space<x_is...,x_j>{std::move(one)[typename x_is::ei{}]...,std::move(two)};
}



template<class... x_is, class x_j>
auto operator * (const x_j&  two,const vector_space<x_is...>& one)
{
  return (vector_space<>{}|...|(two*one[typename x_is::ei{}]));
}


template<class... x_is, class... x_js>
auto operator * (const vector_space<x_is...>& one,const vector_space<x_js...>&  two)
{
  return ((one*two[typename x_js::ei{}])+...);
}




template<bool Complete,class... Ds, class Id2, class Value_type>
Is_Complete<Complete,vector_space<Ds...,x_i<Id2,Value_type>>>
operator |
    (Is_Complete<Complete,vector_space<Ds...>>&& d, x_i<Id2,Value_type>&& d2)
{
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,x_i<Id2,Value_type>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}


template<bool Complete,class... Ds>
Is_Complete<false,vector_space<Ds...>> operator |(Is_Complete<Complete,vector_space<Ds...>>&& d, Nothing)
{
  return Is_Complete(std::bool_constant<false>{},std::move(d.value));
}


} // namespace ten



#endif // QM_VECTOR_SPACE_H
