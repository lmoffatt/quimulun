#ifndef DATA_H
#define DATA_H
#include "qm_unit.h"
#include <map>
#include <vector>
#include <algorithm>
namespace qm {

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& me)
{
  os<<"  [";
  for (auto& x:me)
    os<<x<<" ";
  os<<"]   ";

  return os;
}


struct Nothing{};


inline Nothing operator+(Nothing, Nothing ){return Nothing{};}
template <class...> class Position;

template <class Id> struct Position<Id>
{
  std::size_t i;
  auto& operator[](Id)const {return *this;}
  auto& operator[](Id) {return *this;}
  auto operator()()const {return  i;}
};
template <class...Ids> struct Position: Position<Ids>...
{

  using Position<Ids>::operator[]...;
};




template<class...> struct vec;

template<> struct vec<>
{
  template <class T>
  using to_vector=T;

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
  static bool next(const Vector& x,Position& p) {return false;}

};
template<class I0, class...I> struct vec<I0,I...>{
  template <class T>
  using to_vector=std::vector<typename vec<I...>::template to_vector<T>>;

  template <class Vector, class Positionx>
  static auto& get(Vector& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }
  template <class Vector, class Positionx>
  static auto& get(const Vector& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }

  template <class Vector, class Position>
  static auto& get_I(I0,const Vector& x,const Position& p)
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i,const Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }



  template <class Vector, class Position>
  static auto& get_I(I0,Vector& x,const Position& p)
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i, Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }





  template <class Vector, class Position>
  static auto size(I0,const Vector& x,const Position& p)
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
      ++p[I0{}].i;
      if (p[I0{}].i<x.size())
        return true;
      else {
        p[I0{}].i=0; return false;
      }
    }
  }
};






template<class...I0, class...I1>
auto operator<<(const vec<I0...>&, const vec<I1...>&)
{

  return transfer_t<pack_difference_t<Cs<I1...>,Cs<I0...>>,vec<I0...>>{};


}



template <class Id> struct transpose {};

 /* template <class rowIndex, class colIndex=transpose<rowIndex>> struct matrix{
    template <class T>
    using v=Matrix<T>;

  };
  */

template<class T, class U>
void copy_size(const U& source,T& destination)
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

 template<class Id, class, class...> class Datum_;

 template<class Id, class Value_type>
 struct Datum_<Id,Value_type,vec<>>
  {
 public:
   //typedef typename Id::T T;
   //typedef typename Id::unit unit;

   typedef Id myId;

   typedef Value_type element_type;
   typedef  Value_type value_type;

   typedef vec<> myIndexes;

   constexpr static  auto numIndex=0;

private:
  value_type value_;
public:
  Datum_(value_type&& x):value_{std::move(x)}{}
  Datum_(Id,value_type&& x):value_{std::move(x)}{}
  Datum_()=default;
  auto& operator[](Id)const & {return *this;}
  auto& operator[](Id) & {return *this;}
  auto operator[](Id)&& {return *this;}
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

  friend std::ostream& operator<<(std::ostream& os, const Datum_ me)
  {
    return os<<me.value();
  }

};



template<class Id, class Value_type>
Datum_(Id,Value_type&&)->Datum_<Id,Value_type,vec<>>;

template<class Id, class Value_type,class X,class... Xs> class Datum_<Id,Value_type,vec<X,Xs...>>
{
public:
  typedef typename Id::T T;

  typedef typename Id::unit unit;
  typedef   Id myId;

  typedef Value_type element_type;

  typedef typename vec<X,Xs...>::template to_vector<Value_type> value_type;

  typedef vec<X,Xs...> myIndexes;
  constexpr static  auto numIndex=sizeof... (Xs)+1;

private:
   value_type value_;



public:
  Datum_(value_type&& x):value_{std::move(x)}{}
  Datum_(Id,value_type&& x):value_{std::move(x)}{}
  auto& operator[](Id)& {return *this;}

  auto& operator[](Id)const & {return *this;}
  auto&& operator[](Id)&& {return std::move(*this);}


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



  friend std::ostream& operator<<(std::ostream& os, const Datum_& me)
  {
    for (auto& x:me.value())
       os<<x<<" ";
    return os;
  }


};

template<class Id>
Datum_(Id,v<typename Id::T,typename Id::unit>&&)->Datum_<Id,v<typename Id::T,typename Id::unit>,vec<>>;






