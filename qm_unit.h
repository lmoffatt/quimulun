#ifndef QM_UNIT_H
#define QM_UNIT_H
#include "static_string.h"
#include "mytypetraits.h"
#include "qm_pack_operations.h"
#include "qm_basics.h"
#include <cstddef>
#include <tuple>
#include <cmath>
#include <vector>
#include <variant>
//namespace qm {








struct m{ constexpr static auto  className=my_static_string("m");};
struct s{constexpr static auto  className=my_static_string("s");};
struct kg{constexpr static auto  className=my_static_string("kg");};
struct A{constexpr static auto  className=my_static_string("A");};

template <class m, int N> struct u{
  constexpr static auto  className=m::className +my_static_string("^")+to_static_string<N>();
};

template <class m> struct u<m,1>{
  constexpr static auto  className=m::className ;
};


template<class ,class>
struct add_if_same_unit;

template <class m, int N, int N2>
struct add_if_same_unit<u<m,N>,u<m,N2>>{
  typedef u<m,N+N2> type ;
};

template <class m,class m2, int N, int N2>
struct add_if_same_unit<u<m,N>,u<m2,N2>>{
  typedef u<m,N> type ;
};

template<class ...>
struct substr_if_same_unit;

template <class m, int N, int N2>
struct substr_if_same_unit<u<m,N>,u<m,N2>>{
  typedef u<m,N-N2> type ;
};

template <class m,class m2, int N, int N2>
struct substr_if_same_unit<u<m,N>,u<m2,N2>>{
  typedef u<m,N> type ;
};



template<class ...>
struct wrap_if_not_unit;

template <class m>
struct wrap_if_not_unit<u<m,0>>{
  typedef std::tuple<> type ;
};

template <class m, int N>
struct wrap_if_not_unit<u<m,N>>{
  typedef std::tuple<u<m,N>> type ;
};



template<class ...> struct p;


template<> struct p<>
{
  constexpr static auto  className=my_static_string("") ;

  constexpr bool operator==(const p&)const {return true;}

  friend std::istream& operator>>(std::istream& is, p<>&){return is;}


};



template<class m, int N> struct p<u<m,N>>
{
  constexpr static auto  className=u<m,N>::className ;
  constexpr bool operator==(const p&)const {return true;}

};

template<class m, int N,class ...ms, int...Ns> struct p<u<m,N>,u<ms,Ns>...>
{
  constexpr static auto  className=(u<m,N>::className+(my_static_string(".")+...+u<ms,Ns>::className)) ;
  constexpr bool operator==(const p&)const {return true;}

};


template<class ...units>
class unit_system
{
public:
  template<class u>
  static constexpr std::size_t index= index_of_this_type(Cs<units...>{},C<u>{});
  typedef p<u<units,0>...> scalar_unit;


};
typedef unit_system<m,s,kg,A> my_unit_sytem;

typedef typename my_unit_sytem::scalar_unit my_scalar_unit;

template<class...>struct simplify;

template<class...T>
using simplify_t=typename simplify<T...>::type;


template<class...us>
struct simplify<p<us...>>
{
  typedef transfer_t<
      decltype(std::tuple_cat(std::declval<typename wrap_if_not_unit<us>::type>() ...)),
      p<>> type;
};


template<class, int>struct mult_exponent;

template<class T, int N>
using mult_exponent_t=typename mult_exponent<T, N>::type;

template<class...ms, int...Ns, int N>
struct mult_exponent<p<u<ms,Ns>...>, N>
{
  typedef p<u<ms,Ns*N>...> type;
};

template<class, int>struct div_exponent;

template<class T, int N>
using div_exponent_t=typename div_exponent<T,N>::type;

template<class...ms, int...Ns, int N>
struct div_exponent<p<u<ms,Ns>...>, N>
{
  static_assert (((Ns % N == 0)&&... &&true), "some exponent is not divisible");
  typedef p<u<ms,Ns/N>...> type;
};




template<class...>struct add_exponent;

template<class...T>
using add_exponent_t=typename add_exponent<T...>::type;


template<class...us, class u>
struct add_exponent<p<us...>, u>
{
  typedef p<
      typename add_if_same_unit<us,u>::type ...
      > type;
};


template<class...us,class u, class... us2>
struct add_exponent<p<us...>, u, us2...>
{
  typedef add_exponent_t<add_exponent_t<p<us...>,u>,us2...> type;
};


template<class...>struct substr_exponent;

template<class...T>
using substr_exponent_t=typename substr_exponent<T...>::type;


template<class...us, class u>
struct substr_exponent<p<us...>, u>
{
  typedef p<
      typename substr_if_same_unit<us,u>::type ...
      > type;
};


template<class...us,class u, class... us2>
struct substr_exponent<p<us...>, u, us2...>
{
  typedef substr_exponent_t<substr_exponent_t<p<us...>,u>,us2...> type;
};




template<class ...us>
using p_t=simplify_t<add_exponent_t<
    typename my_unit_sytem::scalar_unit,us...>>
    ;

typedef simplify_t<my_scalar_unit> dimension_less;



auto operator*(p<>,p<>)
{
  return  p<>{};
}

auto operator/(p<>,p<>)
{
  return  p<>{};
}


template<class... units1, class ...units2>
auto operator*(p<units1...>,p<units2...>)
{
  return  simplify_t<add_exponent_t<my_scalar_unit,units1...,units2...>>{};
}

template<class... units1, class ...units2>
constexpr auto operator/(p<units1...>,p<units2...>)
{ return  simplify_t<substr_exponent_t<add_exponent_t<my_scalar_unit,units1...>,units2...>>{};
}
template<class ...units2>
auto operator/(p<>,p<units2...>)
{ return  simplify_t<substr_exponent_t<my_scalar_unit,units2...>>{};
}

template<class ...units2>
auto operator/(p<units2...>,p<>)
{ return  p<units2...>{};
}










/*
template<class Id,class Id0, class Value_type,class...Datas, template<class...> class Datum>
auto consolidate(Id,const Datum<Id0, Value_type>& one,const Datas...d)
{
  typedef decltype ((vec<>{}<<...<<typename Datas::myIndexes{})) myvec;
  typedef Datum<Id,Value_type,myvec> myDatum;

  typedef typename myDatum::value_type myValue_type;

  myValue_type out;
  auto p=myDatum::begin();

  fill_vector(out,p,myvec{},one,d...);

  return myDatum(std::move(out));
}

*/




//} // namespace qm


#endif // QM_UNIT_H
