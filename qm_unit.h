


#ifndef QM_UNIT_H
#define QM_UNIT_H

#include "static_string.h"
# include <tuple>
#include "qm_base.h"
#include <type_traits>




template <class m, int N> struct u;

template<class ...> struct p;
template <class m, int N> using u_t=typename u<m,N>::type;


template <class m, int N> struct u{
  constexpr static auto  name_N()
  {
    if constexpr (N==1) return m::name;
    else return m::name +my_static_string("^")+to_static_string<N>();
  }

  constexpr static auto  name=name_N();
  constexpr auto operator[](m)const {return *this;}
  constexpr auto n()const {return N;}
  using type=u<m,N>;

};
template<int N2,class m, int N>
constexpr auto pow(u<m,N>) {return u<m,N*N2>{};}

template<int N2,class m, int N>
constexpr auto pow_inv(u<m,N>) {
  if constexpr (N % N2==0)
  return u<m,N/N2>{};
  else return Nothing{};
}

template<class ...> struct p;
template<class... m1s, int... N1s,class... m2s, int... N2s>
 constexpr auto operator && (p<u<m1s,N1s>...>, p<u<m2s,N2s>...>)
{
  return p<u<m1s,N1s>...,u<m2s,N2s>...>{};
}


template<class ...ms, int...Ns> struct p<u<ms,Ns>...>:u<ms,Ns>...
{
private:


  template<class m, int N, class ...m1s, int...N1s>
  static constexpr auto name_impl(u<m,N>, u<m1s, N1s>...)
  {
    return (u<m,N>::name+...+(my_static_string(".")+u<m1s,N1s>::name));
  }

  static constexpr auto name_impl()
  {
    return my_static_string("");
  }

  template<class m2, int N2,class... m2s, int... N2s>
  static constexpr auto product_impl(p<> , p<u<m2,N2>,u<m2s,N2s>...>)
  {
    return p<u<m2,N2>,u<m2s,N2s>...>{};
  }
  template<class... m2s, int... N2s>
  static constexpr auto product_impl( p<u<m2s,N2s>...>,p<> )
  {
    return p<u<m2s,N2s>...>{};
  }

  template< class m1, int N1, class ...m1s, int...N1s, class m2, int N2, class... m2s, int... N2s>
  static constexpr auto product_impl(p<u<m1,N1>, u<m1s, N1s>...> , p<u<m2,N2>,u<m2s,N2s>...>)
  {
    if constexpr (std::is_same_v<m1,m2 >)
    {
      if constexpr (N1+N2==0)
        return product_impl( p<u<m1s, N1s>...>{} , p<u<m2s,N2s>...>{});
      else
        return p<u<m1,N1+N2>>{}&& product_impl( p<u<m1s, N1s>...>{} , p<u<m2s,N2s>...>{});
    }
    else if constexpr (m2::name<m1::name)
      return p<u<m2,N2>>{}&& product_impl(p< u<m1,N1>, u<m1s, N1s>...>{} , p<u<m2s,N2s>...>{});
    else
      return p<u<m1,N1>>{}&& product_impl(p<u<m1s,N1s>...>{},p<u<m2,N2>,u<m2s,N2s>...>{});

  }

public:

  using u<ms,Ns>::operator[]...;


  constexpr static auto  name=name_impl(u<ms,Ns>{}...) ;
  constexpr bool operator==(const p&)const {return true;}


  template<class... m2s, int... N2s>
  friend constexpr auto operator*(p , p<u<m2s,N2s>...>)
  {
    return product_impl(p{},p<u<m2s,N2s>...>{});
  }
  template<class... m2s, int... N2s>
  friend constexpr auto operator/(p , p<u<m2s,N2s>...>)
  {
    return product_impl(p{},p<u<m2s,-N2s>...>{});
  }

};


template<class m, int N>
constexpr auto remove_zero_power(p<u<m,N>>)
{
  if constexpr (N==0)
    return p<>{};
  else
    return p<u<m,N>>{};
}


template<class ...us>
using p_t=decltype ((p<>{}*...*p<us>{}));


typedef p<> dimension_less;
template<class T, class unit>
class nl_u
{
private:
  T n_;

  constexpr static auto name_impl=my_static_string("log(")+unit::name;
public:
  constexpr static auto name=name_impl+my_static_string(")");
  __host__ __device__ constexpr auto& operator[](unit)const {return *this;}
  __host__ __device__ constexpr auto& operator[](unit){return *this;}
  __host__ __device__ constexpr auto& operator()()const{ return n_;}
  __host__ __device__ constexpr auto& operator()(){ return n_;}
  __host__ __device__ constexpr nl_u(T n, unit):n_{n}{}

  __host__ __device__ constexpr nl_u()=default;

  template<class U>
  friend
      __host__ __device__ constexpr auto operator + (nl_u x, nl_u<U,unit> y)
  {
    using R=std::decay_t<decltype (x()+y())>;
    return nl_u<R,unit>(x()+y(),unit{});
  }

