#ifndef QM_TENSOR_MODEL_H
#define QM_TENSOR_MODEL_H
#include "qm_tensor_distribution.h"
#include <functional>
#include <type_traits>


//namespace qm {
template<class Id> struct all{
  //  constexpr static auto className=Id::className+my_static_string("_all");
};




template<class Id, class...Xs, class valuetype>
struct all<x_i<Id,vector_field<vec<Xs...>,valuetype>>>
{
  x_i<Id,vector_field<vec<Xs...>,valuetype>>const & me;

  all(x_i<Id,vector_field<vec<Xs...>,valuetype>>const & v):me{v}{}
  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto& operator()() const {return *this;}

  template<class Position>
  auto& operator()(const Position& ){ return me();}


  template<class Position>
  auto& operator()(const Position& )const { return me();}

};

template<class Id, class...Xs, class valuetype>
struct all<x_i<Id,vector_field<vec<Xs...>,valuetype>>&&>
{
  x_i<Id,vector_field<vec<Xs...>,valuetype>> me;

  all(x_i<Id,vector_field<vec<Xs...>,valuetype>>&& v):me{std::move(v)}{}
  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto& operator()() const {return *this;}

  template<class Position>
  auto& operator()(const Position& ){ return me();}


  template<class Position>
  auto& operator()(const Position& )const { return me();}

};



template<class Id, class...Xs, class valuetype>
all(x_i<Id,vector_field<vec<Xs...>,valuetype>>const & v)->all<x_i<Id,vector_field<vec<Xs...>,valuetype>>>;

template<class Id, class...Xs, class valuetype>
all(x_i<Id,vector_field<vec<Xs...>,valuetype>>&& v)->all<x_i<Id,vector_field<vec<Xs...>,valuetype>>&&>;

template <class anId, class...Datas>
auto get_from(all<anId>, Datas&&...ds)//->decltype (all((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...)))
{
 // using test=typename anId::this_id;
  return all((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...));
}


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
//    using test=typename Id::myID;
//    using test2=typename Cs<Xs...>::myXs;

    auto  out=apply(g_,get_from(Xs{},par...)()...);

    return x_i(Id{},std::move(out));
  }
  F(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
};

template<class Id,class G, class Xrandom,class... Xs>
class  Fr
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class x_rand,class... Param>
  auto operator()(x_rand& x,const Param&... par)const
  {
    auto  out=apply_random(g_,x[non_const<Xrandom>{}](),get_from(Xs{},par...)()...);

    return x_i(Id{},std::move(out));
  }
  Fr(Id ,G&& g,non_const<Xrandom>&&, Xs&&...):g_{std::move(g)}{}
};





template<class Id,class G, class Xrandom,class... Xs>
class  Fr<all<Id>,G,Xrandom,Xs...>
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class x_rand,class... Param>
  auto operator()(x_rand& x,const Param&... par)const
  {
    auto  out=std::invoke(g_,x[non_const<Xrandom>{}](),get_from(Xs{},par...)()...);

    return x_i(Id{},std::move(out));
  }
  Fr(all<Id> ,G&& g,non_const<Xrandom>&&, Xs&&...):g_{std::move(g)}{}
};


template<class Id,class G, class... Xs>
class  F<all<Id>,G,Xs...>
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class... Param>
  auto operator()( Param&&... par)const
  {
    auto  out=std::invoke(g_,get_from(Xs{},std::forward<Param>(par)...)()...);

    return x_i(Id{},std::move(out));
  }
  F(all<Id> ,G&& g, Xs&&...):g_{std::move(g)}{}
};



template<class ...Ids,class G, class... Xs>
class  F<Cs<Ids...>,G,Xs...>
{
private:
  G g_;
public:
  typedef   Cs<Ids...> myId;
  auto &operator[](Cs<Ids...>)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    auto  out=apply(g_,get_from(Xs{},par...)...);
    return std::apply([](auto&&...v){return vector_space(x_i(Ids{},std::move(v))...);}, std::move(out));
  }
  F(Cs<Ids...> ,G&& g, Xs&&...):g_{std::move(g)}{}
};

