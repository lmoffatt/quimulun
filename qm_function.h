#ifndef QM_FUNCTION_H
#define QM_FUNCTION_H

#include "static_string.h"


template<class Id,class G>
class  F
{
public:
  typedef   Id myId;
  inline static constexpr auto name_impl=my_static_string("\n");
  inline static constexpr auto name=name_impl+Id::name+my_static_string("->")+G::name;

  constexpr auto operator[](Id)const {return *this;}

  constexpr F(Id ,G&& ){}
  constexpr F(){}
};





#endif // QM_FUNCTION_H
