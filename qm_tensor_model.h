#ifndef QM_TENSOR_MODEL_H
#define QM_TENSOR_MODEL_H
#include "qm_tensor_distribution.h"
#include <functional>
#include <type_traits>

namespace ten {
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

    auto out=consolidate<value_type>(vec<>{},get_from<Xs>(par...)...);
    auto p=out.begin();
    do {
      out(p)=std::invoke(g_,get_from<Xs>(par...)(p)...);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
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

template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...> d, myselect<Ids...>)
{
  return (vector_space<>{}+...+(std::move(d)[Ids{}]));
}
template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...> d, myselect<Cs<Ids...>>)
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

template<class...Fs, class...Fs2>
auto operator << (vector_space<Fs...> q1,vector_space<Fs2...> q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,myselect<>> non_common_Ids ;
  return (std::move(q1)|non_common_Ids{})+std::move(q2);
}


template <class...Ids,class...Ids2>
constexpr const bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}





template < class Fid, class... Fs,class Id,class...Ds2, class Random>
auto sample(const quimulun<Fid,Fs...>& qui,Id,const vector_space<Ds2...>& d, Random& mt)
{
  return sample(qui[Id{}],d,mt);
}


template <class... Ids_Not_Reachead>
constexpr void check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
}

template <class Fid,class...F,class...Ds2,  class... Fields,class Random>
auto sample(const quimulun<Fid,F...>& qui,const vector_space<Ds2...>& d, Cs<Fields...>,Random& mt)
{
  return (Is_Complete<true,vector_space<>>()|...|sample(qui,Fields{},d,mt));
}


template <class Fid,class...F,class...Ds2, class Random>
auto sample(const quimulun<Fid,F...>& qui,vector_space<Ds2...>&& d, Random& mt)
{
  auto Fields=pack_difference_t<Cs<typename F::myId...>,Cs<typename Ds2::myId...>>{};
  auto s=sample(qui,d,Fields,mt);
  //typedef typename decltype (s)::sample sample_type;
  if constexpr ( std::is_same_v<typename decltype(s)::type,vector_space<>>)
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


template <class Fid,class...F,class... Fields,class...Ds1,class...Datas>
auto calculate(const quimulun<Fid,F...>& qui, Cs<Fields...>,const vector_space<Ds1...>& variables, const Datas&... parameters)
{
  return (Is_Complete<true,vector_space<>>()|...|calculate(qui,Fields{},variables, parameters...));

}


template <class Fid,class...F,class...Datas,class...Ds2 >
auto calculate(const quimulun<Fid,F...>& qui,vector_space<Ds2...>&& variables,const Datas&...param )
{
  auto Fields=pack_difference_t<Cs<typename F::myId...>,pack_concatenation_t<Cs<typename Ds2::myId...>,typename Datas::myIds...>>{};
  //typedef typename Cs<typename Datas::myIds...>::sample Ds2_ltype;
 // typedef typename Cs<typename F::myId...>::sample sample_ltype;
  auto s=calculate(qui,Fields,variables,param...);
 // typedef typename decltype (s)::sample sample_type;
  if constexpr ( std::is_same_v<decltype(s.value),vector_space<>>&& ! std::is_same_v<Cs<>,decltype(Fields) >)
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
  auto ds=calculate(qui,vector_space<>{},d...);
  auto s=(logP(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class Fid,class...F,class...Datas>
auto FIM(const quimulun<Fid,F...>& qui,const Datas&... d)
{
  auto ds=calculate(qui,vector_space<>{},d...);
  auto s=(FIM(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class Fid,class...F,class...Ds2, class... Ds3>
auto MAP(const quimulun<Fid,F...>& qui,const vector_space<Ds2...>& data, const vector_space<Ds2...>& parameters )
{
  auto dpar=Self_Derivative(parameters);

  auto logL=logP(qui,data,dpar);
  auto fim=FIM(qui,data,dpar);





}











//} // namespace qm



} // namespace ten

#endif // QM_TENSOR_MODEL_H
