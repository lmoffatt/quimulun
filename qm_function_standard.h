#ifndef QM_FUNCTION_STANDARD_H
#define QM_FUNCTION_STANDARD_H

#include "static_string.h"
#include "qm_operator.h"

 struct Pi {
     friend constexpr auto collect_arguments(Pi){return arguments<>{};}
     inline static constexpr auto name=my_static_string("pi");
 };

 template <long v>
 struct N
 {
     friend constexpr auto collect_arguments(N){return arguments<>{};}
     inline static constexpr auto name= to_static_string<v>();};


template<class ei>
struct Log10_t{
  constexpr static auto name_impl=my_static_string("log10(");
  constexpr static auto name=name_impl+ei::name+my_static_string(")");
};



struct Exp_10_f{
  inline static constexpr auto name=my_static_string("exp10");
  template<class X>
  inline static constexpr auto get_name(){return name+my_static_string("(")+X::name+ my_static_string(")");}
};


struct Exp_f{
  inline static constexpr auto name=my_static_string("exp");
  template<class X>
  inline static constexpr auto get_name(){return name+my_static_string("(")+X::name+ my_static_string(")");}
};


struct Cos_f{
  inline static constexpr auto name=my_static_string("cos");
  template<class X>
  inline static constexpr auto get_name(){return name+my_static_string("(")+X::name+ my_static_string(")");}
};


struct S{
  inline static constexpr auto name=my_static_string("sum");
  template<class X, class... Y>
  inline static constexpr auto get_name(){return (X::name+...+(my_static_string("+")+Y::name));}
  inline static constexpr auto get_name(){return name;}
};
struct P{
  inline static constexpr auto name=my_static_string("product");
  template<class X, class Y>
  inline static constexpr auto get_name(){return X::name+my_static_string("*")+Y::name;}
};

struct Div{
  inline static constexpr auto name=my_static_string("div");
  template<class X, class Y>
  inline static constexpr auto get_name(){return X::name+my_static_string("*")+Y::name;}
};



struct Minus{
  inline static constexpr auto name=my_static_string("minus");
  template<class X>
  inline static constexpr auto get_name(){return my_static_string("-")+X::name;}
 };





template <class X>
constexpr auto Exp10(X)
{
  return  Op(Exp_10_f{},X{});
};

template <class X>
constexpr auto Exp(X)
{
  return  Op(Exp_f{},X{});
};


template <class X>
constexpr auto Cos(X)
{
  return  Op(Cos_f{},X{});
};


template<class X, class Y>
constexpr auto operator+ (X,Y)
{
  return Op(S{},X{},Y{});
}


template<class X, class Y>
constexpr auto operator* (X,Y)
{
  return Op(P{},X{},Y{});
}

template<class X>
constexpr auto operator -(X)
{
  return Op(Minus{},X{});
}

template<class X, class Y>
constexpr auto operator/ (X,Y)
{
  return Op(Div{},X{},Y{});
}




#endif // QM_FUNCTION_STANDARD_H
