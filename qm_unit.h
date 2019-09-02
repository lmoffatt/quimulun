#ifndef QM_UNIT_H
#define QM_UNIT_H


#include "mytypetraits.h"
#include <cstddef>
#include <tuple>
#include <cmath>
#include <vector>

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

};



template<class m, int N> struct p<u<m,N>>
{
  constexpr static auto  className=u<m,N>::className ;

};

template<class m, int N,class ...ms, int...Ns> struct p<u<m,N>,u<ms,Ns>...>
{
  constexpr static auto  className=(u<m,N>::className+(my_static_string(".")+...+u<ms,Ns>::className)) ;

};

template<class ...ms, int...Ns>
std::ostream& operator<<(std::ostream& os,p<u<ms,Ns>...> )
{
  return os<<p<u<ms,Ns>...>::className.c_str();
}








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




template<class ...> struct v;
template<class ...> struct logv;





template<class TYPE,class myunit> class v<TYPE,myunit>
{
  TYPE value_;
public:
 typedef TYPE T;
 typedef myunit unit;

  v(TYPE&& x,myunit): value_{std::move(x)}{}
  v(TYPE&& x): value_{std::move(x)}{}
  v()=default;
  const TYPE& value()const &{return value_;}
  TYPE value()&& {return value_;}
  friend std::ostream& operator<<(std::ostream& os, const v& m)
  {
    return os<<m.value()<<" "<<myunit{};
  }
  v& operator+=(const v& other){value_+=other.value(); return *this;}

};

template<class...us>
class logv_units
{
private:
  std::tuple<std::pair<long,us>...> units;
public:
  auto& operator()(){return units;}
  auto const& operator()()const {return units;}

  logv_units()=default;
  logv_units(std::pair<long,us>... myunits):units{std::move(myunits)...}{}
  template<class u>
  long get_number()const
  {
    if constexpr (is_in_pack<u,us...>()) return std::get<std::pair<long,u>>(units).first;
    else return 0;
  }
  template<class u>
  long& get(){return std::get<std::pair<long,u>>(units).first;}
  friend std::ostream& operator<<(std::ostream& os, const logv_units& m)
  {
    std::apply([&os,&m](auto&... p){
    ((os<<" "<<p.first<<"*log("<<p.second<<")"),...);},m.units);
    return os;
  }

};



template<class ...u1, class... u2>
auto operator +(const logv_units<u1...>& x,const logv_units<u2...>& y )
{
  typedef transfer_t<pack_difference_t<Cs<u2...>,Cs<u1...>>,logv_units<u1...>> res_type;
  res_type out;
  ((out.template get<u1>()+=x.template get_number<u1>()),...);
  ((out.template get<u2>()+=y.template get_number<u2>()),...);
  return out;

}





template<class T,class... Units> class logv<T,Units...>
{
  T value_;
  logv_units<Units...> us_;





public:

  logv(T&& x,std::pair<long,Units>... us): value_{std::move(x)}, us_{std::move(us)...}{}
  logv(T&& x,logv_units<Units...>&& us): value_{std::move(x)}, us_{std::move(us)}{}
  const T& value()const &{return value_;}
  T value()&& {return value_;}
  auto& size()const {return us_();}
  auto& units()const {return us_;}
  auto& units() {return us_;}
  auto& size() {return us_();}
  friend std::ostream& operator<<(std::ostream& os, const logv& m)
  {
   os<<m.value();
   os<<m.units();
    return os;
  }
  friend logv operator-(logv&& me)
  {
    me.value_=-me.value_;
    me.size()=std::apply([](auto&&...p)
                        {return std::make_tuple(std::make_pair(-p.first,p.second)...);},
                          std::move(me.size()));
    return me;
  }
  friend logv operator-(const logv& x)
  {
    logv me=x;
    return -std::move(me);
  }


  friend logv operator-(double a, logv me)
  {
    me=-me;
    me.value_+=a;
    return me;
  }
  friend logv operator-(logv me, v<double,dimension_less> a)
  {
    me.value_-=a.value();
    return me;
  }




};

template<class T,class... units> logv(T,logv_units<units...>)->logv<T,units...>;


template<class T,class... units1, class ...units2>
auto operator+(const logv<T,units1...>& me, const logv<T,units2...>& ti)
{
  return logv(me.value()+ti.value(),me.units()+ti.units());

}



template <class T>
v(T&& x)->v<T,dimension_less>;
template<class T,class unit>
v(T&& x,unit)->v<T,unit>;

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
auto operator/(p<units1...>,p<units2...>)
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


template <class T, class U,class unit1, class unit2>
auto operator*( const v<T,unit1>& one, const  v<U,unit2>& other)
{
  return v(one.value()*other.value(),unit1{}*unit2{});
}


template <class T, class unit1>
auto operator*( const v<T,unit1>& one, double other)
{
  return v(one.value()*other,unit1{});
}
template <class T, class unit1>
auto operator*(  double other,const v<T,unit1>& one)
{
  return one*other;
}


template <class T, class U,class unit1, class unit2>
auto operator/( const v<T,unit1>& one, const  v<U,unit2>& other)
{
  return v(one.value()/other.value(),unit1{}/unit2{});
}




template <class T, class U,class unit1>
auto operator+( const v<T,unit1>& one, const  v<U,unit1>& other)
{
  return v(one.value()+other.value(),unit1{});
}




template <class T, class U,class unit1>
auto operator-( const v<T,unit1>& one, const  v<U,unit1>& other)
{
  return v(one.value()-other.value(),unit1{});
}


template <class T, class unit1>
auto operator-( const v<T,unit1>& one)
{
  return v(-one.value(),unit1{});
}










template <class T,class unit1>
auto sqr(const v<T,unit1>& x) { return x * x; }


inline v<double,dimension_less> exp(const v<double,dimension_less>& x)
{
  return v<double,dimension_less>(std::exp(x.value()));
}

template <class unit1>
auto sqrt(const v<double,unit1>& x) { return v<double,div_exponent_t<unit1,2>>(std::sqrt(x.value())); }

template <class unit1>
auto log(const v<double,unit1>& x) { return logv<double,unit1>(std::log(x.value()),{1,unit1{}}); }



//} // namespace qm


#endif // QM_UNIT_H