template<class ...Ids,class G, class... Xs>
class  F<all<Cs<Ids...>>,G,Xs...>
{
private:
  G g_;
public:
  typedef   Cs<Ids...> myId;
  auto &operator[](Cs<Ids...>)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    auto  out=std::invoke(g_,get_from(Xs{},par...)...);
    return std::apply([](auto&&...v){return vector_space(x_i(Ids{},std::move(v))...);}, std::move(out));
  }
  F(all<Cs<Ids...>> ,G&& g, Xs&&...):g_{std::move(g)}{}
};

template< class Id,class G, class... Xs>
struct extract_function_Id<F<Id,G,Xs...>>{using type=Cs<Id>;};
template< class Id,class G, class... Xs>
struct extract_function_Id<Fr<Id,G,Xs...>>{using type=Cs<Id>;};

template< class Id,class G, class... Xs>
struct extract_function_Id<F<all<Id>,G,Xs...>>{using type=Cs<Id>;};
template< class Id,class G, class... Xs>
struct extract_function_Id<Fr<all<Id>,G,Xs...>>{using type=Cs<Id>;};

template< class Id,class G, class... Xs>
struct extract_function_Id<Coord<all<Id>,G,Xs...>>{using type=Cs<Id>;};



template<class Id,class G, class... Xs,class... Datas>
auto calculate(const F<Id,G,Xs...>& f, Datas&&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from(Xs{},std::forward<Datas>(d)...)),Nothing>||...))
  {
    //   using testId=typename Id::test_Id;
    // using data=typename Cs<Datas...>::data;
    //using test=typename Cs<Id,Xs...>::fails_Xs;
    return Nothing{};

  }else
  {
    //  using testId=typename Id::test_Id;
    //  using test=typename Cs<Id,Xs...>::pass_Xs;

    return f(std::forward<Datas>(d)...);
  }
}

template<class Id,class G, class... Xs,class... Datas>
auto calculate(const Fr<Id,G,Xs...>& f, Datas&&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from(Xs{},std::forward<Datas>(d)...)),Nothing>||...))
  {
    //   using testId=typename Id::test_Id;
    // using data=typename Cs<Datas...>::data;
    //using test=typename Cs<Id,Xs...>::fails_Xs;
    return Nothing{};

  }else
  {
    //  using testId=typename Id::test_Id;
    //  using test=typename Cs<Id,Xs...>::pass_Xs;

    return f(std::forward<Datas>(d)...);
  }
}



    template<class Id, class G, class...Xs> F(Id&&,G&&,Xs&&...)->F<Id,G,Xs...>;

template<class e_i, class Value_type>
struct f_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  using element_type= Value_type ;


  using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;


private:
  Value_type value_;
public:

  explicit f_i(const value_type& x):value_{x}{}
  explicit f_i(value_type&& x):value_{std::move(x)}{}
  explicit f_i(e_i,value_type&& x):value_{std::move(x)}{}
  explicit f_i(e_i,const value_type& x):value_{x}{}


  f_i()=default;
  f_i const& operator[](e_i)const & {return *this;}
  f_i& operator[](e_i) & {return *this;}

  friend auto begin(const f_i& me){ return begin(me());}

  f_i operator[](e_i)&& {return *this;}

  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}





  auto& operator()()const &{ return value_;}
  auto& operator()()&{ return value_;}
  //auto operator()()&& {return value_;}



  template<class Position>
  auto& operator()(const Position& p){ return value_(p);}

  template<class Position>
  auto& operator()(const Position& p)const { return value_(p);}

};



template<bool Complete,class... Ds, class Id2, class Value_type>
Is_Complete<Complete,vector_space<Ds...,f_i<Id2,Value_type>>>
operator &&
    (Is_Complete<Complete,vector_space<Ds...>>&& d, f_i<Id2,Value_type>&& d2)
{
  //  using test=typename Id2::test_this_Id;
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,f_i<Id2,Value_type>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}

