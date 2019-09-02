#ifndef QM_DISTRIBUTION_H
#define QM_DISTRIBUTION_H
#include <iomanip>

#include "qm_data.h"
#include <random>

//namespace qm {
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
    auto out=consolidate<Id,value_type>(vec<>{},par[Xs{}]...);
      auto p=out.begin();
      do {
        out(p)=g_.sample(par[Xs{}](p)...,mt);

      } while (out.next(p));
      return out;
   }
   template<class Param>
   auto logP(const Param& par)const
   {
     auto& x=par[Id{}];
     auto p=x.begin();
     auto logProb=g_.logP(x(p),par[Xs{}](p)...);
     while (x.next(p))
       logProb=std::move(logProb)+g_.logP(x(p),par[Xs{}](p)...);
     return logProb;
   }



  D(Id id,Distribution&& g, Xs&&...):g_{std::move(g)}{}
};



template<class Id,class Distribution, class... Xs,class Rnd, class Datas>
auto sample(const D<Id,Distribution,Xs...>& dist,const Datas& d,Rnd& mt)
{
   if constexpr ((std::is_same_v<decltype (d[Xs{}]),Nothing>||...))
      return Nothing{};
    else
  return dist.sample(d,mt);
}

template<class Id,class Distribution, class... Xs, class Datas>
auto logP(const D<Id,Distribution,Xs...>& dist,const Datas& d)
{
  if constexpr ((std::is_same_v<decltype (d.at(Xs{})),Nothing>||...))
    return Nothing{};
  else
    return dist.logP(d);
}






//} // namespace qm

#endif // QM_DISTRIBUTION_H
