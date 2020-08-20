#ifndef QM_QUIMULUN_H
#define QM_QUIMULUN_H
#include "static_string.h"


template <class...xs>
struct quimulun: public xs...
{
  static inline constexpr auto quimulun_name=my_static_string("\nquimulun{");
  static inline constexpr auto name=(quimulun_name+...+xs::name)+my_static_string("\n}");
  using xs::operator[]...;

  constexpr quimulun(xs&&...x):xs{x}...{}

  constexpr quimulun(){}


};
template<class ...xs,class... xs2>
 auto operator&&(quimulun<xs...>&& , quimulun<xs2...>&& )
{return quimulun<xs...,xs2...>{} ;}


template <class...xs>
quimulun(xs&&...)->quimulun<xs...>;

#endif // QM_QUIMULUN_H
