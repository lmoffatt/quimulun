#include <iostream>
#include <random>
#include "qm_unit.h"
#include "qm_distribution.h"
#include "qm_models.h"
#include "qm_derivative.h"
using namespace qm;
typedef p<u<m,1>> meter;
typedef p<u<s,1>> second;
typedef p_t<u<m,1>,u<s,-1>> meters_per_second;

struct position
{
  typedef double T;
  typedef meter unit;
  constexpr static auto  className=my_static_string("position");
};
struct mytime
{
  typedef double T;
  typedef second unit;
  constexpr static auto  className=my_static_string("time");
};

struct velocity
{
  typedef double T;
  typedef meters_per_second unit;
  constexpr static auto  className=my_static_string("velocity");
};



struct position2
{
  typedef double T;
  typedef decltype (meter{}*meter{}) unit;
  constexpr static auto  className=my_static_string("position2");
};



struct model_position {  constexpr static auto  className=my_static_string("model_position");
};

template <class T> struct distribution{ constexpr static auto  className=T::className+my_static_string("_distribution");
};

int main()
{



  std::random_device rd;
  auto initseed = rd();

  std::mt19937 mt(initseed);




  auto qui=quimulun{
      model_position{},
      Datum_(Start<Start<mytime>>{},v(0.0,second{})),
      Datum_(Step<Start<mytime>>{}, v(100.0,second{})),
      Datum_(Duration<Start<mytime>>{}, v(1000.0,second{})),
      Datum_(Duration<mytime>{}, v(1.0,second{})),
      Datum_(Step<mytime>{}, v(0.01,second{})),
      Datum_(mean<velocity>{}, v(1.0,meters_per_second{})),
      Datum_(stddev<velocity>{}, v(1.0,meters_per_second{})),

      Coord(Start<mytime>{},EvenCoordinate{},Start<Start<mytime>>{},Duration<Start<mytime>>{},Step<Start<mytime>>{}),
      Coord(mytime{},EvenCoordinate{},Start<mytime>{},Duration<mytime>{},Step<mytime>{}),
      F(mean<position>{},[](auto time, auto velocity){return time*velocity;},mytime{},velocity{}),

      D(velocity{},Normal_Distribution{},mean<velocity>{},stddev<velocity>{}),

      D(position{},Normal_Distribution{},mean<position>{},stddev<position>{}),
      //F(position2{},[](auto x){return Datum_(position2{}, x.value()*x.value()};},position{}),
      // Normal_Distribution<mean<position>>{},
      D(stddev<position>{},Exponential_Distribution{},mean<stddev<position>>{}),
      Datum_(mean<mean<position>>{}, v(1.0,meter{})),
      Datum_(stddev<mean<position>>{}, v(0.2,meter{})),
      Datum_(mean<stddev<position>>{}, v(0.2,meter{}))
      };

  //typedef typename decltype (typename mytime::unit{}* typename velocity::unit{})::dge de;

  auto s=sample(qui,Data_(model_position{}),mt);
  auto ss=s| qm::select<mean<velocity>,stddev<position>>{};
  auto ds=self_Derivative(std::move(ss));
  auto ds2=s<<ds;


  std::cout << ds <<std::endl;

  auto logL=logP(qui,s);
  auto dlogL=logP(qui,ds2);

  std::cout << s <<std::endl;
  auto s2=s;
  auto s3=s;
  auto sss=std::move(s2)+std::move(s3);
  std::cout<<s<<"\n";
  std::cout << logL<<std::endl;
  std::cout << dlogL<<std::endl;

  return 0;
}
