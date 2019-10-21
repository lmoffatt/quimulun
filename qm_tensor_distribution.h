#ifndef QM_TENSOR_DISTRIBUTION_H
#define QM_TENSOR_DISTRIBUTION_H


#include <iomanip>

#include "qm_tensor_derivative.h"
#include "qm_tensor_coordinate.h"
//#include "qm_derivative.h"
#include <random>

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





struct Normal_Distribution
{
  template<class unit,class Rnd>
  auto sample(const v<double,unit>& mean, const v<double,unit>& stddev,Rnd& mt)const
  {

    return v<double,unit>(std::normal_distribution<double>
                           {mean.value(),stddev.value()}(mt));
  }

  template<class unit>
  auto logP(const v<double,unit>& x,const v<double,unit>& mean, const v<double,unit>& stddev)const
  {
    return -0.5 * std::log(2 * PI) -  log(stddev) -
           v(0.5) * sqr((x - mean) / stddev);
  }
  template<class vx, class vm,class vs>
  auto logP(const vx& x,const vm& mean, const vs& stddev)const
  {
    return -0.5 * std::log(2 * PI) -  log(stddev) -
           v(0.5) * sqr((x - mean) / stddev);
  }

  template<class derVar,class derMean, class derStd>
  auto FIM(const derVar& var ,const derMean& mean ,const derStd& stddev )const
  {

    auto dvar= Df(var)/center(stddev);
    auto dmean= Df(mean)/center(stddev);
    auto dstd=Df(stddev)/center(stddev);
 //   typedef typename decltype (dvar)::se ge;
   // typedef typename decltype (dvar(up<Iu_>{},dn<Id_>{})*dvar(up<Ju_>{},dn<Jd_>{}))::se ge;
    return dvar*dvar+dmean*dmean+2.0*dstd*dstd;
  }

};




struct Exponential_Distribution

{
  template<class unit,class Rnd>
  auto sample(const v<double,unit>& mean, Rnd& mt)const
  {
    return v<double,unit>(std::exponential_distribution<double>
                           {1.0/center(mean).value()}(mt));
  }

  template<class unit>
  auto logP(const v<double,unit>& x,const v<double,unit>& mean)const
  {     return -log(mean) -(x/ mean);
  }
  template<class vx, class vm>
  auto logP(const vx& x,const vm& mean)const
  {     return -log(mean) -(x/ mean);
  }

  template<class derVar,class derMean>
  auto FIM(const derVar& var,const derMean& mean )const
  {
    auto dvar= Df(var)/center(mean);
    auto dmean= Df(mean)/center(mean);
    return (dvar*dvar)+
           (dmean*dmean);
  }


};


template<class Id,class Distribution, class... Xs>
class  D
{
private:
  Distribution g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const & {return *this;}
  auto &operator[](Id) & {return *this;}
  auto operator[](Id)&& {return *this;}

  template<class Param,class Rnd>
  auto sample(const Param& par,Rnd& mt)const
  {
    typedef  decltype(g_.sample(std::declval<typename std::decay_t<decltype(par[Xs{}])>::element_type>()...,mt)) value_type;
    auto out=consolidate<value_type>(vec<>{},par[Xs{}]...);


    auto p=out.begin();
    do {
      out(p)=g_.sample(par[Xs{}](p)...,mt);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
  }
  template<class... Param>
  auto logP(const Param&... par)const
  {
    auto const& x=get_from<Id>(par...);
    auto p=x().begin();
    auto logProb=g_.logP(x()(p),get_from<Xs>(par...)(p)...);
    while (x().next(p))
      logProb=std::move(logProb)+g_.logP(x()(p),get_from<Xs>(par...)(p)...);
    return logProb;
  }
  template<class... Param>
  auto FIM(const Param&... par)const
  {
    auto const& x=get_from<Id>(par...);
    auto p=x().begin();
    auto fim=g_.FIM(x(p),get_from<Xs>(par...)(p)...);
    while (x().next(p))
      fim=std::move(fim)+g_.FIM(x(p),get_from<Xs>(par...)(p)...);
    return fim;
  }


  D(Id ,Distribution&& g, Xs&&...):g_{std::move(g)}{}
};



template<class Id,class Distribution, class... Xs,class Rnd, class Datas>
auto sample(const D<Id,Distribution,Xs...>& dist,const Datas& d,Rnd& mt)
{
  if constexpr ((std::is_same_v<decltype (d[Xs{}]),Nothing>||...))
    return Nothing{};
  else
    return dist.sample(d,mt);
}
template<class Id,class Distribution, class... Xs,class... Datas>
auto calculate(const D<Id,Distribution,Xs...>& ,const Datas&... d )
{
  return get_from<Id>(d...);
}



template<class Id,class Distribution, class... Xs, class... Datas>
auto logP(const D<Id,Distribution,Xs...>& dist,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from<Xs>(d...)),Nothing>||...))
    return Nothing{};
  else
    return dist.logP(d...);
}

template<class Id,class Distribution, class... Xs, class... Datas>
auto FIM(const D<Id,Distribution,Xs...>& dist,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from<Xs>(d...)),Nothing>||...))
    return Nothing{};
  else
    return dist.FIM(d...);
}







#endif // QM_TENSOR_DISTRIBUTION_H
