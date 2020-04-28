#ifndef QM_TENSOR_DISTRIBUTION_EVALUATION_H
#define QM_TENSOR_DISTRIBUTION_EVALUATION_H
#include <qm_tensor_distribution_declaration.h>

template<class Rnd>
auto myInvoke(Sample, Bernoulli_Distribution& d, Rnd& mt)
{

  return v<bool,dimension_less>(d.d()(mt.value()));
}

template<class unit,class Rnd>
auto myInvoke(Sample, Bernoulli_Distribution& , Rnd& mt,const v<double,unit>& p)
{

  return v(Bernoulli_Distribution::distribution_type{p.value()}(mt.value()),dimension_less{});
}
template<class unit,class Rnd>
auto myInvoke(Sample, Bernoulli_Distribution& , Rnd& mt,const logv<double,unit>& p)
{
  return v(Bernoulli_Distribution::distribution_type{std::exp(p.value())}(mt.value()),dimension_less{});
}


template<class unit>
auto myInvoke(logProbability, Bernoulli_Distribution,bool x,const logv<double,unit>& p)
{
  if (x) return p;
  else return log(1.0-std::exp(p));
}
template<class unit>
auto myInvoke(logProbability,  Bernoulli_Distribution,bool x,const v<double,unit>& p)
{
  if (x) return log(p);
  else return log(1.0-p);
}


template<class Int,class unit,class Rnd>
auto myInvoke(Sample, Uniform_Integer_Distribution<Int>& d,Rnd& mt,const v<Int,unit>& min, const v<Int,unit>& max)
{
  return v<Int,unit>(d.d()(mt.value(),min.value(),max.value()));
}

template<class Int,class unit,class Rnd>
auto myInvoke(Sample, Uniform_Integer_Distribution<Int>,Rnd& mt, const v<Int,unit>& max)
{
  return v<Int,unit>(typename Uniform_Integer_Distribution<Int>::distribution_type{Int{0},max.value()}(mt.value()));
}



template<class Int,class unit>
auto myInvoke(logProbability, Uniform_Integer_Distribution<Int>,const v<Int,unit>&, const v<Int,unit>& min, const v<Int,unit>& max)
{
  return log(v<double,dimension_less>(1.0)/(max-min+1));
}

template<class Int,class unit,class Rnd>
auto myInvoke(Sample, Uniform_Index_Distribution<Int>& d,Rnd& mt,const v<Int,unit>& min, const v<Int,unit>& max)
{
  return v<Int,unit>(d.d()(mt.value(),min.value(),max.value()-1));
}

template<class Int,class unit,class Rnd>
auto myInvoke(Sample, Uniform_Index_Distribution<Int>,Rnd& mt, const v<Int,unit>& max)
{
  return v<Int,unit>(typename Uniform_Integer_Distribution<Int>::distribution_type{Int{0},max.value()}(mt.value())-1);
}



template<class Int,class unit>
auto myInvoke(logProbability, Uniform_Index_Distribution<Int>,const v<Int,unit>&, const v<Int,unit>& min, const v<Int,unit>& max)
{
  return log(v<double,dimension_less>(1.0)/(max-min));
}




template<class Real,class unit,class Rnd>
auto myInvoke(Sample, Normal_Distribution<Real>& d,Rnd& mt, const v<double,unit>& mean, const v<double,unit>& stddev)
{

  return v<double,unit>(d.d()(mt.value())*stddev.value()+mean.value());
}

template<class Real,class unit,class Rnd>
auto myInvoke(Sample, Normal_Distribution<Real>& d,Rnd& mt, const logv<double,unit>& mean, const v<double,dimension_less>& stddev)
{
  double out=d.d()(mt.value())*stddev.value()+mean.value();
  return logv(std::move(out),unit{});
}


template<class Real,class unit>
auto myInvoke(logProbability, const Normal_Distribution<Real>& ,const v<double,unit>& x,const v<double,unit>& mean, const v<double,unit>& stddev)
{
  return -0.5 * std::log(2 * PI) -  log(stddev) -
         v(0.5) * sqr((x - mean) / stddev);
}
template<class Real,class vx, class vm,class vs>
auto myInvoke(logProbability, const Normal_Distribution<Real>& ,const vx& x,const vm& mean, const vs& stddev)
{


  return -0.5 * std::log(2 * PI) -  log(stddev) -
         0.5 * sqr((x - mean) / stddev);
}

template<class Real,class derVar,class derMean, class derStd>
auto myInvoke(FisherInformationMatrix, const Normal_Distribution<Real>& ,const derVar& var ,const derMean& mean ,const derStd& stddev )
{

  auto dvar= Df(var)/center(stddev);
  auto dmean= Df(mean)/center(stddev);
  auto dstd=Df(stddev)/center(stddev);
  //   typedef typename decltype (dvar)::se ge;
  // typedef typename decltype (dvar(up<Iu_>{},dn<Id_>{})*dvar(up<Ju_>{},dn<Jd_>{}))::se ge;
  return dvar*dvar+dmean*dmean+2.0*dstd*dstd;
}





