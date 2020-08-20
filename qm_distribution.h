#ifndef QM_DISTRIBUTION_H
#define QM_DISTRIBUTION_H

#include "static_string.h"


template<class Id,class Distribution>
class  D
{
public:
  typedef   Id myId;

  inline static constexpr auto name=eol_s+Id::name+my_static_string("~~")+Distribution::name;

  constexpr auto operator[](Id)const {return *this;}

  constexpr D(Id ,Distribution ){}
  constexpr D(){}
};



#endif // QM_DISTRIBUTION_H
