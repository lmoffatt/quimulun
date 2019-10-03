#include <iostream>
#include <random>
#include "qm_unit.h"
#include "qm_distribution.h"
#include "qm_models.h"
#include "qm_derivative.h"
#include "qm_vector_space.h"
#include "qm_tensor_derivative.h"
#include "qm_tensor_distribution.h"
#include "qm_tensor_model.h"
#include "qm_tensor_coordinate.h"
//using namespace qm;
typedef p<u<m,1>> meter;
typedef p<u<s,1>> second;
typedef p_t<u<m,1>,u<s,-1>> meters_per_second;
typedef p_t<u<m,1>,u<s,-2>> meters_per_second2;

struct i__{};
struct j__{};
struct k__{};
struct l__{};


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
struct asceleration
{
  typedef double T;
  typedef meters_per_second2 unit;
  constexpr static auto  className=my_static_string("asceleration");
};

namespace ten {


auto r=te<up<position,mytime>,dn<>>{}(up<i__,j__>{},dn<>{})*te<up<position>,dn<velocity>>{}(up<k__>{},dn<j__>{});
}

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
  auto initseed = 0;
  //rd();

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
 auto datum=Datum(der<Id,mytime>{},v(0.0,second{}));
 auto ddd=Data(model_position{},Datum(der<Id,mytime>{},v(0.0,dimension_less{}/second{})),Datum(der<Id,velocity>{},v(0.0,dimension_less{}/meters_per_second{})));
 std::cerr<<"ddd ="<<ddd*ddd<<"\n";
 // typedef typename decltype (ddd*ddd)::dd  ddeg;


 typedef  der_t<dvalue_ddata,mydata> d2value_ddata;

  typedef  der_t<mytime_datum,mydata> ddatum_ddata;
  //typedef  der_t<mytime_datum,ddatum_ddata> ddatum_ddata_inv;
  typedef  der_t<mydata,mydata> ddata_ddata;
 // typedef  der_t<mydata,ddata_ddata> ddata_ddata_inv;

  // typedef der_t<myder,Data<model_position,Datum<position,v<double,meter>,vec<>>>> myder2;

 // typedef typename dvalue_ddata::dd detwetr;
 //  typedef typename d2value_ddata::dd detwetr2;

 //  typedef typename dvalue_ddata_inv::dd detwetr;
 //   typedef typename ddatum_ddata::dd detwetr2;