template<class Id, class Value_type,class... Xs>
Datum_<Id,Value_type,vec<Xs...>>& operator+=(Datum_<Id,Value_type,vec<Xs...>> &one,const Datum_<Id,Value_type,vec<Xs...>>& two )
{
  auto pos=one.begin();
  one(pos)+=  two(pos);
  while (one.next(pos)) one(pos)+=two(pos);
  return one;

}
template<class Id, class Value_type,class... Xs>
Datum_<Id,Value_type,vec<Xs...>>& operator-=(Datum_<Id,Value_type,vec<Xs...>> &one,const Datum_<Id,Value_type,vec<Xs...>>& two )
{
  auto pos=one.begin();
  one(pos)= one(pos)-  two(pos);
  while (one.next(pos)) one(pos)=one(pos)-two(pos);
  return one;

}

template<class Id, class Value_type,class... Xs>
Datum_<Id,Value_type,vec<Xs...>> operator-(Datum_<Id,Value_type,vec<Xs...>> one)
{
  auto pos=one.begin();
  one(pos)= - one(pos);
  while (one.next(pos)) one(pos)=-one(pos);
  return one;

}



template<class Id, class Value_type,class... Xs, class T>
Datum_<Id,Value_type,vec<Xs...>>& operator/=(Datum_<Id,Value_type,vec<Xs...>> &one,const v<T,dimension_less>& two )
{
  auto pos=one.begin();
  one(pos)/=  two;
  while (one.next(pos)) one(pos)/=two;
  return one;

}

template<class Id, class Value_type,class... Xs, class T, class unit, class Res_type=decltype (std::declval<Value_type>()/std::declval<v<T,unit>>())>
Datum_<Id,Res_type,vec<Xs...>> operator/(const Datum_<Id,Value_type,vec<Xs...>> &one,const v<T,unit>& two )
{
  Datum_<Id,Res_type,vec<Xs...>> out;
  copy_size(one.value(),out.value());
  auto pos=one.begin();
  out(pos)= one(pos)/ two;
  while (one.next(pos)) out(pos)=one(pos)/two;
  return out;

}


template<class Id, class Value_type,class... Xs, class T, class unit, class Res_type=decltype (std::declval<Value_type>()*std::declval<v<T,unit>>())>
Datum_<Id,Res_type,vec<Xs...>> operator*(const Datum_<Id,Value_type,vec<Xs...>> &one,const v<T,unit>& two )
{
  Datum_<Id,Res_type,vec<Xs...>> out;
  copy_size(one.value(),out.value());
  auto pos=one.begin();
  out(pos)= one(pos)* two;
  while (one.next(pos)) out(pos)=one(pos)*two;
  return out;

}






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
   auto& i=p[X{}].i;
   for (i=0; i<n; ++i)
     fill_vector(v[i],p,vec<X1...>{},one,two...);
   }
   else fill_vector(v,p,vec<X,X1...>{},two...);
}


template<class Id,class Id0, class Value_type,class...Datas>
auto consolidate(Id,const Datum_<Id0, Value_type>& one,const Datas...d)
{
  typedef decltype ((vec<>{}<<...<<typename Datas::myIndexes{})) myvec;
  typedef Datum_<Id,Value_type,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out;
  auto p=myDatum::begin();

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
}


template<class Id, class Value_type,class... myIndex,class Id0, class Value_type0,class... X0, class...Datas>
auto consolidate(vec<myIndex...>,const Datum_<Id0,Value_type0,vec<X0...>>& one,const Datas...d)
{
  typedef decltype ((vec<X0...>{}<<...<<typename Datas::myIndexes{})<<vec<myIndex...>{}) myvec;


  typedef Datum_<Id,Value_type,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out{};
  auto p=myDatum::begin(myIndex{}...);

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
}



template<class Id, class Value_type,class ...Ind,class Datas, class Rand>
  Datum_<Id,Value_type,vec<Ind...>> sample(const Datum_<Id,Value_type,vec<Ind...>>& d, const Datas& , Rand& ){return d;}

struct logP_zero{
  constexpr static auto  className=my_static_string("logP_is_zero");
};

template<class T>
auto operator+(T&& x,logP_zero){return std::forward<T>(x);}
template<class T>
auto operator+( logP_zero,T&& x){return std::forward<T>(x);}
inline auto operator+(logP_zero ,logP_zero ){return logP_zero {};}

template<class Id, class Value_type,class ...Ind,class Datas>
auto logP(const Datum_<Id,Value_type,vec<Ind...>>& d, const Datas& ){return logP_zero{};}