template<class... > struct quimulun;

template<class...Fs > struct  quimulun: public Fs...{
  using Fs::operator[]...;
  using myIds= Cs<typename Fs::myId...>;
  
  quimulun(Fs&&...f):Fs{std::move(f)}...{}
  quimulun(const Fs&...f):Fs{f}...{}

  friend constexpr auto begin(const quimulun& ){ return Position<>{};}

  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const { return *this;}
  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }

};




template<class...Fs >   quimulun(Fs&&...) -> quimulun<Fs...>;
template< class...Fs >   quimulun(const Fs&...) -> quimulun<Fs...>;

template<class... Ids> struct myselect{ static constexpr auto size=sizeof... (Ids);};
template<class... Ids> struct myremove{};


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
template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<Ids...>>,myselect<>>;
  return q|sel{};
}

template<class...Fs, class ...Ids >
auto operator | ( quimulun<Fs...>&& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<Ids...>>,myselect<>>;
  return std::move(q)|sel{};
}




template<class...Fs, class ...Ids >
auto operator | ( quimulun<Fs...>&& q, myselect<Ids...>)
{
  return quimulun(std::move(q)[Ids{}]...);
}



template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...>&& d, myselect<Ids...>)
{
  return vector_space{std::move(d)[Ids{}]...};
}

template<class... Ds, class...Ids>
auto operator | (const vector_space<Ds...>& d, myselect<Ids...>)
{
  // using test=typename Cs<decltype (d[Ids{}])...>::test;
  return vector_space{d[Ids{}]...};
}



template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...>&& d, myselect<Cs<Ids...>>)
{
  return std::move(d)|myselect<Ids...>{};
}




template<class... Ds, class...Ids>
auto operator | (const vector_space<Ds...>& d, myselect<Cs<Ids...>>)
{
  return d|myselect<Ids...>{};
}






template<class...Ds, class ...Ids >
auto operator | (const vector_space<Ds...>& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Ds::myId...>,Cs<Ids...>>,myselect<>>;
  return q|sel{};
}

template<class...Ds, class ...Ids >
auto operator | ( vector_space<Ds...>&& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Ds::myId...>,Cs<Ids...>>,myselect<>>;
  return std::move(q)|sel{};
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





template <class... Ids_Not_Reachead>
constexpr bool check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
  return false;
}














template <class...Fs,class... Ids,class...Datas>
auto logP(const quimulun<Fs...>& qui, myselect<Ids...>,const Datas&... d)
{
  auto s=(logP(qui[Ids{}],qui,d...)+...);
  return s;
}


template <class...Fs,class... Ids,class...Datas>
auto FIM(const quimulun<Fs...>& qui, myselect<Ids...>,const Datas&... d)
{
  auto s=(FIM(qui[Ids{}],qui,d...)+...);
  return s;
}



template <class...Fs,class...Ds2, class... Ds3>
auto MAP(const quimulun<Fs...>& qui,const vector_space<Ds2...>& data, const vector_space<Ds2...>& parameters )
{
  auto dpar=Self_Derivative(parameters);

  auto logL=logP(qui,data,dpar);
  auto fim=FIM(qui,data,dpar);


}





template <class...Fs,class...ds, class Sim, typename =std::enable_if_t<is_this_template_class<vector_space,std::decay_t<Sim>>::value>>
auto Parameters(const quimulun<Fs...>& qui,const vector_space<ds...>& data,Sim&& simulation_results)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;
  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return  std::forward<Sim>(simulation_results)|Parameters_ids{};
}

template <class...Fs,class...ds, class Sim, typename =std::enable_if_t<is_this_template_class<vector_space,std::decay_t<Sim>>::value>>
auto Variables(const quimulun<Fs...>& qui,const vector_space<ds...>& data,Sim&& simulation_results)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;
  //using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  using Variables_ids=transfer_t<parameters_ids,myremove<>>;


  return std::forward<Sim>(simulation_results)|Variables_ids{};
}


