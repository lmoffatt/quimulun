#include <iostream>
#include <random>
#include "qm_unit.h"
#include "qm_distribution.h"
#include "qm_models.h"
#include "qm_derivative.h"
//using namespace qm;
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

struct logLik
{
  typedef double T;
  typedef dimension_less unit;
  constexpr static auto  className=my_static_string("logLik");
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


  auto x=Data(position{},Datum(position2{},0), Data(mean<position>{},Datum(stddev<position>{},{9,meter{}})));

  std::cerr<<x[mean<position>{}][stddev<position>{}]<<"\n";


  std::random_device rd;
  auto initseed = rd();

  std::mt19937 mt(initseed);

  typedef der_t<position,mytime> dpos_dtime;

  typedef der_t<position,dpos_dtime> ddtime;


  typedef v<double,second> mytime_value;
  typedef Datum<mytime,v<double,second>,vec<>> mytime_datum;
  typedef Datum<position,v<double,meter>,vec<>> myposition_datum;
  typedef Data<model_position,mytime_datum,myposition_datum> mydata;
  typedef  der_t<mytime_value,mydata> dvalue_ddata;
 typedef  der_t<mytime_value,dvalue_ddata> dvalue_ddata_inv;
 static_assert (std::is_same_v<mydata,dvalue_ddata_inv > );

 typedef  der_t<dvalue_ddata,mydata> d2value_ddata;

  typedef  der_t<mytime_datum,mydata> ddatum_ddata;
  //typedef  der_t<mytime_datum,ddatum_ddata> ddatum_ddata_inv;
  typedef  der_t<mydata,mydata> ddata_ddata;
 // typedef  der_t<mydata,ddata_ddata> ddata_ddata_inv;

  // typedef der_t<myder,Data<model_position,Datum<position,v<double,meter>,vec<>>>> myder2;

   typedef typename dvalue_ddata::dd detwetr;
   typedef typename d2value_ddata::dd detwetr2;

//   typedef typename dvalue_ddata_inv::dd detwetr;
//    typedef typename ddatum_ddata::dd detwetr2;
//    typedef typename ddatum_ddata_inv::dd detwetr2;
//    typedef typename ddata_ddata::dd detwetr3;
   // typedef typename ddata_ddata_inv::dd detwetr4;



  auto qui=quimulun{
      model_position{},
      Datum(Start<Start<mytime>>{},v(0.0,second{})),
      Datum(Step<Start<mytime>>{}, v(100.0,second{})),
      Datum(Duration<Start<mytime>>{}, v(1000.0,second{})),
      Datum(Duration<mytime>{}, v(1.0,second{})),
      Datum(Step<mytime>{}, v(0.01,second{})),
      Datum(mean<velocity>{}, v(1.0,meters_per_second{})),
      Datum(stddev<velocity>{}, v(1.0,meters_per_second{})),

      Coord(Start<mytime>{},EvenCoordinate{},Start<Start<mytime>>{},Duration<Start<mytime>>{},Step<Start<mytime>>{}),
      Coord(mytime{},EvenCoordinate{},Start<mytime>{},Duration<mytime>{},Step<mytime>{}),
      F(mean<position>{},[](auto time, auto velocity){return time*velocity;},mytime{},velocity{}),

      D(velocity{},Normal_Distribution{},mean<velocity>{},stddev<velocity>{}),

      D(position{},Normal_Distribution{},mean<position>{},stddev<position>{}),
      //F(position2{},[](auto x){return Datum(position2{}, x.value()*x.value()};},position{}),
      // Normal_Distribution<mean<position>>{},
      D(stddev<position>{},Exponential_Distribution{},mean<stddev<position>>{}),
      Datum(mean<mean<position>>{}, v(1.0,meter{})),
      Datum(stddev<mean<position>>{}, v(0.2,meter{})),
      Datum(mean<stddev<position>>{}, v(0.2,meter{}))
      };

  //typedef typename decltype (typename mytime::unit{}* typename velocity::unit{})::dge de;

  auto s=sample(qui,Data(model_position{}),mt);
  auto ss=s| myselect<mean<velocity>,stddev<position>,stddev<velocity>>{};
  auto ds=self_Derivative(std::move(ss));
  auto ds2=s<<ds;


  std::cout << ds <<std::endl;

  auto logL=logP(qui,s);
  auto dlogL=logP(qui,ds2);
  auto dlogLL=dlogL;
  auto ddlogL=Data(model_position{},Datum(logLik{},std::move(dlogLL)));
  std::cerr<<ddlogL<<"\n";
  //std::cerr<<"derivative"<<ddlogL[der<logLik,mean<velocity>>{}]<<"\n";
//  std::cout << s <<std::endl;
  auto s2=s;
  auto s3=s;
  auto sss=std::move(s2)+std::move(s3);
 // std::cout<<s<<"\n";
  std::cout << logL<<std::endl;
 std::cout << dlogL<<std::endl;

  return 0;
}
