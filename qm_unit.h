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
  friend std::istream& operator>>(std::istream& is, p<>&){
    return is;

  }

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


template<class... us, class m, int N>
constexpr auto operator| (Cs<p<us...>,p<>>, u<m,N>)
{
      return p<us...,u<m,N>>{};

}

template<class... us,class m0, int N0,class ...ms, int...Ns,  int N, typename =std::enable_if_t<N+N0!=0,int>>
constexpr auto operator| (Cs<p<us...>,p<u<m0,N0>,u<ms,Ns>...>>, u<m0,N>){
        return p<us...,u<m0,N+N0>,u<ms,Ns>...>{};
 }

 template<class... us,class m0, int N0,class ...ms, int...Ns>
 constexpr auto operator| (Cs<p<us...>,p<u<m0,N0>,u<ms,Ns>...>>, u<m0,-N0>){
   return p<us...,u<ms,Ns>...>{};
 }

 template<class... us,class m0, int N0,class ...ms, int...Ns, class m, int N, typename =std::enable_if_t<!std::is_same_v<m,m0 >,int>>
constexpr auto operator| (Cs<p<us...>,p<u<m0,N0>,u<ms,Ns>...>>, u<m,N>){

   if constexpr (m::className<m0::className)
       return p<us...,u<m,N>,u<m0,N0>,u<ms,Ns>...>{};
  else return Cs<p<us...,u<m0,N0>>,p<u<ms,Ns>...>>{}|u<m,N>{};
}


template<class ...ms, int...Ns, class m, int N>
constexpr auto operator* (p<u<ms,Ns>...>, u<m,N>){
  return Cs<p<>,p<u<ms,Ns>...>>{}|u<m,N>{};
}

template<class ...us>
using p_t=decltype ((p<>{}*...*us{}));




template<class ...ms, int...Ns, class... m2s, int... N2s>
constexpr auto operator* (p<u<ms,Ns>...>,p<u<m2s,N2s>...>){

  auto res= (p_t<u<ms,Ns>...>{}*...*u<m2s,N2s>{});
  return res;
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







//} // namespace qm


#endif // QM_UNIT_H
