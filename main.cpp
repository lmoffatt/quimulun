#include <iostream>
#include <random>
#include <fstream>
#include "mytypetraits.h"

#include "qm_unit.h"
//#include "qm_distribution.h"
//#include "qm_models.h"
//#include "qm_derivative.h"
#include "qm_vector_space.h"
#include "qm_tensor_derivative.h"
#include "qm_tensor_distribution.h"
#include "qm_tensor_model.h"
#include "qm_tensor_coordinate.h"
#include <cassert>
//using namespace qm;
typedef p<u<m,1>> meter;
typedef p<u<s,1>> second;
typedef p<u<m,1>,u<s,-1>> meters_per_second;
typedef p<u<m,1>,u<s,-2>> meters_per_second2;

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



auto r=te<up<position,mytime>,dn<>>{}(up<i__,j__>{},dn<>{})*te<up<position>,dn<velocity>>{}(up<k__>{},dn<j__>{});

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




  std::random_device rd;
  auto initseed = 0;
  //rd();

  std::mt19937 mt(initseed);





  auto qui=quimulun{
      x_i(Start<Start<mytime>>{},v(0.0,second{})),
      x_i(Step<Start<mytime>>{}, v(100.0,second{})),
      x_i(Duration<Start<mytime>>{}, v(200.0,second{})),
      x_i(Duration<mytime>{}, v(1.0,second{})),
      x_i(Step<mytime>{}, v(0.5,second{})),
      x_i(mean<velocity>{}, v(1.0,meters_per_second{})),
      x_i(mean<asceleration>{}, v(0.01,meters_per_second2{})),
      x_i(stddev<asceleration>{}, v(0.01,meters_per_second2{})),

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
      x_i(mean<mean<position>>{}, v(1.0,meter{})),
      x_i(stddev<mean<position>>{}, v(0.2,meter{})),
      x_i(mean<stddev<position>>{}, v(0.2,meter{})),
      x_i(mean<stddev<velocity>>{}, v(1.0,meters_per_second{}))
  };


  typedef Cs<position> data_fields;
  typedef Cs<stddev<velocity>,stddev<position>,velocity,asceleration> param_fields;

  //typedef typename decltype (typename mytime::unit{}* typename velocity::unit{})::dge de;


  {
    std::cerr<<"-------------new stuff--------- \n";
    std::mt19937 mt(initseed);

    auto s=sample(qui,vector_space<>(),mt);
    auto par=s| myselect<param_fields>{};
    auto dpar=Self_Derivative(par);
    auto data=s| myselect<data_fields>{};


    //std::cerr << "parameters \n"<<par <<std::endl;
    //std::cerr << "dparameters \n"<<dpar <<std::endl;

    //std::cerr << "data \n"<<data <<std::endl;

    auto logL=logP(qui,s);
    auto dlogL=vector_space(logP(qui,data,dpar));


    auto fim=FIM(qui,data,dpar);
    std::cerr <<"logL"<< logL<<std::endl;
    std::cerr << "dlogL"<< dlogL<<std::endl;
    std::cerr<<"FIM "<<fim<<"\n";
    std::string fname="out.txt";
    std::ofstream f(fname.c_str());
    auto dpar_new=decltype (dpar){};
    auto fim_new=decltype(fim){};
    auto dlogL_new=decltype(dlogL){};
   // to_DataFrame(f,s);
    to_DataFrame(f,dlogL);
    f.close();

    auto s_new=decltype (s){};
    std::ifstream fe;
    fe.open(fname.c_str());
    if (fe.is_open())
    {
      from_DataFrame(fe,dlogL_new);
//      from_DataFrame(fe,fim_new);
 //     from_DataFrame(fe,s_new);

    }
    std::cerr<<"\ndlogL\n"<<dlogL;
    std::cerr<<"\n dlogL_new\n"<<dlogL_new;


    assert(dlogL==dlogL_new);

   // std::cerr<<"\ns\n"<<s;
   // std::cerr<<"\n S_new\n"<<s_new;


   // assert(s==s_new);

  }
  return 0;
}
