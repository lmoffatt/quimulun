#include <iostream>
#include <random>
#include "qm_unit.h"
#include "qm_distribution.h"
#include "qm_models.h"
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

  //qm::Data<mean<position>,stddev<position>> d(v(1.0,meter{}),v(0.1,meter{}));


  std::random_device rd;
  auto initseed = rd();

  std::mt19937 mt(initseed);

 // auto s=normal.sample(d,mt);

 // auto ss=s+Nothing{};
//  auto logL=normal.logP(d,(s+Nothing{}));

//  std::cout << d<<std::endl;

//  std::cout << s<<std::endl;
//  std::cout << logL<<std::endl;




  auto qui=quimulun{
      model_position{},
      Datum<Start<Start<mytime>>,vec<>>{v(0.0,second{})},
      Datum<Step<Start<mytime>>,vec<>>{v(100.0,second{})},
      Datum<Duration<Start<mytime>>,vec<>>{v(1000.0,second{})},
      Datum<Duration<mytime>,vec<>>{v(1.0,second{})},
      Datum<Step<mytime>,vec<>>{v(0.01,second{})},
      Datum<velocity,vec<>>{v(1.0,meters_per_second{})},
      Coord(Start<mytime>{},EvenCoordinate{},Start<Start<mytime>>{},Duration<Start<mytime>>{},Step<Start<mytime>>{}),
      Coord(mytime{},EvenCoordinate{},Start<mytime>{},Duration<mytime>{},Step<mytime>{}),
      F(mean<position>{},[](auto time, auto velocity){return time*velocity;},mytime{},velocity{}),

      D(position{},Normal_Distribution{},mean<position>{},stddev<position>{}),
      //F(position2{},[](auto x){return Datum<position2,vec<>>{x.value()*x.value()};},position{}),
      // Normal_Distribution<mean<position>>{},
      D(stddev<position>{},Exponential_Distribution{},mean<stddev<position>>{}),
      Datum<mean<mean<position>>,vec<>>{v(1.0,meter{})},
      Datum<stddev<mean<position>>,vec<>>{v(0.2,meter{})},
      Datum<mean<stddev<position>>,vec<>>{v(0.2,meter{})}
      };

/*  auto qui2=quimulun{
      model_position{},
      Normal_Distribution<position>{},
        quimulun{distribution<mean<position>>{},
            Normal_Distribution<mean<position>>{},
      Datum<mean<mean<position>>>{v(1.0,meter{})},
               Datum<stddev<mean<position>>>{v(0.2,meter{})}},
        quimulun{distribution<stddev<position>>{},
                 Exponential_Distribution<stddev<position>>{},
      Datum<mean<stddev<position>>>{v(0.2,meter{})}
      }
  };

*/

//  auto sss=sample<C>(qui,mt);
//  std::cout << sss<<std::endl;

  auto ssss=sample<C>(qui,model_position{},mt);
  std::cout << ssss<<std::endl;

  //auto s2=sample(qui2,model_position{},C<double>{},mt);
  //std::cout << s2<<std::endl;

  std::cerr<< "size of qm::v<double,meter> ="<<sizeof (qm::v<double,meter>)<<"\n";
  std::cerr<< "size of double ="<<sizeof (double)<<"\n";
  std::cerr<< "size of bool ="<<sizeof (bool)<<"\n";
  std::cerr<< "size of std::string ="<<sizeof (std::string)<<"\n";
  std::cerr<< "size of char ="<<sizeof (char)<<"\n";
  std::cerr<< "size of position::className ="<<sizeof position::className<<"\n";
  std::cerr<< "size of short ="<<sizeof (short)<<"\n";
  std::cerr<< "size of int ="<<sizeof (int)<<"\n";
  std::cerr<< "size of std::size_t ="<<sizeof (std::size_t)<<"\n";
  std::cerr<< "size of long ="<<sizeof (long)<<"\n";
  std::cerr<< "size of long double ="<<sizeof (long double)<<"\n";
  std::cerr<< "size of long long ="<<sizeof (long long)<<"\n";
  std::cerr<< "size of qm::Datum<position> ="<<sizeof (qm::Datum<position,vec<>>)<<"\n";
  //std::cerr<< "size of Normal_Distribution<mean<position>> ="<<sizeof(Normal_Distribution<mean<position>>)<<"\n";
  std::cerr<< "size of qui ="<<sizeof qui<<"\n";
  std::cerr<< "size of decltype(qui) ="<<sizeof (decltype(qui))<<"\n";

  return 0;
}