//    typedef typename ddatum_ddata_inv::dd detwetr2;
 //   typedef typename ddata_ddata::dd detwetr3;
   // typedef typename ddata_ddata_inv::dd detwetr4;



  auto qui=quimulun{
      model_position{},
      Datum(Start<Start<mytime>>{},v(0.0,second{})),
      Datum(Step<Start<mytime>>{}, v(100.0,second{})),
      Datum(Duration<Start<mytime>>{}, v(200.0,second{})),
      Datum(Duration<mytime>{}, v(1.0,second{})),
      Datum(Step<mytime>{}, v(0.5,second{})),
      Datum(mean<velocity>{}, v(1.0,meters_per_second{})),
      Datum(mean<asceleration>{}, v(0.01,meters_per_second2{})),
      Datum(stddev<asceleration>{}, v(0.01,meters_per_second2{})),

      Coord(Start<mytime>{},EvenCoordinate{},Start<Start<mytime>>{},Duration<Start<mytime>>{},Step<Start<mytime>>{}),
      Coord(mytime{},EvenCoordinate{},Start<mytime>{},Duration<mytime>{},Step<mytime>{}),
      F(mean<position>{},[](auto time, auto velocity, auto asceleration){return time*velocity+0.5*time*time*asceleration;},
          mytime{},velocity{},asceleration{}),

      D(velocity{},Normal_Distribution{},mean<velocity>{},stddev<velocity>{}),
      D(asceleration{},Normal_Distribution{},mean<asceleration>{},stddev<asceleration>{}),

      D(position{},Normal_Distribution{},mean<position>{},stddev<position>{}),
      //F(position2{},[](auto x){return Datum(position2{}, x.value()*x.value()};},position{}),
      // Normal_Distribution<mean<position>>{},
      D(stddev<position>{},Exponential_Distribution{},mean<stddev<position>>{}),
      D(stddev<velocity>{},Exponential_Distribution{},mean<stddev<velocity>>{}),
      Datum(mean<mean<position>>{}, v(1.0,meter{})),
      Datum(stddev<mean<position>>{}, v(0.2,meter{})),
      Datum(mean<stddev<position>>{}, v(0.2,meter{})),
      Datum(mean<stddev<velocity>>{}, v(1.0,meters_per_second{}))
      };

  using ten::x_i;
  auto qui2=ten::quimulun{
      model_position{},
      x_i(Start<Start<mytime>>{},v(0.0,second{})),
      x_i(Step<Start<mytime>>{}, v(100.0,second{})),
      x_i(Duration<Start<mytime>>{}, v(200.0,second{})),
      x_i(Duration<mytime>{}, v(1.0,second{})),
      x_i(Step<mytime>{}, v(0.5,second{})),
      x_i(mean<velocity>{}, v(1.0,meters_per_second{})),
      x_i(mean<asceleration>{}, v(0.01,meters_per_second2{})),
      x_i(stddev<asceleration>{}, v(0.01,meters_per_second2{})),

      ten::Coord(Start<mytime>{},EvenCoordinate{},Start<Start<mytime>>{},Duration<Start<mytime>>{},Step<Start<mytime>>{}),
      ten::Coord(mytime{},EvenCoordinate{},Start<mytime>{},Duration<mytime>{},Step<mytime>{}),
      ten::F(mean<position>{},[](auto time, auto velocity, auto asceleration){return time*velocity+0.5*time*time*asceleration;},
          mytime{},velocity{},asceleration{}),

      ten::D(velocity{},ten::Normal_Distribution{},mean<velocity>{},stddev<velocity>{}),
      ten::D(asceleration{},ten::Normal_Distribution{},mean<asceleration>{},stddev<asceleration>{}),

      ten::D(position{},ten::Normal_Distribution{},mean<position>{},stddev<position>{}),
      //F(position2{},[](auto x){return Datum(position2{}, x.value()*x.value()};},position{}),
      // Normal_Distribution<mean<position>>{},
      ten::D(stddev<position>{},ten::Exponential_Distribution{},mean<stddev<position>>{}),
      ten::D(stddev<velocity>{},ten::Exponential_Distribution{},mean<stddev<velocity>>{}),
      x_i(mean<mean<position>>{}, v(1.0,meter{})),
      x_i(stddev<mean<position>>{}, v(0.2,meter{})),
      x_i(mean<stddev<position>>{}, v(0.2,meter{})),
      x_i(mean<stddev<velocity>>{}, v(1.0,meters_per_second{}))
  };


  typedef Cs<position> data_fields;
  typedef Cs<stddev<velocity>,stddev<position>,velocity,asceleration> param_fields;

  //typedef typename decltype (typename mytime::unit{}* typename velocity::unit{})::dge de;

  {
  auto s=sample(qui,Data(model_position{}),mt);
  auto par=s| myselect<param_fields>{};
  auto dpar=Self_Derivative(par);
  auto data=s| myselect<data_fields>{};


  std::cout << "parameters \n"<<par <<std::endl;
  std::cout << "dparameters \n"<<dpar <<std::endl;

  std::cout << "data \n"<<data <<std::endl;

  auto logL=logP(qui,s);
  auto dlogL=logP(qui,data,dpar);
  auto fim=FIM(qui,data,dpar);


  std::cout << "logL"<< logL<<std::endl;
 std::cout << dlogL<<std::endl;
 std::cerr<<"FIM "<<fim<<"\n";
  }
  {
    std::cerr<<"-------------new stuff--------- \n";
    std::mt19937 mt(initseed);

    auto s=ten::sample(qui2,ten::vector_space<>(),mt);
    auto par=s| ten::myselect<param_fields>{};
    auto dpar=ten::Self_Derivative(par);
    auto data=s| ten::myselect<data_fields>{};


    std::cerr << "parameters \n"<<par <<std::endl;
    std::cerr << "dparameters \n"<<dpar <<std::endl;

    std::cerr << "data \n"<<data <<std::endl;

    auto logL=ten::logP(qui2,s);
    auto dlogL=ten::logP(qui2,data,dpar);


    auto fim=ten::FIM(qui2,data,dpar);


    std::cerr <<"logL"<< logL<<std::endl;
    std::cerr << "dlogL"<< dlogL<<std::endl;
    std::cerr<<"FIM "<<fim<<"\n";
  }
  return 0;
}