template<bool complete,class Data>
struct Is_Complete{
  Data value;
  typedef Data type;
  static constexpr bool is_complete=complete;
  Is_Complete(std::bool_constant<complete>,Data&& d):value{std::move(d)}{}
  Is_Complete()=default;

};

template<bool Complete, class Data> Is_Complete(std::bool_constant<Complete>,Data&&)->Is_Complete<Complete,Data>;


template<class Id,class... Ds> struct Data_: public Ds...
{
  using Ds::operator[]...;
  typedef Id myId;
  typedef Cs<Id,typename Ds::myId...> myIds;

  auto& operator[](Id)const {return *this;}

  template<class I>
  auto& at(I)const {
    static_assert (is_in_pack(I{},myIds{}),"identifier is not in Data" );
    return (*this)[I{}];
  }


  template<class I>
  auto operator[](I)const ->std::enable_if_t<!is_in_pack(I{},myIds{}),Nothing>
  {return Nothing{};}

  Data_(Ds&&...d):Ds{std::move(d)}...{}
  Data_(Id,Ds&&...d):Ds{std::move(d)}...{}

//  Data_(Data_<Id,Ds...>&& x):Ds{std::move(x)[typename Ds::myId{}]}...{}

  friend std::ostream& operator<<(std::ostream& os, const Data_& me)
  {
    ((os<<typename Ds::myId{}<<"="<<me[typename Ds::myId{}]<<"  ")&&...);
    return os;
  }
};

template<class Id,class... Ds>
Data_(Id,Ds&&...)->Data_<Id,Ds...>;