template<class Real,class unit,class Rnd>
auto myInvoke(Sample, Exponential_Distribution<Real>& d,Rnd& mt, const v<double,unit>& mean)
{
  return v<double,unit>(d.d()(mt)/center(mean).value());
}

template<class Real,class unit>
auto myInvoke(logProbability, Exponential_Distribution<Real>& ,const v<double,unit>& x,const v<double,unit>& mean)
{     return -log(mean) -(x/ mean);
}
template<class Real,class vx, class vm>
auto myInvoke(logProbability, Exponential_Distribution<Real>& ,const vx& x,const vm& mean)
{     return -log(mean) -(x/ mean);
}

template<class Real,class derVar,class derMean>
auto myInvoke(FisherInformationMatrix, Exponential_Distribution<Real>& ,const derVar& var,const derMean& mean )
{
  auto dvar= Df(var)/center(mean);
  auto dmean= Df(mean)/center(mean);
  return (dvar*dvar)+
         (dmean*dmean);
}




template<class Real,class Rnd>
auto myInvoke(Sample, stretch_move_Distribution<Real>, Rnd& mt,double alfa)
{
  std::uniform_real_distribution<double> U(std::sqrt(1.0 / alfa),
                                           std::sqrt(alfa));
  return  sqr(U(mt));
}
template<class Real,class Rnd>
auto myInvoke(Sample, stretch_move_Distribution<Real>, Rnd& mt,const v<Real,dimension_less>& alfa)
{
  std::uniform_real_distribution<double> U(std::sqrt(1.0 / alfa.value()),
                                           std::sqrt(alfa.value()));
  return  sqr(U(mt.value()));
}


template<class Real>
auto myInvoke(logProbability,stretch_move_Distribution<Real>,double x,double /*alfa*/, double logZ)
{
  return -0.5*std::log(x)-logZ;
}
template<class Real>
auto myInvoke(logProbability,stretch_move_Distribution<Real>,double x,double alfa)
{
  double logZ=std::log(2.0)+std::log(alfa-1.0)-0.5*std::log(alfa);
  return myInvoke(logProbability{},stretch_move_Distribution<Real>{},x,alfa,logZ);
}









template<class Id,class Distribution, class... Xs,class Rnd, class...Datas>
auto Invoke_on_this(Sample,const D<Id,Distribution,Arguments<Xs...>>& ,Rnd& mt, const Datas&...ds)
{
  auto res=(find_in(Xs{},ds...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=Distribution{};
    auto  out=apply_op(Sample{},d,mt,get_from(Xs{},ds...)...);
    return x_i(Id{},std::move(out));
  }

}

template<class Id,class Distribution, class... Xs,class... XXs,class Rnd, class...Datas>
auto Invoke_on_this(Sample,const D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>> ,Rnd& mt, const Datas&...ds)
{
  auto res=((find_in(Xs{},ds...)&&...)&&...&&find_in(XXs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=Distribution{};
    auto  out=apply_op(Sample{},d,mt,get_from(Xs{},ds...)...);
    return x_i(Id{},std::move(out));
  }

}



template<class Id,class Distribution, class... Xs,class...Datas>
auto Invoke_on_this(logProbability,const D<Id,Distribution,Arguments<Xs...>>& ,const Datas&...ds)
{
  auto res=(find_in(Id{},ds...)&&...&&find_in(Xs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto const& x=get_from(Id{},ds...);
    auto p=x.begin();
    auto logProb=myInvoke(logProbability{},Distribution{},x(p),get_from(Xs{},ds...)(p)...);
    while (x.next(p))
      logProb=std::move(logProb)+myInvoke(logProbability{},Distribution{},x(p),get_from(Xs{},ds...)(p)...);
    return x_i(logpr<up<Id>,dn<Xs...>>{},std::move(logProb));
  }

}




template<class Id,class Distribution, class... Xs, class... Datas>
auto Invoke_on_this(FisherInformationMatrix,const D<Id,Distribution,Xs...>& ,const Datas&... par)
{
  auto res=(find_in(Id{},par...)&&...&&find_in(Xs{},par...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else
  {
    auto const& x=get_from(Id{},par...);
    auto p=x().begin();
    auto fim=myInvoke(FisherInformationMatrix{},Distribution{},x(p),get_from(Xs{},par...)(p)...);
    while (x().next(p))
      fim=std::move(fim)+myInvoke(FisherInformationMatrix{},Distribution{},x(p),get_from(Xs{},par...)(p)...);
    return fim;
  }
}






#endif // QM_TENSOR_DISTRIBUTION_EVALUATION_H
