#ifndef DATA_H
#define DATA_H
#include "qm_unit.h"
#include <map>
#include <vector>
namespace qm {


template<template<class...> class Tr,class Id> class Datum_
{
public:
  typedef typename Tr<Id>::type::T T;

  typedef typename Tr<Id>::type::unit unit;

private:
  v<T,unit> value_;
public:
  Datum_(v<T,unit>&& x):value_{std::move(x)}{}
  auto& operator()(Id)const & {return value_;}
  v<T,unit> operator()(Id)&& {return std::move(value_);}

  friend std::ostream& operator<<(std::ostream& os, const Datum_ me)
  {
    return os<<me(Id{});
  }

};


template <template<class...>class, class...> class Data_;

template<template<class...> class Tr,class... Ids> class Data_
{
public:
  template<class Id>
  using T_t=typename Tr<Id>::type::T;

  template<class Id>
  using v_t=v<typename Tr<Id>::type::T,typename Tr<Id>::type::unit>;

private:
  std::tuple<Datum_<Tr,Ids>...> tu_;
public:
  Data_(v_t<Ids>&&... x):tu_{std::move(x)...}{}
  template<class Id>
  auto& operator()(Id)const & {return std::get<Datum_<Tr,Id>>(tu_);}
  template<class Id>
  Datum_<Tr,Id> operator()(Id)&& {return std::move(std::get<Datum_<Tr,Id>>(tu_));}

  friend std::ostream& operator<<(std::ostream& os, const Data_ me)
  {
    ((os<<Ids{}<<"="<<me(Ids{})<<"  ")&&...);
    return os;
  }

};

template<class... Ids>
using Data=Data_<C,Ids...>;


template<template<class...> class Tr,class... Ids, class Id>
auto& value(const Data_<Tr,Ids...>& d,Id id)
{
  return d(id)(id);
}

template<template<class...> class Tr,class... Ids, class Id>
auto& at(const Data_<Tr,Ids...>& d,Id id)
{
  return d(id);
}


template<class Id> struct Index{
  typedef std::size_t T;
  typedef dimension_less unit;
};


template<class Id> class EvenCoordinate
{
public:
  typedef typename Id::T T;
  typedef typename Id::unit unit;
private:
  T start_;
  T step_;
  T end_;
  std::size_t nsteps_;
  static T get_end(T start,T step,std::size_t nsteps){ return start+nsteps*step;}
  static T get_step(T start, T end, std::size_t nsteps) { return (end-start)/nsteps;}
public:
  EvenCoordinate(v<T,unit> start, v<T,unit> end, std::size_t nsteps)
      :start_{start.value()},end_{end.value()},nsteps_{nsteps},step_{get_step(start.value(),end.value(),nsteps)} {}
  v<T,unit> start()const {return start_;}
  v<T,unit> step()const {return step_;}
  v<T,unit> end()const {return end_;}

  auto nsteps()const {return nsteps_;}
  v<T,unit> operator()(std::size_t i)const {return start_+i*step_;}

  std::size_t index(const v<T,unit>& x)
  {
    return (x.value()-start_)/step_;
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











} // namespace qm

#endif // DATA_H
