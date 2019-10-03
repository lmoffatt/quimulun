#ifndef DATA_H
#define DATA_H
#include "qm_unit.h"
#include <map>
#include <vector>
#include <algorithm>
//namespace qm {

template <class Vector, class Position, class... Datum>
void fill_vector(Vector&,Position& ,vec<>, const Datum&...  )
{}

template <class Vector, class Position, class... X1>
void fill_vector(Vector&,Position& ,vec<X1...>)
{}


template <class Vector, class X, class... X1, class Position,class Datum0, class... Datum1>
void fill_vector(Vector&v,Position& p,vec<X,X1...> ,const Datum0& one,const Datum1&... two)
{
  if constexpr (is_in_pack(X{},typename Datum0::myIndexes{}))
  {
    auto n=one.size(X{},p);
    v.resize(n);
    auto& i=p[X{}]();
    for (i=0; i<n; ++i)
      fill_vector(v[i],p,vec<X1...>{},one,two...);
  }
  else fill_vector(v,p,vec<X,X1...>{},two...);
}



template<class Id, class Value_type,class... myIndex,class Id0, class Value_type0,class... X0, class...Datas, template<class...> class Datum>
auto consolidate(vec<myIndex...>,const Datum<Id0,Value_type0,vec<X0...>>& one,const Datas...d)
{
  typedef decltype ((vec<X0...>{}<<...<<typename Datas::myIndexes{})<<vec<myIndex...>{}) myvec;


  typedef Datum<Id,Value_type,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out{};
  auto p=myDatum::begin(myIndex{}...);

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
}



 template<class Id, class, class...> class Datum;

 template<class Id, class Value_type>
 struct Datum<Id,Value_type,vec<>>
  {
 public:
   //typedef typename Id::T T;
   //typedef typename Id::unit unit;

   typedef Id myId;

   typedef Value_type element_type;
   typedef  Value_type value_type;

   typedef vec<> myIndexes;

   constexpr static  auto numIndex=0;


 protected:
  Datum const& get(Id)const & {return *this;}
  Datum& get(Id) & {return *this;}
  Datum get(Id)&& {return *this;}
private:
  value_type value_;
public:
  Datum(value_type&& x):value_{std::move(x)}{}
  Datum(Id,value_type&& x):value_{std::move(x)}{}
  Datum()=default;
  Datum const& operator[](Id)const & {return *this;}
  Datum& operator[](Id) & {return *this;}
  Datum operator[](Id)&& {return *this;}


  static auto begin() {return Position<>{};}



  bool next(Position<>& p)const
  {
    return false;
  }
  template<class Position>
  auto& operator()(const Position& p){ return value_;}

  template<class Position>
  auto& operator()(const Position& p)const{ return value_;}

  auto& operator()()const{ return value_;}
  auto& operator()(){ return value_;}

  auto& value()const& {return value_;}
  auto& value()& {return value_;}
  auto value()&& {return value_;}

  friend std::ostream& operator<<(std::ostream& os, const Datum me)
  {
    return os<<me.value();
  }

};



template<class Id, class Value_type>
Datum(Id,Value_type&&)->Datum<Id,Value_type,vec<>>;