template<class Id,bool Complete,class... Ds, class Id2, class Value_type,class... vecs>
Is_Complete<Complete,Data_<Id,Ds...,Datum_<Id2,Value_type,vecs...>>> operator |
    (Is_Complete<Complete,Data_<Id,Ds...>>&& d, Datum_<Id2,Value_type,vecs...>&& d2)
{
  return Is_Complete(std::bool_constant<Complete>{},
                     Data_<Id,Ds...,Datum_<Id2,Value_type,vecs...>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}


template<class Id,bool Complete,class... Ds>
Is_Complete<false,Data_<Id,Ds...>> operator |(Is_Complete<Complete,Data_<Id,Ds...>>&& d, Nothing)
{
  return Is_Complete(std::bool_constant<false>{},std::move(d.value));
}

/*
template<class Id,bool Complete,bool Complete2,class... Ds, class Id2, class ...Ds2>
Is_Complete<Complete&&Complete2,Data_<Id,Ds...,Data_<Id2,Ds2...>>>
operator |(Is_Complete<Complete,Data_<Id,Ds...>>&& d, Is_Complete<Complete2,Data_<Id2,Ds2...>>&& d2)
{
  return
      Is_Complete<Complete&Complete2,Data_<Id,Ds...,Data_<Id2,Ds2...>>>
      (std::move(d)(typename Ds::myIds{})...,std::move(d2));
}
*/


template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
 auto operator+(Data_<Id,Ds1...>&& one, Datum_<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<is_in_pack<Datum_<Id2,Value_type,Xs...>,Ds1...>(),Data_<Id,Ds1...>>
{
  one[Id2{}]+=two;
  return one;
}

template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator+(Data_<Id,Ds1...>&& one, Datum_<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<!is_in_pack<Datum_<Id2,Value_type,Xs...>,Ds1...>(),Data_<Id,Ds1...,Datum_<Id2,Value_type,Xs...>>>
{
  return Data_(Id{},std::move(one)[typename Ds1::myId{}]...,std::move(two));
}

template<class Id,class... Ds1,  class... Ds2>
auto operator+(Data_<Id,Ds1...>&& one, Data_<Id,Ds2...>&& two)
{
  return (std::move(one)+...+std::move(two)[typename Ds2::myId{}]);
}

template<class Id,class... Ds1>
auto operator-(Data_<Id,Ds1...> one)
{
  ((one[typename Ds1::myId{}] = -one[typename Ds1::myId{}]),...);
  return one;
}


template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator-(Data_<Id,Ds1...>&& one, Datum_<Id2,Value_type,Xs...> two)
    ->std::enable_if_t<is_in_pack<Datum_<Id2,Value_type,Xs...>,Ds1...>(),Data_<Id,Ds1...>>
{
  one[Id2{}]-=two;
  return one;
}

template<class Id,class... Ds1, class Id2, class Value_type,class... Xs>
auto operator-(Data_<Id,Ds1...>&& one, Datum_<Id2,Value_type,Xs...>&& two)
    ->std::enable_if_t<!is_in_pack<Datum_<Id2,Value_type,Xs...>,Ds1...>(),Data_<Id,Ds1...,Datum_<Id2,Value_type,Xs...>>>
{
  return Data_(Id{},std::move(one)[typename Ds1::myId{}]...,-std::move(two));
}

template<class Id,class... Ds1,  class... Ds2>
auto operator-(Data_<Id,Ds1...>&& one, Data_<Id,Ds2...>&& two)
{
  return (std::move(one)-...-std::move(two)[typename Ds2::myId{}]);
}


template<class Id,class... Ds,  class T, class unit>
auto operator/(Data_<Id,Ds...> one, const v<T, unit>& two)
{
  return Data_(Id{},std::move(one)[typename Ds::myId{}]/two...);

}



template<class T, class unit,class Id, class... Ds>
auto operator*(const Data_<Id,Ds...>& one, const v<T, unit>& x)
{
  return Data_(Id{},one[typename Ds::myId{}]*x...);

}
template<class Id,class... Ds,  class T, class unit>
auto operator*( const v<T, unit>& two,Data_<Id,Ds...> one)
{return one*two;}



template<class Id> struct Start{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_start");

};
template<class Id> struct Duration{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_end");

};
template<class Id> struct Step{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_step");

};



 struct EvenCoordinate
{

  template<class T, class unit>
  auto operator()(v<T,unit> start, v<T,unit> duration, v<T,unit> step)const
  {
    auto n=duration/step;
    std::vector<v<T,unit>> out;
    out.reserve(n.value());
    for (auto x=start; x.value()<start.value()+duration.value(); x=x+step)
      out.push_back(x);

    return out;
  }


};



template<class Id> class UnEvenCoordinate
{
public:
  typedef typename Id::T T;
  typedef typename Id::unit unit;
private:private:

  std::map<T, std::size_t> m_;
  std::vector<T> v_;

  std::map<T, std::size_t> vector_to_map(const std::vector<T>& v)
  {
    std::map<T, std::size_t> out;
    for (std::size_t i=0; i<v.size(); ++i)
      out[v[i]]=i;
    return out;
  }
public:
  UnEvenCoordinate(v<std::vector<T>,unit>&& c): m_{vector_to_map(c)},v_{std::move(c).value()}{}
  v<T,unit> start()const {return v_[0];}
  v<T,unit> end()const {return v_.back();}

  auto nsteps()const {return v_.size();}
  v<T,unit> operator()(std::size_t i)const {return v_[i];}

  std::size_t index(const v<T,unit>& x)
  {
    auto it= m_.lower_bound(x);
    return it->second;
  }

};


template<class...> class Cartesian;

template<template<class >class...Coordinates, class...Ids>
class Cartesian<Coordinates<Ids>...>
{
private :
  std::tuple<Coordinates<Ids>...> tu_;
public :

};




template<class Id,class G, class... Xs>
class  Coord
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class Param>
  auto operator()(const Param& par)const
  {
    typedef typename decltype(g_(std::declval<typename std::decay_t<decltype(par[Xs{}])>::element_type>()...))::value_type value_type;

    //typedef typename value_type::sgser dgewr;

    auto out=consolidate<Id,value_type>(vec<Id>{},par[Xs{}]...);
    auto p=out.begin(Id{});

      do {
        out(Id{},p)=g_(par[Xs{}](p)...);

      } while (out.next(p));
      return out;
    }

  Coord(Id id,G&& g, Xs&&...):g_{std::move(g)}{}
};



template<class Id,class G, class... Xs,class Rnd, class Datas>
auto sample(const Coord<Id,G,Xs...>& f,const Datas& d,Rnd& )
{
  if constexpr ((std::is_same_v<decltype (d[Xs{}]),Nothing>||...))
    return Nothing{};
  else
    return f(d);
}

template<class Id,class G, class... Xs,class Datas>
auto logP(const Coord<Id,G,Xs...>& f,const Datas& d)
{
  return logP_zero{};
}



template<class Id, class G, class...Xs> Coord(Id&&,G&&,Xs&&...)->Coord<Id,G,Xs...>;








} // namespace qm

#endif // DATA_H
