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


template<class... > struct quimulun;

template<class...Ids, class...Gs > struct  quimulun<F<Ids,Gs>...>: public F<Ids,Gs>...{using F<Ids,Gs>::operator()...;};

template<class...Ids, class...Gs >   quimulun(F<Ids,Gs>...) -> quimulun<F<Ids,Gs>...>;


template <template<class...> class Tr, class Id,class...Ids, class...Gs,class...Ids2, class Random>
auto sample(const quimulun<F<Ids,Gs>...>& qui,Id,Data_<Tr,Ids2...>&& d, Random& mt)
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

template <template<class...> class Tr,class...Ids, class... Gs,class...Ids2, class Random>
Data_<Tr,Ids...> sample(const quimulun<F<Ids,Gs>...>& qui,Data_<Tr,Ids2...>&& d, Random& mt)
{
  auto s=(sample(qui,Ids{},std::move(d),mt)+...);
  if constexpr (std::is_same_v<decltype (s),Data_<Tr,Ids...> >)
    return s;
  else
    return sample(qui,std::move(s),mt);
}
template <template<class...> class Tr,class...Ids, class ...Gs,class Random>
auto sample(const quimulun<F<Ids,Gs>...>& qui, Tr<double>,Random& mt)
{
  return sample(qui,Data_<Tr>(),mt);
}








} // namespace qm


#endif // QM_MODELS_H