template <class...Fs,class...ds, class...ps, class... vs>
auto logPrior(const quimulun<Fs...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& variables )
{
  using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<Fs>...>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;

  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return logP(qui,Parameters_ids{},data,parameters,variables);
}

template <class...Fs,class...ds, class...ps, class... vs>
auto logLikelihood(const quimulun<Fs...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& variables )
{
  using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<Fs>...>;
  using likelihood_ids=pack_intersection_t<distribution_ids,Cs<typename ds::myId...>>;
  using Likelihood_ids=transfer_t<likelihood_ids,myselect<>>;
  return logP(qui,Likelihood_ids{},data,parameters,variables);
}

template <class...Fs,class...ds, class...ps, class... vs>
auto fimPrior(const quimulun<Fs...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& dvariables )
{
  using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<Fs>...>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;

  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return FIM(qui,Parameters_ids{},data,parameters,dvariables);
}

template <class...Fs,class...ds, class...ps, class... vs>
auto fimLikelihood(const quimulun<Fs...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& dvariables )
{
  using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<Fs>...>;
  using likelihood_ids=pack_intersection_t<distribution_ids,Cs<typename ds::myId...>>;
  using Likelihood_ids=transfer_t<likelihood_ids,myselect<>>;
  return FIM(qui,Likelihood_ids{},data,parameters,dvariables);
}





/*

template <class...Fs,class...Ds2,  class... Fields,class Random, class...Datas>
auto sample(const quimulun<Fs...>& qui,const vector_space<Ds2...>& data, Random& mt,Cs<Fields...>,const Datas&...d)
{
  return (sample(qui,data,mt,Fields{},d...)+...);
}


template <class...Fs,class...Ds2, class Random>
auto sample(const quimulun<Fs...>& qui,vector_space<Ds2...>&& d, Random& mt)
{
  auto Fields=pack_difference_t<Cs<typename F::myId...>,Cs<typename Ds2::myId...>>{};
  auto s=sample(qui,d,Fields,mt);
  //typedef typename decltype (s)::sample sample_type;
  if constexpr ( std::is_same_v<typename decltype(s)::type,vector_space<>>)
  {
    //typedef typename decltype(Fields)::sample sample_type;

    check_if_reacheable(Fields);
    return false;
  }
  if constexpr (s.is_complete)
    return std::move(d)+std::move(s.value);
  else
    return sample(qui,std::move(d)+std::move(s.value),mt);
}

*/
template < class... Fs,class Id,class...Ds2,  class... Datas>
auto calculate_Id(const quimulun<Fs...>& qui, Id,const Datas&...other)
{
  return calculate(qui[Id{}],qui,other...);
}


template <class...Fs,class...ds,class... Var_ids, class...Other >
auto calculate(const quimulun<Fs...>& qui, myselect<Var_ids...>, const Other&... other)
{
  return (vector_space<>{}+...+calculate_Id(qui,Var_ids{},other...));
}

template <class...Fs,class...ds,class... Var_ids, class...Other >
auto calculate_set(const quimulun<Fs...>& qui, myselect<Var_ids...>, const Other&... other)
{
  return (vector_space<>{}+...+calculate_Id(qui,Var_ids{},other...));
}




template < class... Fs,class Id, class Random, class... Datas>
auto sample_Id(const quimulun<Fs...>& qui, Random& mt,Id,const Datas&...other)
{
  return sample(qui[Id{}],mt,qui,other...);
}


template <class...Fs,class...ds, class x_random,class... Par_ids, class...Other >
auto sample(const quimulun<Fs...>& qui, x_random& mt,myselect<Par_ids...>,const Other&... other)
{

  return (vector_space<>{}+...+sample_Id(qui,mt,Par_ids{},other...));
}



