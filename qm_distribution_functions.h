#ifndef QM_DISTRIBUTION_FUNCTIONS_H
#define QM_DISTRIBUTION_FUNCTIONS_H

#include "qm_operator.h"


struct Normal_Distribution_f{
  inline static constexpr auto name=my_static_string("Normal_distribution");

  template<class Mean, class Stddev>
  inline static constexpr auto get_name(){return name+my_static_string("(")+Mean::name+my_static_string("+")+Stddev::name;}


};



template <class Mean, class Stddev>
constexpr auto Normal_Distribution(Mean, Stddev)
{
  return  Op(Normal_Distribution_f{},Mean{},Stddev{});
};










#endif // QM_DISTRIBUTION_FUNCTIONS_H
