#ifndef QM_VALUE_H
#define QM_VALUE_H
#include "qm_unit.h"
#include <cmath>


template<class T> constexpr auto get_Name()
{
    return T::name;
}

template<> constexpr auto get_Name<double>()
{
    return my_static_string("real");
}

template<> constexpr auto get_Name<std::size_t>()
{
    return my_static_string("count");
}

template<> constexpr auto get_Name<std::string>()
{
    return my_static_string("label");
}


template <class T, class unit>
class v
{
private:
  T x_;
public:
  inline static constexpr auto name=get_Name<T>()+osbrk_s+unit::name+my_static_string("]");

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
  friend constexpr auto operator-(const v& one)
  {
    return v(-one(),unit{});
  }


  __host__ __device__
  friend constexpr auto operator-(const v& one, const v& two)
  {
    return v(one()-two(),unit{});
  }

  __host__ __device__
  friend constexpr auto operator*(const v& one, double two)
  {
      return v(one()*two);
  }
  __host__ __device__
  friend constexpr auto operator*(double two, const v& one)
  {
      return v(two*one());
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

 auto exp(const v<double,p<>>& x){ return v<double,p<>>(std::exp(x()));}
 auto cos(const v<double,p<>>& x){ return v<double,p<>>(std::cos(x()));}


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
      using test=typename unit_out::ja;
    return logv<R,unit_out>(one()+two(),unit_out{});
  }
  template<class U, class ...l_us2>
  friend constexpr auto operator-(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()-two())>;
    using unit_out=std::decay_t<decltype (one.unit()-two.unit())>;
      using test=typename unit_out::ja;
    return logv<R,unit_out>(one()-two(),one.unit()-two.unit());
  }


};

template <class T, class u>
class logv<T,l_u<u>>
{
private:
  T x_;
public:
  inline static constexpr auto log_name=my_static_string("log");
  inline static constexpr auto name=get_Name<T>()+osbrk_s+log_name+u::name+my_static_string("]");
  constexpr auto& operator()()const {return x_;}
  constexpr auto& operator()(){return x_;}

  constexpr auto unit()const {return l_u<p<u>>{};}


  constexpr  logv(T x, l_u<u> lu): x_{x}{}

  constexpr  logv(T x): x_{x}{}

  constexpr logv()=default;

  template<class U, class ...l_us2>
  friend constexpr auto operator+(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()+two())>;
    using unit_out=std::decay_t<decltype (one.unit()+two.unit())>;
    return logv<R,unit_out>(one()+two(),unit_out{});
  }

  template<class T2>
  friend constexpr auto operator+(const v<T2,p<>>& one, const logv& two)
  {
    return logv(one()+two());
  }
  template<class T2>
  friend constexpr auto operator+( const logv& two,const v<T2,p<>>& one)
  {
    return logv(two()+one());
  }



  template<class U, class ...l_us2>
  friend constexpr auto operator-(const logv& one, const logv<U,log_unit<l_us2...>>& two)
  {
    using R=std::decay_t<decltype (one()-two())>;
    using unit_out=std::decay_t<decltype (one.unit()-two.unit())>;
    using test=typename unit_out::ja;
    return logv<R,unit_out>(one()-two(),one.unit()-two.unit());
  }


  friend constexpr auto operator +(const logv& me, const v<double,p<>>& x)
  {
      return logv(me()+x());
  }

  friend constexpr auto operator +( const v<double,p<>>& x,const logv& me)
  {
      return logv(x()+me());
  }

  friend constexpr auto pow( double a, const logv& me)
  {
      using R=std::decay_t<decltype (std::pow(a,me()))>;


      return v<R,u>(std::pow(a,me()));
  }




  __host__ friend std::ostream& operator<<(std::ostream& os, const logv& x){ os<<x(); return os;}

  __host__ friend std::istream& operator>>(std::istream& is, logv& x){ is>>x(); return is;}


};

template <class T, class u>
logv(T,l_u<u>)->logv<T,l_u<u>>;





#endif // QM_VALUE_H