template<class Id, class Value_type,class X,class... Xs> class Datum<Id,Value_type,vec<X,Xs...>>
{
public:
  typedef typename Id::T T;

  typedef typename Id::unit unit;
  typedef   Id myId;

  typedef Value_type element_type;

  typedef typename vec<X,Xs...>::template to_vector<Value_type> value_type;

  typedef vec<X,Xs...> myIndexes;
  constexpr static  auto numIndex=sizeof... (Xs)+1;

protected:
  Datum const& get(Id)const & {return *this;}
  Datum& get(Id) & {return *this;}
  Datum get(Id)&& {return *this;}
private:
   value_type value_;



public:
  Datum(value_type&& x):value_{std::move(x)}{}
  Datum(Id,value_type&& x):value_{std::move(x)}{}
  Datum& operator[](Id)& {return *this;}

 Datum const& operator[](Id)const & {return *this;}
  Datum operator[](Id)&& {return *this;}


  template<class Position>
  auto& operator()(const Position& p){ return vec<X,Xs...>::get(value_,p);}

  template<class Position>
  auto& operator()(const Position& p)const { return vec<X,Xs...>::get(value_,p);}


  template<class I,class Position>
  auto& operator()(I i,const Position& p){ return vec<X,Xs...>::get_I(i,value_,p);}

  template<class I,class Position>
  auto& operator()(I i,const Position& p)const { return vec<X,Xs...>::get_I(i,value_,p);}


  template<class I,class Position>
  auto size(I,const Position& p)const->std::enable_if_t<is_in_pack(I{},Cs<X,Xs...>{}),std::size_t>
  {
    return vec<X,Xs...>::size(I{},value_,p);
  }

  auto& value() {return value_;}
  auto& value()const {return value_;}

  static auto begin() {return Position<X,Xs...>{};}

  static auto begin(X){return Position<>{};}


  template<class I>
  static auto begin(I)->std::enable_if_t<is_in_pack(I{},Cs<X,Xs...>{}),pack_until_this_t<I,Position<X,Xs...>>>

  {return pack_until_this_t<I,Position<X,Xs...>>{};}


  template<class... Ts>
  auto next(Position<Ts...>& p)const->std::enable_if_t<is_contained_in(Cs<Ts...>{},Cs<X,Xs...>{}),bool>
  {
    return vec<Ts...>::next(value_,p);
  }




  bool next(Position<X,Xs...>& p)const
  {
    return vec<X,Xs...>::next(value_,p);
  }



  friend std::ostream& operator<<(std::ostream& os, const Datum& me)
  {
    for (auto& x:me.value())
       os<<x<<" ";
    return os;
  }


};

template<class Id>
Datum(Id,v<typename Id::T,typename Id::unit>&&)->Datum<Id,v<typename Id::T,typename Id::unit>,vec<>>;






template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>>& operator+=(Datum<Id,Value_type,vec<Xs...>> &one,const Datum<Id,Value_type,vec<Xs...>>& two )
{
  auto pos=one.begin();
  one(pos)+=  two(pos);
  while (one.next(pos)) one(pos)+=two(pos);
  return one;
}
template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>>& operator-=(Datum<Id,Value_type,vec<Xs...>> &one,const Datum<Id,Value_type,vec<Xs...>>& two )
{
  auto pos=one.begin();
  one(pos)= one(pos)-  two(pos);
  while (one.next(pos)) one(pos)=one(pos)-two(pos);
  return one;

}

template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>> operator-(Datum<Id,Value_type,vec<Xs...>> one)
{
  auto pos=one.begin();
  one(pos)= - one(pos);
  while (one.next(pos)) one(pos)=-one(pos);
  return one;

}



template<class Id, class Value_type,class... Xs, class T>
Datum<Id,Value_type,vec<Xs...>>& operator/=(Datum<Id,Value_type,vec<Xs...>> &one,const v<T,dimension_less>& two )
{
  auto pos=one.begin();
  one(pos)/=  two;
  while (one.next(pos)) one(pos)/=two;
  return one;

}

template<class Id, class Value_type,class... Xs, class T, class unit, class Res_type=decltype (std::declval<Value_type>()/std::declval<v<T,unit>>())>
Datum<Id,Res_type,vec<Xs...>> operator/(const Datum<Id,Value_type,vec<Xs...>> &one,const v<T,unit>& two )
{
  Datum<Id,Res_type,vec<Xs...>> out;
  copy_size(one.value(),out.value());
  auto pos=one.begin();
  out(pos)= one(pos)/ two;
  while (one.next(pos)) out(pos)=one(pos)/two;
  return out;

}


template<class Id, class Value_type,class... Xs, class T, class unit, class Res_type=decltype (std::declval<Value_type>()*std::declval<v<T,unit>>())>
Datum<Id,Res_type,vec<Xs...>> operator*(const Datum<Id,Value_type,vec<Xs...>> &one,const v<T,unit>& two )
{
  Datum<Id,Res_type,vec<Xs...>> out;
  copy_size(one.value(),out.value());
  auto pos=one.begin();
  out(pos)= one(pos)* two;
  while (one.next(pos)) out(pos)=one(pos)*two;
  return out;

}


template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>> operator*(const Datum<Id,Value_type,vec<Xs...>> &one,double two )
{
  auto out=one;
  auto pos=one.begin();
  out(pos)= one(pos)* two;
  while (one.next(pos)) out(pos)=one(pos)*two;
  return out;
}


