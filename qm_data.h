#ifndef DATA_H
#define DATA_H
#include "qm_unit.h"
#include <map>
#include <vector>
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
    return vec<I...>::get(x[p[I0{}]()],p);
  }
  template <class Vector, class Positionx>
  static auto& get(const Vector& x,const Positionx& p)
  {
    return vec<I...>::get(x[p[I0{}]()],p);
  }

  template <class Vector, class Position>
  static auto& get_I(I0,const Vector& x,const Position& p)
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i,const Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x[p[I0{}]()],p);
  }



  template <class Vector, class Position>
  static auto& get_I(I0,Vector& x,const Position& p)
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i, Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x[p[I0{}]()],p);
  }





  template <class Vector, class Position>
  static auto size(I0,const Vector& x,const Position& p)
  {
    return x.size();
  }

  template <class In,class Vector, class Position>
  static auto size(In i,const Vector& x,const Position& p)->std::enable_if_t<is_in_pack<In,I...>(),std::size_t>
  {
    return vec<I...>::size(i,x[p[I0{}]()],p);
  }


  template <class Vector, class Position>
  static bool next(const Vector& x,Position& p)
  {
    if(vec<I...>::next(x[p[I0{}]()],p))
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


 template<template<class...> class Tr,class Id, class...> class Datum_;

 template<template<class...> class Tr,class Id>
 struct Datum_<Tr,Id,vec<>>
  {
 public:
   typedef typename Tr<Id>::type::T T;
   typedef typename Tr<Id>::type::unit unit;

   typedef Id myId;
   typedef  v<T,unit> value_type;

   typedef vec<> myIndexes;

   constexpr static  auto numIndex=0;

private:
  v<T,unit> value_;
public:
  Datum_(v<T,unit>&& x):value_{std::move(x)}{}
  Datum_()=default;
  auto& operator[](Id)const & {return *this;}
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

  auto& value()const {return value_;}

  friend std::ostream& operator<<(std::ostream& os, const Datum_ me)
  {
    return os<<me.value();
  }

};


template<template<class...> class Tr,class Id, class X,class... Xs> class Datum_<Tr,Id,vec<X,Xs...>>
{
public:
  typedef typename Tr<Id>::type::T T;

  typedef typename Tr<Id>::type::unit unit;
  typedef   Id myId;

  typedef typename vec<X,Xs...>::template to_vector<v<T,unit>> value_type;

  typedef vec<X,Xs...> myIndexes;
  constexpr static  auto numIndex=sizeof... (Xs)+1;

private:
   value_type value_;



public:
  Datum_(value_type&& x):value_{std::move(x)}{}
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


template<class Id,template<class...> class Tr,class Id0, class...Datas>
auto consolidate(Id,const Datum_<Tr,Id0>& one,const Datas...d)
{
  typedef decltype ((vec<>{}<<...<<typename Datas::myIndexes{})) myvec;
  typedef Datum_<Tr,Id,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out;
  auto p=myDatum::begin();

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
}


template<class Id,class... myIndex,template<class...> class Tr,class Id0, class... X0, class...Datas>
auto consolidate(Id,vec<myIndex...>,const Datum_<Tr,Id0,vec<X0...>>& one,const Datas...d)
{
  typedef decltype ((vec<X0...>{}<<...<<typename Datas::myIndexes{})<<vec<myIndex...>{}) myvec;

  typedef Datum_<Tr,Id,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out{};
  auto p=myDatum::begin(myIndex{}...);

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
  }



template<template<class...> class Tr,class Id, class ...Ind,class Datas, class Rand>
auto sample(const Datum_<Tr,Id,vec<Ind...>>& d, const Datas& , Rand& ){return d;}

struct logP_zero{
  constexpr static auto  className=my_static_string("logP_is_zero");
};

template<class T>
auto operator+(T&& x,logP_zero){return std::forward<T>(x);}
template<class T>
auto operator+( logP_zero,T&& x){return std::forward<T>(x);}
inline auto operator+(logP_zero ,logP_zero ){return logP_zero {};}

template<template<class...> class Tr,class Id, class ...Ind,class Datas>
auto logP(const Datum_<Tr,Id,vec<Ind...>>& d, const Datas& ){return logP_zero{};}



template<template<class...> class Tr,class Id,bool complete,class... Ds> struct Data_: public Ds...
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

  static constexpr bool is_complete=complete;

  template<class I>
  auto operator[](I)const ->std::enable_if_t<!is_in_pack(I{},myIds{}),Nothing>
  {return Nothing{};}

  Data_(Ds&&...d):Ds{std::move(d)}...{}

  Data_(Data_<Tr,Id,!complete,Ds...>&& x):Ds{std::move(x)[typename Ds::myId{}]}...{}

  friend std::ostream& operator<<(std::ostream& os, const Data_ me)
  {
    ((os<<typename Ds::myId{}<<"="<<me[typename Ds::myId{}]<<"  ")&&...);
    return os;
  }
};



template<template<class...> class Tr,class Id,bool Complete,class... Ds, class Id2, class... Id2s>
Data_<Tr,Id,Complete,Ds...,Datum_<Tr,Id2,Id2s...>> operator |(Data_<Tr,Id,Complete,Ds...>&& d, Datum_<Tr,Id2,Id2s...>&& d2)
{
  return Data_<Tr,Id,Complete,Ds...,Datum_<Tr,Id2,Id2s...>>(std::move(d)[typename Ds::myId{}]...,std::move(d2));
}


template<template<class...> class Tr,class Id,bool Complete,class... Ds>
Data_<Tr,Id,false,Ds...> operator |(Data_<Tr,Id,Complete,Ds...>&& d, Nothing)
{
  return Data_<Tr,Id,false,Ds...>(std::move(d));
}


template<template<class...> class Tr,class Id,bool Complete,bool Complete2,class... Ds, class Id2, class ...Ds2>
Data_<Tr,Id,Complete&Complete2,Ds...,Data_<Tr,Id2,Complete2,Ds2...>>
operator |(Data_<Tr,Id,Complete,Ds...>&& d, Data_<Tr,Id2,Complete2,Ds2...>&& d2)
{
  return
      Data_<Tr,Id,Complete&Complete2,Ds...,Data_<Tr,Id2,Complete2,Ds2...>>
      (std::move(d)(typename Ds::myIds{})...,std::move(d2));
}








template<class... Ids>
using Datum=Datum_<C,Ids...>;







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
    auto out=consolidate(Id{},vec<Id>{},par[Xs{}]...);
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