template <class...Fs,class...ds,class... Ds, class...Var_ids, class...var_ids>
auto calculate(const quimulun<Fs...>& qui,
               myselect<Var_ids...>,vector_space<var_ids...>&& variables, const Ds&... datas)
{
  auto new_variables=calculate_set(qui,myselect<Var_ids...>{},variables,datas...);


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(Cs<Var_ids...>{});
    }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return calculate(qui,new_var_ids{},std::move(variables)+std::move(new_variables),datas...);

  }
}



template <class...Fs,class...ds, class...Ds>
auto calculate(const quimulun<Fs...>& qui, const Ds&...datas)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;
  return calculate(qui,Variables_ids{}, vector_space<>{},datas...);
}


template < class... Fs,class x_random,class Id,class...Ds2,  class... Datas>
auto random_calculate_Id(const quimulun<Fs...>& qui,x_random& mt, Id,const Datas&...other)
{
  return calculate(qui[Id{}],mt,qui,other...);
}


template <class...Fs,class x_random,class...ds,class... Var_ids, class...Other >
auto random_calculate_this(const quimulun<Fs...>& qui, x_random& mt,myselect<Var_ids...>, const Other&... other)
{
  return (vector_space<>{}+...+random_calculate_Id(qui,mt,Var_ids{},other...));
}




template <class...Fs,class x_random,class...ds,class... Ds, class...Var_ids, class...var_ids>
auto random_calculate(const quimulun<Fs...>& qui,x_random& mt,
               myselect<Var_ids...>,vector_space<var_ids...>&& variables, const Ds&... datas)
{
  auto new_variables=random_calculate_this(qui,mt,myselect<Var_ids...>{},variables,datas...);


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
       return check_if_reacheable(Cs<Var_ids...>{});

  }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return random_calculate(qui,mt,new_var_ids{},std::move(variables)+std::move(new_variables),datas...);

  }
}



template <class...Fs,class x_random,class...ds, class...Ds>
auto random_calculate(const quimulun<Fs...>& qui,x_random& mt, const Ds&...datas)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return random_calculate(qui,mt,Variables_ids{}, vector_space<>{},datas...);


}





template <class...Fs,class...Ds, class x_random,class... Par_ids, class...Var_ids,class... par_ids, class...var_ids>
auto sample(const quimulun<Fs...>& qui,  x_random& mt,
            myselect<Par_ids...>,myselect<Var_ids...>,
            vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables,const Ds&... datas)
{
  auto new_parameters=sample(qui,mt,myselect<Par_ids...>{},parameters,variables,datas...);
  auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,datas...);


  if constexpr (new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)==0)
      return std::make_tuple(std::move(parameters),std::move(variables));
    else
      return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>>{});
  }
  else
  {
    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return sample(qui,mt,new_par_ids{},new_var_ids{},
                  std::move(parameters)+std::move(new_parameters),
                  std::move(variables)+std::move(new_variables),datas...);

  }
}



template <class...Fs,class...Ds, class x_random>
auto sample(const quimulun<Fs...>& qui, x_random& mt, const Ds&... datas)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,pack_concatenation_t<typename Ds::myIds...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return sample(qui,mt,Parameters_ids{},Variables_ids{}, vector_space<>{}, vector_space<>{},datas...);


}


template <class...Fs,class...Ds, class x_random,class... Par_ids, class...Var_ids,class... par_ids, class...var_ids>
auto sampleParameters(const quimulun<Fs...>& qui,  x_random& mt,
                      myselect<Par_ids...>,myselect<Var_ids...>,
                      vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables, const Ds&...datas)
{
  auto new_parameters=sample(qui,mt,myselect<Par_ids...>{},parameters,variables,datas...);
  using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
  if constexpr (new_par_ids::size==0)
    return std::move(parameters)+std::move(new_parameters);
  else
  {
    auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,datas...);
    if constexpr (new_parameters.size()+new_variables.size()==0)
      return check_if_reacheable(Cs<Par_ids...>{});
    else
    {
      using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
      return sampleParameters(qui,mt,new_par_ids{},new_var_ids{},
                    std::move(parameters)+std::move(new_parameters),
                    std::move(variables)+std::move(new_variables),datas...);

    }
  }
}



