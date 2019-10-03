#ifndef QM_MODELS_H
#define QM_MODELS_H
#include "qm_distribution.h"
#include <functional>
#include <type_traits>
//namespace qm {



template<class Id,class G, class... Xs>
class  F
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    typedef  decltype(std::invoke(g_,std::declval<typename std::decay_t<decltype(get_from<Xs>(par...))>::element_type>()...)) value_type;

    auto out=consolidate<Id,value_type>(vec<>{},get_from<Xs>(par...)...);
    auto p=out.begin();
    do {
      out(p)=std::invoke(g_,get_from<Xs>(par...)(p)...);

    } while (out.next(p));
    return out;
  }
  F(Id id,G&& g, Xs&&...):g_{std::move(g)}{}
};

template<class Id,class G, class... Xs,class... Datas>
auto calculate(const F<Id,G,Xs...>& f,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from<Xs>(d...)),Nothing>||...))
    return Nothing{};
  else
    return f(d...);
}

template<class Id,class G, class... Xs,class Rnd, class Datas>
auto sample(const F<Id,G,Xs...>& f,const Datas& d,Rnd& )
{
  return calculate(f,d);
}



template<class Id,class G, class... Xs,class... Datas>
auto logP(const F<Id,G,Xs...>& ,const Datas&... )
{
  return logP_zero{};
}

template<class Id,class G, class... Xs,class... Datas>
auto FIM(const F<Id,G,Xs...>& ,const Datas&...)
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

template<class... Ids> struct myselect{};

template<class Id, class...Fs, class ...Ids >
auto operator | (quimulun<Id,Fs...> q, myselect<Cs<Ids...>>)
{
  return std::move(q)|myselect<Ids...>{};
}
template<class Id, class...Fs, class ...Ids >
auto operator | (quimulun<Id,Fs...> q, myselect<Ids...>)
{
  return quimulun(Id{},std::move(q)[Ids{}]...);
}

template<class Id,class... Ds, class...Ids>
auto operator | (Data<Id,Ds...> d, myselect<Ids...>)
{
  return (Data<Id>{}+...+(std::move(d)[Ids{}]));
}
template<class Id,class... Ds, class...Ids>
auto operator | (Data<Id,Ds...> d, myselect<Cs<Ids...>>)
{
  return std::move(d)|myselect<Ids...>{};
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
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,myselect<>> non_common_Ids ;
  return (q1|non_common_Ids{})+q2;
}

template<class Id, class...Fs, class Id2, class...Fs2>
auto operator << (Data<Id,Fs...> q1,Data<Id2,Fs2...> q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,myselect<>> non_common_Ids ;
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


template < class Fid, class... Fs,class Id0,class...Datas>
auto calculate(const quimulun<Fid,Fs...>& qui,Id0,const Datas&... data)
{
  return calculate(qui[Id0{}],data...);
}


template <class Fid,class...F,class... Fields,class Id1,class...Ds1,class...Datas>
auto calculate(const quimulun<Fid,F...>& qui, Cs<Fields...>,const Data<Id1,Ds1...>& variables, const Datas&... parameters)
{
  return (Is_Complete<true,Data<Id1>>()|...|calculate(qui,Fields{},variables, parameters...));

}


template <class Fid,class...F,class...Datas,class Id2,class...Ds2 >
auto calculate(const quimulun<Fid,F...>& qui,Data<Id2,Ds2...>&& variables,const Datas&...param )
{
  auto Fields=pack_difference_t<Cs<typename F::myId...>,pack_concatenation_t<Cs<typename Ds2::myId...>,typename Datas::myIds...>>{};
  auto s=calculate(qui,Fields,variables,param...);
  //typedef typename decltype (s)::sample sample_type;
  if constexpr ( std::is_same_v<decltype(s.value),Data<Id2>>&& ! std::is_same_v<Cs<>,decltype(Fields) >)
  {
    check_if_reacheable(Fields);
    return false;
  }
  else if constexpr (s.is_complete)
    return std::move(variables)+std::move(s.value);
  else
    return calculate(qui,std::move(variables)+std::move(s.value),param...);
}




template <class Fid,class...F,class...Datas>
auto logP(const quimulun<Fid,F...>& qui,const Datas&... d)
{
  auto ds=calculate(qui,Data<Fid>{},d...);
  auto s=(logP(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class Fid,class...F,class...Datas>
auto FIM(const quimulun<Fid,F...>& qui,const Datas&... d)
{
  auto ds=calculate(qui,Data<Fid>{},d...);
  auto s=(FIM(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class Id, class Fid,class...F,class...Ds2, class... Ds3>
auto MAP(const quimulun<Fid,F...>& qui,const Data<Id,Ds2...>& data, const Data<Id,Ds2...>& parameters )
{
  auto dpar=Self_Derivative(parameters);

  auto logL=logP(qui,data,dpar);
  auto fim=FIM(qui,data,dpar);





}











//} // namespace qm


#endif // QM_MODELS_H
