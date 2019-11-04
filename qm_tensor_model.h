#ifndef QM_TENSOR_MODEL_H
#define QM_TENSOR_MODEL_H
#include "qm_tensor_distribution.h"
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
//    using pretest=typename Cs<typename std::decay_t<decltype(get_from<Xs...>(par...))>::element_type>::test;
//    using test=typename value_type::dge;
    auto out=consolidate<value_type>(vec<>{},get_from<Xs>(par...)...);
    auto p=out.begin();
    do {
      out(p)=std::invoke(g_,get_from<Xs>(par...)(p)...);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
  }
  F(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
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


template<class... > struct quimulun;

template<class...Fs > struct  quimulun: public Fs...{
  using Fs::operator[]...;
  using myIds= Cs<typename Fs::myId...>;
  
  quimulun(Fs&&...f):Fs{std::move(f)}...{}
  quimulun(const Fs&...f):Fs{f}...{}

};




template<class...Fs >   quimulun(Fs&&...) -> quimulun<Fs...>;
template< class...Fs >   quimulun(const Fs&...) -> quimulun<Fs...>;

template<class... Ids> struct myselect{};

template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myselect<Cs<Ids...>>)
{
  return q|myselect<Ids...>{};
}
template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myselect<Ids...>)
{
  return quimulun(q[Ids{}]...);
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


template<class...Fs, class...Fs2>
auto operator + (const quimulun<Fs...>& q1,const quimulun<Fs2...>& q2)
    ->std::enable_if_t<!(is_in_pack(typename Fs2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Fs2...>>
{
  return quimulun(q1[typename Fs::myId{}]...,q2[typename Fs2::myId{}]...);
}

template<class...Fs, class...Ids2>
auto operator + (const quimulun<Fs...>& q1,const vector_space<Ids2...>& q2)
    ->std::enable_if_t<!(is_in_pack(typename Ids2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Ids2...>>
{
  return quimulun(q1[typename Fs::myId{}]...,q2[typename Ids2::myId{}]...);
}
template<class...Fs, class...Ids2>
auto operator + (const vector_space<Ids2...>& q2,const quimulun<Fs...>& q1)
    ->std::enable_if_t<!(is_in_pack(typename Ids2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Ids2...>>
{
  return quimulun(q2[typename Ids2::myId{}]...,q1[typename Fs::myId{}]...);
}


template< class...Fs,  class...Fs2>
auto operator << (const quimulun<Fs...>& q1,const quimulun<Fs2...>& q2)
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
constexpr  bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}



template <class ...Ups, class ...Downs, class value_one,
          class ...Ups2, class ...Downs2, class value_two>
 auto operator + (const x_i<logpr<up<Ups...>,dn<Downs...>>,value_one>& one,
                         const x_i<logpr<up<Ups2...>,dn<Downs2...>>,value_two>& two)

{
  return x_i(logpr<up<Ups...>,dn<Downs...>>{}+logpr<up<Ups2...>,dn<Downs2...>>{},one()+two());
    };




template < class... Fs,class Id,class...Ds2, class Random>
auto sample(const quimulun<Fs...>& qui,Id,const vector_space<Ds2...>& d, Random& mt)
{
  return sample(qui[Id{}],d,mt);
}


template <class... Ids_Not_Reachead>
constexpr void check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
}

template <class...F,class...Ds2,  class... Fields,class Random>
auto sample(const quimulun<F...>& qui,const vector_space<Ds2...>& d, Cs<Fields...>,Random& mt)
{
  return (Is_Complete<true,vector_space<>>()&&...&&sample(qui,Fields{},d,mt));
}


template <class...F,class...Ds2, class Random>
auto sample(const quimulun<F...>& qui,vector_space<Ds2...>&& d, Random& mt)
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


template <  class... Fs,class Id0,class...Datas>
auto calculate(const quimulun<Fs...>& qui,Id0,const Datas&... data)
{
  return calculate(qui[Id0{}],data...);
}


template <class...F,class... Fields,class...Ds1,class...Datas>
auto calculate(const quimulun<F...>& qui, Cs<Fields...>,const vector_space<Ds1...>& variables, const Datas&... parameters)
{
  return (Is_Complete<true,vector_space<>>()&&...&&calculate(qui,Fields{},variables, parameters...));

}


template <class...F,class...Datas,class...Ds2 >
auto calculate(const quimulun<F...>& qui,vector_space<Ds2...>&& variables,const Datas&...param )
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




template <class...F,class...Datas>
auto logP(const quimulun<F...>& qui,const Datas&... d)
{
  auto ds=calculate(qui,vector_space<>{},d...);
  auto s=(logP(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class...F,class...Datas>
auto FIM(const quimulun<F...>& qui,const Datas&... d)
{
  auto ds=calculate(qui,vector_space<>{},d...);
  auto s=(FIM(qui[typename F::myId{}],d...,ds)+...);
  return s;
}


template <class...F,class...Ds2, class... Ds3>
auto MAP(const quimulun<F...>& qui,const vector_space<Ds2...>& data, const vector_space<Ds2...>& parameters )
{
  auto dpar=Self_Derivative(parameters);

  auto logL=logP(qui,data,dpar);
  auto fim=FIM(qui,data,dpar);





}











//} // namespace qm




#endif // QM_TENSOR_MODEL_H
