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

template <class m, int N> struct u;
template<class ...> struct p;
template <class m, int N>
using u_t=typename u<m,N>::type;


template <class m, int N> struct u{
  constexpr static auto  className=m::className +my_static_string("^")+to_static_string<N>();
  constexpr auto operator[](m)const {return *this;}
  constexpr auto n()const {return N;}
  using magnitude=m;

  using type=u<m,N>;

  template<int N2>
  friend constexpr u_t<m,N+N2> operator*(u<m,N>,u<m,N2>) {return {};};

  template<class m2, int N2>
  friend constexpr u<m,N> operator*(u<m,N>,u<m2,N2>) {return {};};
};
template<int N2,class m, int N>
constexpr u<m,N*N2> pow(u<m,N>) {return {};}

template<int N2,class m, int N, typename=std::enable_if_t<N%N2==0,int>>
constexpr u<m,N/N2> pow_inv(u<m,N>) {return {};}


template <class m> struct u<m,0>{using type=p<>;};

template <class m> struct u<m,1>{
  constexpr static auto  className=m::className ;
  constexpr auto operator[](m)const {return *this;}
  constexpr auto n()const {return 1;}
  using type=u<m,1>;

  using magnitude=m;
  template<int N2>
  friend constexpr u_t<m,1+N2> operator*(u<m,1>,u<m,N2>) {return {};};

  template<class m2,int N2>
  friend constexpr u<m,1> operator*(u<m,1>,u<m2,N2>) {return {};};


};



template<class ...> struct p;


template<> struct p<>
{
  constexpr static auto  className=my_static_string("") ;
  constexpr bool operator==(const p&)const {return true;}
  friend std::istream& operator>>(std::istream& is, p<>&){return is;}
};

template<class m, int N> struct p<u<m,N>>:u<m,N>
{
  using u<m,N>::operator[];
  constexpr static auto  className=u<m,N>::className ;
  constexpr bool operator==(const p&)const {return true;}
  constexpr p()=default;
  constexpr p(u<m,N>){}
};

template<class m, int N,class ...ms, int...Ns> struct p<u<m,N>,u<ms,Ns>...>:u<m,N>,u<ms,Ns>...
{
  using u<m,N>::operator[];
  using u<ms,Ns>::operator[]...;
  constexpr static auto  className=(u<m,N>::className+(my_static_string(".")+...+u<ms,Ns>::className)) ;
  constexpr bool operator==(const p&)const {return true;}

  constexpr p()=default;
  constexpr p(u<m,N>,u<ms,Ns>...){}

};
template<class ...ms, int...Ns, class m, int N>
p<u<ms,Ns>...,u<m,N>> operator| (p<u<ms,Ns>...>, u<m,N>){return{};}

template<class ...ms, int...Ns>
p<u<ms,Ns>...> operator| (p<u<ms,Ns>...>, p<>){return{};}



template<class ...ms, int...Ns, class m, int N, typename =std::enable_if_t<!is_in_pack<m,ms...>(),int>>
p<u<ms,Ns>...,u<m,N>> operator* (p<u<ms,Ns>...>, u<m,N>){return{};}

template<class ...ms, int...Ns, class m, int N, typename =std::enable_if_t<is_in_pack<m,ms...>(),int>>
auto operator* (p<u<ms,Ns>...>, u<m,N>){return (p<>{}|...|(u<ms,Ns>{}*u<m,N>{}));}

template<class ...ms, int...Ns, class m, int N, typename =std::enable_if_t<!is_in_pack<m,ms...>(),int>>
p<u<ms,Ns>...,u<m,-N>> operator/ (p<u<ms,Ns>...>, u<m,N>){return{};}

template<class ...ms, int...Ns, class m, int N, typename =std::enable_if_t<is_in_pack<m,ms...>(),int>>
auto operator/ (p<u<ms,Ns>...>, u<m,N>){return (p<>{}|...|(u<ms,Ns>{}*u<m,-N>{}));}

template<class ...ms, int...Ns, class... m2s, int... N2s>
auto operator* (p<u<ms,Ns>...> p1,p<u<m2s,N2s>...>){
  return (p1*...*u<m2s,N2s>{});

}

template<class ...ms, int...Ns, class... m2s, int... N2s>
auto operator/ (p<u<ms,Ns>...> p1,p<u<m2s,N2s>...>){
  return (p1*...*u<m2s,-N2s>{});

}



typedef p<> dimension_less;



auto operator*(p<>,p<>)
{
  return  p<>{};
}

auto operator/(p<>,p<>)
{
  return  p<>{};
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
