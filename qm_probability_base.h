#ifndef QM_PROBABILITY_BASE_H
#define QM_PROBABILITY_BASE_H

#include <qm_tensor_base.h>

template <class...> struct pr;

template <class ...Ups, class ...Downs
          >
          //, typename=std::enable_if_t<(sizeof... (Ups)==sizeof... (Downs))&&(sizeof... (Ups)>0),int>>
struct pr<up<Ups...>,dn<Downs...>>{
  constexpr static auto  className=my_static_string("p(")+((Ups::className+my_static_string("&"))+...)+my_static_string("|")
                                    +((Downs::className+my_static_string("&"))+...) + my_static_string(")");


  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...>, dn<iDowns...>){return ein<pr,up<iUps...>,dn<iDowns...>>{};}

  using T=double;
  using unit=p<>;

  /// multiplication of independent probabilities...
  template <class ...Ups2, class ...Downs2>
  friend
      auto operator*(pr,pr<up<Ups2...>,dn<Downs2...>>){
    return pr<up<Ups...,Ups2...>,dn<Downs...,Downs2...>>{};
  }

  friend constexpr bool operator==(const pr& , const pr& ){return true;}

};
template<class > struct pr_transfer;

template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0>
struct pr_transfer<
    ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>>
{
  using type=ein<
      pr<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>;
};


template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>

constexpr auto operator * (ein<pr<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         ein<pr<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return typename pr_transfer<
      decltype (
          ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>{}*
          ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>{})>::type{};
}



template <class...> struct logpr;

template <class ...Ups, class ...Downs
          >
//, typename=std::enable_if_t<(sizeof... (Ups)==sizeof... (Downs))&&(sizeof... (Ups)>0),int>>
struct logpr<up<Ups...>,dn<Downs...>>{
  constexpr static auto  classNameUp=((Ups::className+my_static_string("&"))+...);
  constexpr static auto  classNameDown=((Downs::className+my_static_string("&"))+...);

  constexpr static auto  className=my_static_string("logp(")+classNameUp+my_static_string("|")+classNameDown+ my_static_string(")");

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...>, dn<iDowns...>){return ein<logpr,up<iUps...>,dn<iDowns...>>{};}

  using T=decltype ((typename Ups::T{}*...)/(typename Downs::T{}*...*Ide{}));
 // using unit=decltype ((typename Ups::unit{}*...*p<>{})/(typename Downs::unit{}*...*Ide{}));

  /// multiplication of independent probabilities...
  template <class ...Ups2, class ...Downs2>
  friend
      constexpr auto operator+(logpr,logpr<up<Ups2...>,dn<Downs2...>>){
    return logpr<up<Ups...,Ups2...>,dn<Downs...,Downs2...>>{};
  }

  friend constexpr bool operator==(const logpr& , const logpr& ){return true;}

};
template<class > struct logpr_transfer;

template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0>
struct logpr_transfer<
    ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>>
{
  using type=ein<
      logpr<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>;
};


template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>

constexpr auto operator + (ein<logpr<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         ein<logpr<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return typename logpr_transfer<decltype (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>{}*
                              ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>{})>::type{};
}







#endif // QM_PROBABILITY_BASE_H