template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>> operator*( Datum<Id,Value_type,vec<Xs...>> &&one,double two )
{
  auto pos=one.begin();
  one(pos)= one(pos)* two;
  while (one.next(pos)) one(pos)=one(pos)*two;
  return one;
}

template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>> operator*(double two,const Datum<Id,Value_type,vec<Xs...>> &one )
{
  return one*two;
}

template<class Id, class Value_type,class... Xs>
Datum<Id,Value_type,vec<Xs...>> operator*(double two, Datum<Id,Value_type,vec<Xs...>> &&one )
{
  return std::move(one)*two;
}




template<class Id, class Value_type,class ...Ind,class Datas, class Rand>
  Datum<Id,Value_type,vec<Ind...>> sample(const Datum<Id,Value_type,vec<Ind...>>& d, const Datas& , Rand& ){return d;}

template<class Id, class Value_type,class ...Ind,class... Datas>
Datum<Id,Value_type,vec<Ind...>> calculate(const Datum<Id,Value_type,vec<Ind...>>& d, const Datas& ...){return d;}



template<class Id, class Value_type,class ...Ind,class... Datas>
auto logP(const Datum<Id,Value_type,vec<Ind...>>& , const Datas&... ){return logP_zero{};}

template<class Id, class Value_type,class ...Ind,class... Datas>
auto FIM(const Datum<Id,Value_type,vec<Ind...>>& , const Datas&... ){return logP_zero{};}




template<class Id,class... Ds> struct Data: public Ds...
{
  typedef Cs<Id,typename Ds::myId...> myIds;
/**/
//  using Ds::operator[]...;
/**/
protected:
  using Ds::get...;
protected:
  auto& get(Id)const & {return *this;}
  auto& get(Id) & {return *this;}
  auto get(Id) && {return *this;}
  template <class I> auto get(I) const {return Nothing{};}

public:
  typedef Id myId;


  auto& operator[](Id)const {return *this;}

  template<class I>
  auto& at(I)const {
    static_assert (is_in_pack(I{},myIds{}),"identifier is not in Data" );
    return get(I{});
  }

  template<class I>
  auto operator[](I) &->std::enable_if_t<!(std::is_same_v<Nothing,decltype(get(I{}))>),decltype (std::declval<Data&>().get(I{}))>
  {
    return get(I{});

  }

  template<class I>
  auto operator[](I) const &->std::conditional_t<!(std::is_same_v<Nothing,decltype(get(I{}))>),decltype (std::declval<Data const&>().get(I{})),Nothing>
 // auto operator[](I) const &->std::conditional_t<is_in_pack(I{},myIds{}),decltype (std::declval<Data const &>().get(I{})),Nothing>
  {
    if constexpr(is_in_pack(I{},myIds{}) )
      return get(I{});
    else return Nothing{};

  }
  template<class I>
  auto operator[](I) &&->std::conditional_t<!(std::is_same_v<Nothing,decltype(get(I{}))>),decltype (std::declval<Data&&>().get(I{})),Nothing>
//  auto operator[](I) &&->std::conditional_t<is_in_pack(I{},myIds{}),decltype (std::declval<Data &&>().get(I{})),Nothing>
  {
    if constexpr(is_in_pack(I{},myIds{}) )
      return get(I{});
    else return Nothing{};

  }
  //Data(Ds&&...d):Ds{std::move(d)}...{}
  Data(Id,Ds&&...d):Ds{std::move(d)}...{}

//  Data(Data<Id,Ds...>&& x):Ds{std::move(x)[typename Ds::myId{}]}...{}

  Data()=default;
  friend std::ostream& operator<<(std::ostream& os, const Data& me)
  {
    ((os<<typename Ds::myId{}<<"="<<me[typename Ds::myId{}]<<"  ")&&...);
    return os;
  }
};

template<class Id,class... Ds>
Data(Id,Ds&&...)->Data<Id,Ds...>;







