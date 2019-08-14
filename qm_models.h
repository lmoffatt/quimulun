#ifndef QM_MODELS_H
#define QM_MODELS_H
#include "qm_distribution.h"

namespace qm {


/*template <template<class...> class Tr,template<class...>class Q, class Id,class...Ids, class...Ids2, class Random>
 auto sample(const Q<Ids...>& qui,Id,Data_<Tr,Ids2...>&& d, Random& mt)
{
  if constexpr (is_in_pack<Id,Ids2...>())
    return std::move(d)(Id{});
  else
    return sample(qui(Id{}),d,mt);
}
template <template<class...> class Tr,template<class...>class Q, class Id,class...Ids, class...Ids2>
constexpr bool test(const Q<Ids...>& qui,const Data_<Tr,Ids2...>& d)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}

template <template<class...> class Tr,template<class...>class Q, class...Ids, class...Ids2, class Random>
auto sample(const Q<Ids...>& qui,Data_<Tr,Ids2...>&& d, Random& mt)
{
   auto s=(sample(qui,Ids{},std::move(d),mt)+...);
   if constexpr (test(qui,s))
     return s;
   else
     return sample(qui,std::move(s),mt);
}
template <template<class...> class Tr,template<class...>class Q, class...Ids, class Random>
auto sample(const Q<Ids...>& qui,Tr<double>, Random& mt)
{
  return sample(qui,Data_<Tr>(),mt);
}




template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
*/
template<class Id, class G> class F
{
private:
  G g_;
public:
  F(Id,G g):g_{g}{}
  auto& operator()(Id)const {return g_;}
 };

template<class Id, class G> F(Id,G)->F<Id,G>;


template<class,class... > struct quimulun;

template<class Id, class...Fs > struct  quimulun: public Fs...{
  using Fs::operator()...;
  auto& operator()(Id)const {return *this;}
  typedef Id myId;
  typedef Cs<typename Fs::myId...> myIds;
  quimulun(Id,Fs&&...f):Fs{std::move(f)}...{}

};

template<class Id, class...Fs >   quimulun(Id,Fs...) -> quimulun<Id,Fs...>;


template <template<class...> class Tr, class Fid,class... Fs,class Id,class...Ids2, class Random>
auto sample(const quimulun<Fid,Fs...>& qui,Id,Data_<Tr,Ids2...>&& d, Random& mt)
{
  if constexpr (is_in_pack<Id,Ids2...>())
    return std::move(d)(Id{});
  else
    return sample(qui(Id{}),d,mt);
}
template <class...Ids,class...Ids2>
constexpr const bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}

template <template<class...> class Tr,class Fid,class...F,class...Ids2, class Random>
Data_<Tr,typename F::myId...> sample(const quimulun<Fid,F...>& qui,Data_<Tr,Ids2...>&& d, Random& mt)
{
  auto s=(sample(qui,typename F::myId{},std::move(d),mt)+...);
  if constexpr (std::is_same_v<decltype (s),Data_<Tr,typename F::myId...> >)
    return s;
  else
    return sample(qui,std::move(s),mt);
}
template <template<class...> class Tr,class Id,class...Fs,class Random>
auto sample(const quimulun<Id,Fs...>& qui, Tr<double>,Random& mt)
{
  return sample(qui,Data_<Tr>(),mt);
}




//--------------


template <template<class...> class Tr, class Fid, class... Fs,class Id,class Id2,bool Complete,class...Ds2, class Random>
auto sample(const quimulun<Fid,Fs...>& qui,Id,Data_new_<Tr,Id2,Complete,Ds2...>&& d, Random& mt)
{
  if constexpr (is_in_pack<Id,Id2,typename Ds2::myId...>())
    return std::move(d)(Id{});
  else
    return sample(qui(Id{}),d,mt);
}

template <template<class...> class Tr,class Id, class Fid,class...F,bool Complete,class...Ds2, class Random>
auto sample(const quimulun<Fid,F...>& qui,Data_new_<Tr,Id,Complete,Ds2...>&& d, Random& mt)
{
  auto s=(Data_new_<Tr,Id,true>{}|...|sample(qui,typename F::myId{},std::move(d),mt));
  static_assert (s.is_complete|| !std::is_same_v<decltype (s), Data_new_<Tr,Id,Complete,Ds2...>>," not reached");
  if constexpr (s.is_complete)
    return s;
  else
    return sample(qui,std::move(s),mt);
}
template <template<class...> class Tr,class Fid,class Id, class...Fs,class Random>
auto sample(const quimulun<Fid,Fs...>& qui, Id, Tr<double>,Random& mt)
{
  return sample(qui,Data_new_<Tr,Id,true>(),mt);
}









} // namespace qm


#endif // QM_MODELS_H
