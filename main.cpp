#include <iostream>
#include <random>
#include "qm_unit.h"
#include "qm_distribution.h"
#include "qm_models.h"
using namespace qm;
typedef p<u<m,1>> meter;

struct position
{
  typedef double T;
  typedef meter unit;
  constexpr static auto  className=my_static_string("position");
};




int main()
{
  qm::Normal_Distribution<position> normal;

  qm::Data<mean<position>,stddev<position>> d(v(1.0,meter{}),v(0.1,meter{}));


  std::random_device rd;
  auto initseed = rd();

  std::mt19937 mt(initseed);

  auto s=normal.sample(d,mt);

  auto ss=s+Nothing{};
  auto logL=normal.logP(d,(s+Nothing{}));

  std::cout << d<<std::endl;

  std::cout << s<<std::endl;
  std::cout << logL<<std::endl;

  auto qui=quimulun{
    Normal_Distribution<position>{},
        Normal_Distribution<mean<position>>{},
       Exponential_Distribution<stddev<position>>{},
      Datum<mean<mean<position>>>{v(1.0,meter{})},
      Datum<stddev<mean<position>>>{v(0.2,meter{})},
      Datum<mean<stddev<position>>>{v(0.2,meter{})}
      };

  auto sss=sample(qui,C<double>{},mt);
  std::cout << sss<<std::endl;


  return 0;
}