template<class Id1,bool Complete,class... Ds, class Id2, class Value_type,class... vecs>
Is_Complete<Complete,Data<Id1,Ds...,Datum<Id2,Value_type,vecs...>>> operator |
    (Is_Complete<Complete,Data<Id1,Ds...>>&& d, Datum<Id2,Value_type,vecs...>&& d2)
{
  return Is_Complete(std::bool_constant<Complete>{},
                     Data<Id1,Ds...,Datum<Id2,Value_type,vecs...>>(Id1{},std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}


template<class Id,bool Complete,class... Ds>
Is_Complete<false,Data<Id,Ds...>> operator |(Is_Complete<Complete,Data<Id,Ds...>>&& d, Nothing)
{
  return Is_Complete(std::bool_constant<false>{},std::move(d.value));
}

/*
template<class Id,bool Complete,bool Complete2,class... Ds, class Id2, class ...Ds2>
Is_Complete<Complete&&Complete2,Data<Id,Ds...,Data<Id2,Ds2...>>>
operator |(Is_Complete<Complete,Data<Id,Ds...>>&& d, Is_Complete<Complete2,Data<Id2,Ds2...>>&& d2)
{
  return
      Is_Complete<Complete&Complete2,Data<Id,Ds...,Data<Id2,Ds2...>>>
      (std::move(d)(typename Ds::myIds{})...,std::move(d2));
}
*/


template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
 auto operator+(Data<Id,Ds1...>&& one, Datum<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<is_in_pack<Datum<Id2,Value_type,Xs...>,Ds1...>(),Data<Id,Ds1...>>
{
  one[Id2{}]+=two;
  return one;
}

template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator+(Data<Id,Ds1...>&& one, Datum<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<!is_in_pack<Datum<Id2,Value_type,Xs...>,Ds1...>(),Data<Id,Ds1...,Datum<Id2,Value_type,Xs...>>>
{
  return Data(Id{},std::move(one)[typename Ds1::myId{}]...,std::move(two));
}

template<class Id,class... Ds1,  class... Ds2>
auto operator+(Data<Id,Ds1...>&& one, Data<Id,Ds2...>&& two)
{
  return (std::move(one)+...+std::move(two)[typename Ds2::myId{}]);
}

template<class Id,class... Ds1>
auto operator-(Data<Id,Ds1...> one)
{
  ((one[typename Ds1::myId{}] = -one[typename Ds1::myId{}]),...);
  return one;
}


template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator-(Data<Id,Ds1...>&& one, Datum<Id2,Value_type,Xs...> two)
    ->std::enable_if_t<is_in_pack<Datum<Id2,Value_type,Xs...>,Ds1...>(),Data<Id,Ds1...>>
{
  one[Id2{}]-=two;
  return one;
}

template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator-(Data<Id,Ds1...>&& one, Datum<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<!is_in_pack<Datum<Id2,Value_type,Xs...>,Ds1...>(),Data<Id,Ds1...,Datum<Id2,Value_type,Xs...>>>
{
  return Data(Id{},std::move(one)[typename Ds1::myId{}]...,-std::move(two));
}

template<class Id,class... Ds1,  class... Ds2>
auto operator-(Data<Id,Ds1...>&& one, Data<Id,Ds2...>&& two)
{
  return (std::move(one)-...-std::move(two)[typename Ds2::myId{}]);
}


template<class Id,class... Ds,  class T, class unit>
auto operator/(Data<Id,Ds...> one, const v<T, unit>& two)
{
  return Data(Id{},std::move(one)[typename Ds::myId{}]/two...);

}

template<class Id,class... Ds1,  class... Ds2>
auto operator*(const Data<Id,Ds1...>& one, const Data<Id,Ds2...>& two)
{
  return ((one*two[typename Ds2::myId{}])+...);
}


template<class T, class unit,class Id, class... Ds>
auto operator*(const Data<Id,Ds...>& one, const v<T, unit>& x)
{
  return Data(Id{},one[typename Ds::myId{}]*x...);

}
template<class Id,class... Ds,  class T, class unit>
auto operator*( const v<T, unit>& two,Data<Id,Ds...> one)
{return one*two;}

template<class Id, class... Ds>
auto operator*(const Data<Id,Ds...>& one, double x)
{
  return Data(Id{},one[typename Ds::myId{}]*x...);

}
template<class Id,class... Ds>
auto operator*( double two,Data<Id,Ds...> one)
{return one*two;}



//} // namespace qm

#endif // DATA_H