template <class...Fs,class...Ds, class x_random>
auto sampleParameters(const quimulun<Fs...>& qui, x_random& mt,const Ds&... data )
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids, pack_concatenation_t<typename Ds::myIds...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return sampleParameters(qui,mt,Parameters_ids{},Variables_ids{}, vector_space<>{}, vector_space<>{},data...);


}



template <class...Fs,class...Ds, class x_random,class... Par_ids, class...Var_ids,class ... Pred_ids ,class... par_ids, class...var_ids,class ... pred_ids >
auto simulate(const quimulun<Fs...>& qui,  x_random& mt,myselect<Par_ids...>,myselect<Var_ids...>,myselect<Pred_ids...>,
              vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables, vector_space<pred_ids...>&& predictions,const Ds&...data)
{
  auto new_parameters=sample(qui,mt,myselect<Par_ids...>{},parameters,variables,predictions,data...);
  auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,predictions,data...);
  auto new_predictions=sample(qui,mt,myselect<Pred_ids...>{},parameters,variables,predictions,data...);


  if constexpr (new_parameters.size()+new_variables.size()+new_predictions.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)+sizeof... (Pred_ids)==0)
      return std::make_tuple(std::move(parameters),std::move(variables),std::move(predictions));
    else
      return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>,Cs<Pred_ids...>>{});
  }
  else
  {
    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    using  new_pred_ids=transfer_t<pack_difference_t<Cs<Pred_ids...>,typename decltype (new_predictions)::myIds>,myselect<>>;
    return simulate(qui,mt,new_par_ids{},new_var_ids{},new_pred_ids{},
                    std::move(parameters)+std::move(new_parameters),
                    std::move(variables)+std::move(new_variables),
                    std::move(predictions)+std::move(new_predictions),data...);

  }
}


template <class...Fs,class...Ds, class x_random>
auto simulate(const quimulun<Fs...>& qui,  x_random& mt,const Ds&... data)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using prediction_ids=pack_intersection_t<distribution_ids,pack_concatenation_t<typename Ds::myIds...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,prediction_ids>,myselect<>>;
  using Prediction_ids=transfer_t<prediction_ids,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return simulate(qui,mt,Parameters_ids{},Variables_ids{},Prediction_ids{}, vector_space<>{}, vector_space<>{}, vector_space<>{},data...);


}

template < class... Fs,class Id,class...Ds2,  class... Datas>
auto transfer_Id(const quimulun<Fs...>& qui, Id, Datas&&...other)
{
  return calculate(qui[Id{}],qui,std::forward<Datas>(other)...);
}


template <class...Fs,class...ds,class... Var_ids, class...Other >
auto transfer_this(const quimulun<Fs...>& qui, myselect<Var_ids...>,  Other&&... other)
{
  return (vector_space<>{}+...+transfer_Id(qui,Var_ids{},std::forward<Other>(other)...));
}

template <class...Fs,class...ds,class... Ds, class...Var_ids, class...var_ids>
auto transfer(const quimulun<Fs...>& qui,
               myselect<Var_ids...>,vector_space<var_ids...>&& variables, Ds&&... datas)
{
  auto new_variables=transfer_this(qui,myselect<Var_ids...>{},std::move(variables),std::forward<Ds>(datas)...);


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(Cs<Var_ids...>{});
  }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return transfer(qui,new_var_ids{},std::move(variables)+std::move(new_variables),std::forward<Ds>(datas)...);

  }
}



template <class...Fs,class...ds, class...Ds>
auto transfer(const quimulun<Fs...>& qui, Ds&&...datas)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;
  return transfer(qui,Variables_ids{}, vector_space<>{},std::forward<Ds>(datas)...);
}






//} // namespace qm




#endif // QM_TENSOR_MODEL_H
