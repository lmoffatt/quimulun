#ifndef QM_MODELS_H
#define QM_MODELS_H
#include "qm_distribution.h"
#include <functional>
#include <type_traits>
namespace qm {



template<class Id,class G, class... Xs>
class  F
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class Param>
  auto operator()(const Param& par)const
  {
    typedef  decltype(std::invoke(g_,std::declval<typename std::decay_t<decltype(par[Xs{}])>::element_type>()...)) value_type;

    auto out=consolidate<Id,value_type>(vec<>{},par[Xs{}]...);
    auto p=out.begin();
    do {
      out(p)=std::invoke(g_,par[Xs{}](p).value()...);

    } while (out.next(p));
    return out;
  }
  F(Id id,G&& g, Xs&&...):g_{std::move(g)}{}
};

template<class Id,class G, class... Xs,class Rnd, class Datas>
auto sample(const F<Id,G,Xs...>& f,const Datas& d,Rnd& )
{
  if constexpr ((std::is_same_v<decltype (d[Xs{}]),Nothing>||...))
    return Nothing{};
  else
    return f(d);
}

template<class Id,class G, class... Xs,class Datas>
auto logP(const F<Id,G,Xs...>& f,const Datas& d )
{
  return logP_zero{};
}




template<class Id, class G, class...Xs> F(Id&&,G&&,Xs&&...)->F<Id,G,Xs...>;


template<class,class... > struct quimulun;

template<class Id, class...Fs > struct  quimulun: public Fs...{
  using Fs::operator[]...;
  auto& operator[](Id)const {return *this;}
  typedef Id myId;
  quimulun(Id,Fs&&...f):Fs{std::move(f)}...{}

};




template<class Id, class...Fs >   quimulun(Id,Fs...) -> quimulun<Id,Fs...>;

template<class... Ids> struct select{};

template<class Id, class...Fs, class ...Ids >
auto operator | (quimulun<Id,Fs...> q, select<Ids...>)
{
  return quimulun(Id{},std::move(q)[Ids{}]...);
}

template<class Id,class... Ds, class...Ids>
auto operator | (Data<Id,Ds...> d, select<Ids...>)
{
  return (Data<Id>{}+...+(std::move(d)[Ids{}]));
}

template<class Id, class...Fs, class Id2, class...Fs2>
    auto operator + (quimulun<Id,Fs...> q1,quimulun<Id2,Fs2...> q2)
    ->std::enable_if_t<!(is_in_pack(typename Fs2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Id,Fs...,Fs2...>>
{
  return quimulun(Id{},std::move(q1)[typename Fs::myId{}]...,std::move(q2)[typename Fs2::myId{}]...);
}


template<class Id, class...Fs, class Id2, class...Fs2>
auto operator << (quimulun<Id,Fs...> q1,quimulun<Id2,Fs2...> q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,select<>> non_common_Ids ;
  return (q1|non_common_Ids{})+q2;
}

template<class Id, class...Fs, class Id2, class...Fs2>
auto operator << (Data<Id,Fs...> q1,Data<Id2,Fs2...> q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,select<>> non_common_Ids ;
  return (std::move(q1)|non_common_Ids{})+std::move(q2);
}


template <class...Ids,class...Ids2>
constexpr const bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}





template < class Fid, class... Fs,class Id,class Id2,class...Ds2, class Random>
auto sample(const quimulun<Fid,Fs...>& qui,Id,const Data<Id2,Ds2...>& d, Random& mt)
{
   return sample(qui[Id{}],d,mt);
}



template <class... Ids_Not_Reachead>
constexpr void check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
}

template <class Id, class Fid,class...F,class...Ds2,  class... Fields,class Random>
auto sample(const quimulun<Fid,F...>& qui,const Data<Id,Ds2...>& d, Cs<Fields...>,Random& mt)
{
  return (Is_Complete<true,Data<Id>>()|...|sample(qui,Fields{},d,mt));

}


template <class Id, class Fid,class...F,class...Ds2, class Random>
auto sample(const quimulun<Fid,F...>& qui,Data<Id,Ds2...>&& d, Random& mt)
{
  auto Fields=pack_difference_t<Cs<typename F::myId...>,Cs<typename Ds2::myId...>>{};
  auto s=sample(qui,d,Fields,mt);
  //typedef typename decltype (s)::sample sample_type;
  if constexpr ( std::is_same_v<typename decltype(s)::type,Data<Id>>)
  {
     check_if_reacheable(Fields);
     return false;
  }
  if constexpr (s.is_complete)
    return std::move(d)+std::move(s.value);
  else
    return sample(qui,std::move(d)+std::move(s.value),mt);
}


template <class Id, class Fid,class...F,class...Ds2>
auto logP(const quimulun<Fid,F...>& qui,const Data<Id,Ds2...>& d)
{
  auto s=(logP(qui[typename Ds2::myId{}],d)+...);
    return s;
}








} // namespace qm


#endif // QM_MODELS_H
