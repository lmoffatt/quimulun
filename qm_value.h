#ifndef QM_VALUE_H
#define QM_VALUE_H
#include "qm_unit.h"


template <class T, class unit>
class v
{
private:
  T x_;
public:
  __host__ __device__
  constexpr auto& operator()()const {return x_;}

  __host__ __device__
  constexpr auto& operator()(){return x_;}

  __host__ __device__
  constexpr explicit v(T x, unit): x_{x}{}

  constexpr v()=default;

  __host__ __device__
      constexpr explicit v(T x): x_{x}{}


  __host__ __device__
  friend constexpr auto operator+(const v& one, const v& two)
  {
    return v(one()+two(),unit{});
  }

  __host__ __device__
  friend constexpr auto operator-(const v& one, const v& two)
  {
    return v(one()-two(),unit{});
  }

  template< class U, class unit2>
  __host__ __device__
  friend constexpr auto operator*(const v& one, const v<U,unit2>& two)
  {
    using R=std::decay_t<decltype (one()*two())>;
    using u=std::decay_t<decltype (unit{}*unit2{})>;
    return v<R,u>(one()*two(),u{});
  }

  template< class U, class unit2>
  __host__ __device__
  friend constexpr auto operator/(const v& one, const v<U,unit2>& two)
  {
    using R=std::decay_t<decltype (one()/two())>;
    using u=std::decay_t<decltype (unit{}/unit2{})>;
    return v<R,u>(one()/two(),u{});
  }

  __host__ friend std::ostream& operator<<(std::ostream& os, const v& x){ os<<x(); return os;}

  __host__ friend std::istream& operator>>(std::istream& is, v& x){ is>>x(); return is;}


};

template<class T>
v(T)->v<T,dimension_less>;


template <class , class >
class logv;


template <class T, class ... l_us>
class logv<T,log_unit<l_us...>>
{
private:
  T x_;
  log_unit<l_us...> lu_;
public:
  constexpr auto& operator()()const {return x_;}
  constexpr auto& operator()(){return x_;}

  constexpr auto& unit()const {return x_;}
  constexpr auto& unit(){return x_;}


  constexpr explicit logv(T x, log_unit<l_us...> lu): x_{x}, lu_{lu}{}


  template<class U, class ...l_us2>
  friend constexpr auto operator+(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()+two())>;
    using unit_out=std::decay_t<decltype (one.unit()+two.unit())>;
    return logv<R,unit_out>(one()+two(),unit_out{});
  }
  template<class U, class ...l_us2>
  friend constexpr auto operator-(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()-two())>;
    using unit_out=std::decay_t<decltype (one.unit()-two.unit())>;
    return logv<R,unit_out>(one()-two(),one.unit()-two.unit());
  }


};

template <class T, class u>
class logv<T,l_u<u>>
{
private:
  T x_;
public:
  constexpr auto& operator()()const {return x_;}
  constexpr auto& operator()(){return x_;}

  constexpr auto unit()const {return l_u<p<u>>{};}


  constexpr  logv(T x, l_u<u> lu): x_{x}{}


  template<class U, class ...l_us2>
  friend constexpr auto operator+(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()+two())>;
    using unit_out=std::decay_t<decltype (one.unit()+two.unit())>;
    return logv<R,unit_out>(one()+two(),unit_out{});
  }
  template<class U, class ...l_us2>
  friend constexpr auto operator-(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()-two())>;
    using unit_out=std::decay_t<decltype (one.unit()-two.unit())>;
    return logv<R,unit_out>(one()-two(),one.unit()-two.unit());
  }


};

template <class T, class u>
logv(T,l_u<u>)->logv<T,l_u<u>>;





#endif // QM_VALUE_H
