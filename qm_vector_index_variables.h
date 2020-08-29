#ifndef QM_VECTOR_INDEX_VARIABLES_H
#define QM_VECTOR_INDEX_VARIABLES_H

#include "static_string.h"


template<class Id>  struct Index{
  inline constexpr static auto  name_impl=my_static_string("index(");
  inline constexpr static auto  name=name_impl+Id::name+my_static_string(")");
};





#endif // QM_VECTOR_INDEX_VARIABLES_H
