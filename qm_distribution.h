#ifndef QM_DISTRIBUTION_H
#define QM_DISTRIBUTION_H
#include <iomanip>

#include "qm_data.h"
#include <random>

namespace qm {
inline constexpr double PI = 3.14159265358979323846;


template<class Id>
struct mean{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_mean");
};

template<class Id>
struct stddev{
  typedef typename Id::T T;
  typedef typename Id::unit unit;
  constexpr static auto  className=Id::className+my_static_string("_stddev");
};

template<class Id>
struct variance{
  typedef typename Id::T T;
 // typedef typename Id::unit unit;
  constexpr static auto  className=Id::className+my_static_string("_variance");
};




template<class Id, class T>
class Normal_Distribution;

template<class Id>
struct Normal_Distribution<Id,double>
{
  static_assert (std::is_same_v<typename Id::T,double >);
  typedef typename Id::unit  unit;
  typedef mult_exponent_t<unit,-1> unit_1;
  template<class Rnd, class Datas>
  Data<Id> sample(const Datas& d, Rnd& mt)const
  {
    return Data<Id>(std::normal_distribution<double>
                    {center(value(d,mean<Id>{}).value()),center(value(d,stddev<Id>{}).value())}(mt));
  }

  template<class Param, class Datas>
  auto logP(const Param& par,const Datas& d) const {
    auto std=value(par,stddev<Id>{});
    auto m=value(par,mean<Id>{});
    auto x=value(d,Id{});
    auto logs=- log(std);

    return -0.5 * std::log(2 * PI) -  log(std) -
           v(0.5) * sqr((x - m) / std);
  }
};





} // namespace qm

#endif // QM_DISTRIBUTION_H
