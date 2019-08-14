#ifndef MYTYPETRAITS_H
#define MYTYPETRAITS_H
#include <type_traits>
#include <utility>
#include <array>
#include <string>
#include <iomanip>

namespace qm {

template <std::size_t N> struct my_static_string {
private:
  std::array<char, N> c_;
public:
  constexpr my_static_string(const char (&c)[N]) : c_{} {
    for (std::size_t i = 0; i < N; ++i)
      c_[i] = c[i];
  }
  constexpr my_static_string(std::array<char, N> c) : c_{c} {
  }
  constexpr char operator[](std::size_t i) const { return c_[i]; }

  template <std::size_t N0>
  constexpr my_static_string(my_static_string<N0> one,
                             my_static_string<N - N0 + 1> two)
      : c_{} {
    for (std::size_t i = 0; i < N0 - 1; ++i)
      c_[i] = one[i];
    for (std::size_t i = N0 - 1; i < N - 1; ++i)
      c_[i] = two[i + 1 - N0];

    c_[N - 1] = '\0';
  }

  constexpr const char *c_str() const { return &c_[0]; }

  std::string str() const { return c_str(); }
};

template <int N>
my_static_string(const char (&lit)[N]) // <- match this
    ->my_static_string<N>;

template <std::size_t N1, std::size_t N2>
constexpr auto operator+(const my_static_string<N1> &s1,
                         const my_static_string<N2> &s2)
    -> my_static_string<N1 + N2 - 1> {
  return my_static_string<N1 + N2 - 1>(s1, s2);
}

template <int N>
constexpr auto to_static_string()
{
  if constexpr (N<0)
  {
    return my_static_string("-")+to_static_string<-N>();
  }
  else if constexpr (N<10)
  {
    return my_static_string({'0' + N,'\0'});
  }
  else
  {
    constexpr int N10=N/10;
    constexpr int N0=N-N10*10;
    return to_static_string<N10>()+ my_static_string({'0' + N0,'\0'});
  }
}


template <class T>
auto operator<<(std::ostream& os, T)->decltype (T::className,os)
{
  return os<<T::className.c_str();
}




template<class...> struct Cs{};
template<class...> struct C{};

template <typename T> struct C<T> { typedef T type; };

template<typename T>
auto& center(const T& x){ return x;}



template<class Id, class...Ids>
constexpr bool is_in_pack()
{
  return (std::is_same_v<Id,Ids >||...||false);
}

template<class Id, class...Ids>
constexpr bool is_in_pack(Id, Cs<Ids...>)
{
  return (std::is_same_v<Id,Ids >||...||false);
}



namespace impl {
template <template <typename...> class Cs, typename... Ts, typename T, std::size_t ...Is>
auto constexpr index_of_this_type(Cs<Ts...>, T, std::index_sequence<Is...>) {
  return ((std::is_same_v<Ts,T> ? Is : 0 )+...);
};
}

template <template <typename...> class Cs, template <typename> class C,typename... Ts, typename T>
auto constexpr index_of_this_type(Cs<T,Ts...> c, C<T> t) {
  return impl::index_of_this_type(c,t,std::index_sequence_for<Ts...>());
};


template<class, class> struct transfer{};


template<template<class...>class Co,template<class...>class D, class...T>
struct transfer<Co<T...>,D<>>
{
  typedef D<T...> type;
};

template<template<class...>class Co,template<template<class...>class,class...>class D, template<class...>class Tr,class...T>
struct transfer<Co<T...>,D<Tr>>
{
  typedef D<Tr,T...> type;
};


template<class S, class D>
using transfer_t=typename transfer<S,D>::type;




} // namespace qm


#endif // MYTYPETRAITS_H