  friend
   __host__ __device__ void my_print(const  nl_u& me)
  {
    my_print(me());
    my_print(me.name.c_str());
  }

};

template<class unit>
struct l_u
{

  constexpr static auto name_impl=my_static_string("log(")+unit::name;
  constexpr static auto name=name_impl+my_static_string(")");
  __host__ __device__ constexpr auto operator[](unit)const {return *this;}
  __host__ __device__ constexpr auto operator[](unit){return *this;}



  friend __host__ __device__ constexpr auto exp(l_u) {return unit{};}

  friend __host__ __device__ constexpr auto operator+ (l_u,l_u)
  {
    return nl_u(2l,unit{});
  }

  template<class unit2>
  friend __host__ __device__ constexpr auto operator+ (l_u,l_u<unit2>)
  {
    return nl_u(1l,unit{})+nl_u(1l,unit2{});
  }

  friend __host__ __device__ constexpr auto operator- (l_u,l_u)
  {
    return dimension_less{};
  }



  friend   __host__ __device__ void my_print(l_u me) {    my_print(me.name.c_str());}


};

template<class ...units> __host__ __device__ constexpr auto log(p<units...>) {return l_u<p<units...>>{};}




template<class...>
class log_unit;

template<class... T0s, class... u0s, class ...T1s, class... u1s>
constexpr auto operator&&(log_unit<nl_u<T0s,u0s>...> one,log_unit<nl_u<T1s,u1s>...> two);

template<class... Ts, class... units>
class log_unit<nl_u<Ts,units>...>:public nl_u<Ts,units>...
{
  template<class T, class U, class ...T1s, class ...U1s>
  static constexpr auto name_impl(nl_u<T,U>, nl_u<T1s, T1s>...)
  {
    return (nl_u<T,U>::name+...+(my_static_string("+")+nl_u<T1s,T1s>::name));
  }

  static constexpr auto name_impl()
  {
    return my_static_string("");
  }

  template<class...TBs, class... UBs,class U2,class...U2s>
  static constexpr auto sum_impl(const log_unit& x,Cs<> , const log_unit<nl_u<TBs,UBs>...>& y,Cs<U2,U2s...>)
  {
    return log_unit<std::decay_t<decltype (x[U2{}])>,std::decay_t<decltype (x[U2s{}])>...>(x[U2{}],x[U2s{}]...);
  }

  template<class...TBs, class... UBs,class...U1s>
  static constexpr auto sum_impl(const log_unit& x,Cs<U1s...> , const log_unit<nl_u<TBs,UBs>...>& y,Cs<>)
  {
    return log_unit<std::decay_t<decltype (x[U1s{}])>...>(x[U1s{}]...);
  }

  template<class...TBs, class... UBs,class U1, class...U1s, class U2,  class... U2s>
  static constexpr auto sum_impl(const log_unit& x,Cs<U1,U1s...> , const log_unit<nl_u<TBs,UBs>...>& y,Cs<U2,U2s...>)
  {
    if constexpr (std::is_same_v<U1,U2 >)
    {
      using R=std::decay_t<decltype (x[U1{}]+y[U1{}])>;
      return log_unit<R>(x[U1{}]+y[U1{}])&& sum_impl( x,Cs<U1s...>{},y,Cs<U2s...>{});
    }
    else if constexpr (U2::name<U1::name)
    {
      using R=std::decay_t<decltype (y[U2{}])>;
      return log_unit<R>(y[U2{}])&& sum_impl( x,Cs<U1,U1s...>{},y,Cs<U2s...>{});
    }
    else
    {
      using R=std::decay_t<decltype (x[U1{}])>;
      return log_unit<R>(x[U1{}])&& sum_impl( x,Cs<U1s...>{},y,Cs<U2,U2s...>{});
    }
  }


public:
  using nl_u<Ts,units>::operator[]...;
  log_unit(nl_u<Ts,units> ...lu):nl_u<Ts,units>{lu}...{}

  template<class...T2s, class... u2s>
  friend constexpr auto operator+(log_unit one, log_unit<nl_u<T2s,u2s>...> two)
  {

    return sum_impl(one,Cs<units...>{},two,Cs<u2s...>{});

  }
};

template<class T1, class unit1, class T2,class unit2>
__host__ __device__ constexpr auto operator + (nl_u<T1,unit1> x, nl_u<T2,unit2> y)
{
  return log_unit<nl_u<T1,unit1>>(x)+log_unit<nl_u<T2,unit2>>(y);
}

template<class... T0s, class... u0s, class ...T1s, class... u1s>
constexpr auto operator&&(log_unit<nl_u<T0s,u0s>...> one,log_unit<nl_u<T1s,u1s>...> two)
{
  return log_unit(one[u0s{}]...,two[u1s{}]...);
}









//} // namespace qm



#endif // QM_UNIT_H
