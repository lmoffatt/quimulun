#include <iostream>
#include "qm_unit.h"
#include "qm_distribution.h"
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
  qm::Normal_Distribution<position, double> normal;

  qm::Data<mean<position>,stddev<position>> d(v(1.0,meter{}),v(0.1,meter{}));

  std::mt19937 mt;

  auto s=normal.sample(d,mt);

  auto logL=normal.logP(d,s);

  std::cout << d<<std::endl;

  std::cout << s<<std::endl;
  std::cout << logL<<std::endl;

  return 0;
}
