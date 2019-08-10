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

template<class Id, class T>
class Exponential_Distribution;

template<class Id>
struct Normal_Distribution<Id,double>
{
  static_assert (std::is_same_v<typename Id::T,double >);
  typedef typename Id::unit  unit;
  typedef mult_exponent_t<unit,-1> unit_1;
  template<class Rnd, class Datas>
  auto sample(const Datas& par, Rnd& mt)const
  {
    auto m=par(mean<Id>{});
    auto std=par(stddev<Id>{});
    if constexpr (std::is_same_v<decltype (m),Nothing >|| std::is_same_v<decltype (std),Nothing >)
      return Nothing{};
    else
     return Datum<Id>(std::normal_distribution<double>
                       {center(m.value()).value(),center(std.value()).value()}(mt));
  }

  template<class Param, class Datas>
  auto logP(const Param& par,const Datas& d) const {
    auto std=par(stddev<Id>{}).value();
    auto m=par(mean<Id>{}).value();
    auto x=d(Id{}).value();
    auto logs=- log(std);

    return -0.5 * std::log(2 * PI) -  log(std) -
           v(0.5) * sqr((x - m) / std);
  }
};


template<class Id>
struct Exponential_Distribution<Id,double>
{
  static_assert (std::is_same_v<typename Id::T,double >);
  typedef typename Id::unit  unit;
  typedef mult_exponent_t<unit,-1> unit_1;
  template<class Rnd, class Datas>
  auto sample(const Datas& par, Rnd& mt)const
  {
    auto m=par(mean<Id>{});
    if constexpr (std::is_same_v<decltype (m),Nothing >)
      return Nothing{};
    else
    return Datum<Id>(std::exponential_distribution<double>
                       {1.0/center(m.value()).value()}(mt));
  }

  template<class Param, class Datas>
  auto logP(const Param& par,const Datas& d) const {
    auto m=value(par,mean<Id>{});
    auto x=value(d,Id{});
        return -log(m) -(x/ m);
  }
};
template<class Distribution,class Rnd, class Datas>
auto sample(const Distribution& D,const Datas& d,Rnd& mt)->decltype (D.sample(d,mt))
{
  return D.sample(d,mt);
}






} // namespace qm

#endif // QM_DISTRIBUTION_H
