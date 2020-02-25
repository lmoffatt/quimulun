#ifndef QM_TENSOR_DISTRIBUTION_H
#define QM_TENSOR_DISTRIBUTION_H


#include <iomanip>

#include "qm_tensor_derivative.h"
#include "qm_tensor_coordinate.h"
#include "qm_probability_base.h"
#include <random>
#include <my_math.h>



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
  //typedef typename Id::T T;
  // typedef typename Id::unit unit;
  constexpr static auto  className=Id::className+my_static_string("_variance");
};



template <class ...Ups, class ...Downs
          , class value_type0, class value_type2>
//, typename=std::enable_if_t<(sizeof... (Ups)==sizeof... (Downs))&&(sizeof... (Ups)>0),int>>
auto operator - (const x_i<logpr<up<Ups...>,dn<Downs...>>, value_type0>& one,
               const x_i<logpr<up<Ups...>,dn<Downs...>>, value_type2>& two)
{
  return one()-two();
}





struct Normal_Distribution
{
  template<class unit,class Rnd>
  auto sample(const v<double,unit>& mean, const v<double,unit>& stddev,Rnd& mt)const
  {

    return v<double,unit>(std::normal_distribution<double>
                           {mean.value(),stddev.value()}(mt.value()));
  }
  template<class unit,class Rnd>
  auto sample(const logv<double,unit>& mean, const v<double,dimension_less>& stddev,Rnd& mt)const
  {

    return logv(std::normal_distribution<double>
                {mean.value(),stddev.value()}(mt.value()),unit{});
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
           0.5 * sqr((x - mean) / stddev);
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



class stretch_move_Distribution  {

public:
   constexpr static auto const className =
      my_static_string("stretch_move_Distribution");
  typedef stretch_move_Distribution self_type;

  /// parameter alfa between 1 an infinity, 2 is good.

  template<class Rnd>
  auto sample(double alfa, Rnd& mt)const
  {
    std::uniform_real_distribution<double> U(std::sqrt(1.0 / alfa),
                                             std::sqrt(alfa));
    return  sqr(U(mt));
   }

  auto logP(double x,double /*alfa*/, double logZ)const
  {
    return -0.5*std::log(x)-logZ;
  }
  auto logP(double x,double alfa)const
  {
    double logZ=std::log(2.0)+std::log(alfa-1.0)-0.5*std::log(alfa);
    return logP(x,alfa,logZ);
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

  template<class... Param,class xi_Rnd>
  auto sample(xi_Rnd& mt,const Param&... par)const
  {
    auto  out=apply_sample(g_,mt,get_from(Xs{},par...)...);

  /*  return x_i(Id{},std::move(out));
        using value_type=  decltype(g_.sample(get_from(Xs{},par...)(get_from(Xs{},par...).begin())...,mt(mt.begin())));
    auto out=consolidate<value_type>(vec<>{},par[Xs{}]...);
    auto p=out.begin();



    do {
      out(p)=g_.sample(par[Xs{}](p)...,mt);

    } while (out.next(p));*/
    //using test2=typename decltype(out)::out_type;
   // using test3= typename decltype(x_i(Id{},std::move(out)))::XI_out_type;

    return x_i(Id{},std::move(out));
  }
  template<class... Param>
  auto logP(const Param&... par)const
  {
    auto const& x=get_from(Id{},par...);
    auto p=x.begin();
    auto logProb=g_.logP(x(p),get_from(Xs{},par...)(p)...);
    while (x.next(p))
      logProb=std::move(logProb)+g_.logP(x(p),get_from(Xs{},par...)(p)...);
    return x_i(logpr<up<Id>,dn<Xs...>>{},std::move(logProb));
  }
  template<class... Param>
  auto FIM(const Param&... par)const
  {
    auto const& x=get_from(Id{},par...);
    auto p=x().begin();
    auto fim=g_.FIM(x(p),get_from(Xs{},par...)(p)...);
    while (x().next(p))
      fim=std::move(fim)+g_.FIM(x(p),get_from(Xs{},par...)(p)...);
    return fim;
  }


  D(Id ,Distribution&& g, Xs&&...):g_{std::move(g)}{}
};

template <class> struct extract_distribution_Id{using type=Cs<>;};

template< class Id,class Distribution, class... Xs>
struct extract_distribution_Id<D<Id,Distribution,Xs...>>{using type=Cs<Id>;};

template<class C> using extract_distribution_Id_t=typename extract_distribution_Id<C>::type;

template< class... Ids>
struct extract_distribution_Id<Cs<Ids...>>{using type=pack_concatenation_t<extract_distribution_Id_t<Ids>...>;};


template<class... Ids,class Distribution, class... Xs>
class  D<Cs<Ids...>,Distribution,Xs...>: public D<Ids,Distribution,Xs...>...
{

public:
  using D<Ids,Distribution,Xs...>::operator[]...;
  D(Cs<Ids...> ,Distribution&& g, Xs&&...):
                                              D<Ids,Distribution,Xs...>{Ids{},Distribution{g},Xs{}...}...{}
};



template<class Id,class Distribution, class... Xs,class Rnd, class...Datas>
auto sample(const D<Id,Distribution,Xs...>& dist,Rnd& mt, const Datas&...ds)
{
 //using test2=typename Id::Id_test;
 //using tests=typename Cs<Datas...>::Datas_test;
// using testX=typename Cs<Xs...>::Xs_test;

  if constexpr ((std::is_same_v<decltype (get_from(Xs{},ds...)),Nothing>||...))
    return Nothing{};
  else
    return dist.sample(mt,ds...);
}


template<class Id,class Distribution, class... Xs, class... Datas>
auto logP(const D<Id,Distribution,Xs...>& dist,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from(Xs{},d...)),Nothing>||...))
    return Nothing{};
  else
    return dist.logP(d...);
}

template<class Id,class Distribution, class... Xs, class... Datas>
auto FIM(const D<Id,Distribution,Xs...>& dist,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from(Xs{},d...)),Nothing>||...))
    return Nothing{};
  else
    return dist.FIM(d...);
}







#endif // QM_TENSOR_DISTRIBUTION_H
