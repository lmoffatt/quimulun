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
    auto out=consolidate(Id{},vec<>{},par[Xs{}]...);
    if constexpr (out.numIndex>0)
    {
    auto p=out.begin();
    do {
      out(p)=std::invoke(g_,par[Xs{}](p).value()...);

    } while (out.next(p));
    return out;
    }
    else
    {
      return std::invoke(g_,par[Xs{}].value()...);
    }
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



template<class Id, class G, class...Xs> F(Id&&,G&&,Xs&&...)->F<Id,G,Xs...>;








template<class,class... > struct quimulun;

template<class Id, class...Fs > struct  quimulun: public Fs...{
  using Fs::operator[]...;
  auto& operator[](Id)const {return *this;}
  typedef Id myId;
  quimulun(Id,Fs&&...f):Fs{std::move(f)}...{}

};

template<class Id, class...Fs >   quimulun(Id,Fs...) -> quimulun<Id,Fs...>;


template <class...Ids,class...Ids2>
constexpr const bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}





//--------------


template <template<class...> class Tr, class Fid, class... Fs,class Id,class Id2,bool Complete,class...Ds2, class Random>
auto sample(const quimulun<Fid,Fs...>& qui,Id,Data_<Tr,Id2,Complete,Ds2...>&& d, Random& mt)
{
  if constexpr (is_in_pack<Id,Id2,typename Ds2::myId...>())
    return std::move(d)[Id{}];
  else
    return sample(qui[Id{}],d,mt);
}



template <class... Ids_Not_Reachead>
constexpr void check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
}




template <template<class...> class Tr,class Id, class Fid,class...F,bool Complete,class...Ds2, class Random>
auto sample(const quimulun<Fid,F...>& qui,Data_<Tr,Id,Complete,Ds2...>&& d, Random& mt)
{
  auto s=(Data_<Tr,Id,true>{}|...|sample(qui,typename F::myId{},std::move(d),mt));
  if constexpr ( std::is_same_v<decltype (s),Data_<Tr,Id,Complete,Ds2...>>)
  {
     typedef pack_difference_t<Cs<typename F::myId...>,Cs<typename Ds2::myId...>>  Ids_not_reached_yet;
     check_if_reacheable(Ids_not_reached_yet{});
     return false;
  }
  if constexpr (s.is_complete)
    return s;
  else
    return sample(qui,std::move(s),mt);
}
template <template<class...> class Tr,class Fid,class Id, class...Fs,class Random>
auto sample(const quimulun<Fid,Fs...>& qui, Id,Random& mt)
{
  return sample(qui,Data_<Tr,Id,true>(),mt);
}









} // namespace qm


#endif // QM_MODELS_H
