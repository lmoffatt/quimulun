#ifndef QM_TENSOR_MODEL_H
#define QM_TENSOR_MODEL_H
#include "qm_tensor_distribution.h"
#include <functional>
#include <type_traits>


//namespace qm {
template<class Id> struct all{
//  constexpr static auto className=Id::className+my_static_string("_all");
};



template<class Id,class G, class... Xs>
class  F
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class... Param>
  auto operator()( const Param&... par)const
  {
    auto  out=apply(g_,get_from<Xs>(par...)()...);

    return x_i(Id{},std::move(out));
  }
  F(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
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
  auto operator()(const Param&... par)const
  {
    auto  out=std::invoke(g_,get_from<Xs>(par...)...);

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
    auto  out=apply(g_,get_from<Xs>(par...)...);
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
    auto  out=std::invoke(g_,get_from<Xs>(par...)...);
    return std::apply([](auto&&...v){return vector_space(x_i(Ids{},std::move(v))...);}, std::move(out));
  }
  F(all<Cs<Ids...>> ,G&& g, Xs&&...):g_{std::move(g)}{}
};

template< class Id,class G, class... Xs>
struct extract_function_Id<F<Id,G,Xs...>>{using type=Cs<Id>;};



template<class Id,class G, class... Xs,class... Datas>
auto calculate(const F<Id,G,Xs...>& f, Datas&&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from<Xs>(std::forward<Datas>(d)...)),Nothing>||...))
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

  /*
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

*/

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


};




template<class...Fs >   quimulun(Fs&&...) -> quimulun<Fs...>;
template< class...Fs >   quimulun(const Fs&...) -> quimulun<Fs...>;

template<class... Ids> struct myselect{};
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
constexpr void check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
}














template <class...F,class... Ids,class...Datas>
auto logP(const quimulun<F...>& qui, myselect<Ids...>,const Datas&... d)
{
  auto s=(logP(qui[Ids{}],qui,d...)+...);
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





template <class...F,class...ds, class Sim, typename =std::enable_if_t<is_this_template_class<vector_space,std::decay_t<Sim>>::value>>
auto Parameters(const quimulun<F...>& qui,const vector_space<ds...>& data,Sim&& simulation_results)
{
  using distribution_ids=extract_distribution_Id_t<Cs<F...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;
  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return  std::forward<Sim>(simulation_results)|Parameters_ids{};
}

template <class...F,class...ds, class Sim, typename =std::enable_if_t<is_this_template_class<vector_space,std::decay_t<Sim>>::value>>
auto Variables(const quimulun<F...>& qui,const vector_space<ds...>& data,Sim&& simulation_results)
{
  using distribution_ids=extract_distribution_Id_t<Cs<F...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;
  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  using Variables_ids=transfer_t<parameters_ids,myremove<>>;


  return std::forward<Sim>(simulation_results)|Variables_ids{};
 }


 template <class...F,class...ds, class...ps, class... vs>
 auto logPrior(const quimulun<F...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& variables )
 {
   using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<F>...>;
   using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;

   using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
   return logP(qui,Parameters_ids{},data,parameters,variables);
 }

 template <class...F,class...ds, class...ps, class... vs>
 auto logLikelihood(const quimulun<F...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& variables )
 {
   using distribution_ids=pack_concatenation_t<extract_distribution_Id_t<F>...>;
   using likelihood_ids=pack_intersection_t<distribution_ids,Cs<typename ds::myId...>>;
   using Likelihood_ids=transfer_t<likelihood_ids,myselect<>>;
   return logP(qui,Likelihood_ids{},data,parameters,variables);
 }



 /*

 template <class...F,class...Ds2,  class... Fields,class Random, class...Datas>
 auto sample(const quimulun<F...>& qui,const vector_space<Ds2...>& data, Random& mt,Cs<Fields...>,const Datas&...d)
 {
   return (sample(qui,data,mt,Fields{},d...)+...);
 }


 template <class...F,class...Ds2, class Random>
 auto sample(const quimulun<F...>& qui,vector_space<Ds2...>&& d, Random& mt)
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
 auto calculate(const quimulun<Fs...>& qui,const vector_space<Ds2...>& d, Id,const Datas&...other)
 {
   return calculate(qui[Id{}],d,other...);
 }


 template <class...F,class...ds,class... Var_ids, class...Other >
 auto calculate(const quimulun<F...>& qui, const vector_space<ds...>& data, myselect<Var_ids...>,const Other&... other)
 {
   return (vector_space<>{}+...+calculate(qui,data,Var_ids{},other...));
 }




 template < class... Fs,class Id,class...Ds2, class Random, class... Datas>
 auto sample(const quimulun<Fs...>& qui,const vector_space<Ds2...>& d, Random& mt,Id,const Datas&...other)
 {
   return sample(qui[Id{}],mt,qui,d,other...);
 }


 template <class...F,class...ds, class x_random,class... Par_ids, class...Other >
 auto sample(const quimulun<F...>& qui, const vector_space<ds...>& data, x_random& mt,myselect<Par_ids...>,const Other&... other)
 {

   return (vector_space<>{}+...+sample(qui,data,mt,Par_ids{},other...));
 }

 template <class...F,class...ds,class... par_ids, class...Var_ids, class...var_ids>
 auto calculate(const quimulun<F...>& qui, const vector_space<ds...>& data,const vector_space<par_ids...>& parameters,
                myselect<Var_ids...>,vector_space<var_ids...>&& variables)
 {
   auto new_variables=calculate(qui,data,myselect<Var_ids...>{},parameters,variables);


   if constexpr (new_variables.size()==0)
   {
     if constexpr(sizeof... (Var_ids)==0)
       return std::move(variables);
     else
       return false;
   }
   else
   {
     using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
     return calculate(qui,data,parameters,new_var_ids{},
                   std::move(variables)+std::move(new_variables));

   }
 }



 template <class...F,class...ds, class...ps>
 auto calculate(const quimulun<F...>& qui, const vector_space<ds...>& data,  const vector_space<ps...>& parameters)
 {
   using variables_ids=extract_function_Id_t<Cs<F...>>;
   using Variables_ids=transfer_t<variables_ids,myselect<>>;

   return calculate(qui,data,parameters,Variables_ids{}, vector_space<>{});


 }




 template <class...F,class...ds, class x_random,class... Par_ids, class...Var_ids,class... par_ids, class...var_ids>
 auto sample(const quimulun<F...>& qui, const vector_space<ds...>& data, x_random& mt,
             myselect<Par_ids...>,myselect<Var_ids...>,
               vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables)
 {
   auto new_parameters=sample(qui,data,mt,myselect<Par_ids...>{},parameters,variables);
   auto new_variables=calculate(qui,data,myselect<Var_ids...>{},parameters,variables);


   if constexpr (new_parameters.size()+new_variables.size()==0)
   {
     if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)==0)
       return std::make_tuple(std::move(parameters),std::move(variables));
     else
       return false;
   }
   else
   {
     using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
     using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
     return sample(qui,data,mt,new_par_ids{},new_var_ids{},
                     std::move(parameters)+std::move(new_parameters),
                     std::move(variables)+std::move(new_variables));

   }
 }



 template <class...F,class...ds, class x_random>
 auto sample(const quimulun<F...>& qui, const vector_space<ds...>& data, x_random& mt)
 {
   using distribution_ids=extract_distribution_Id_t<Cs<F...>>;
   using variables_ids=extract_function_Id_t<Cs<F...>>;
   using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,Cs<typename ds::myId...>>,myselect<>>;
   using Variables_ids=transfer_t<variables_ids,myselect<>>;

   return sample(qui,data,mt,Parameters_ids{},Variables_ids{}, vector_space<>{}, vector_space<>{});


 }




 template <class...F,class...ds, class x_random,class... Par_ids, class...Var_ids,class ... Pred_ids ,class... par_ids, class...var_ids,class ... pred_ids >
 auto simulate(const quimulun<F...>& qui, const vector_space<ds...>& data, x_random& mt,myselect<Par_ids...>,myselect<Var_ids...>,myselect<Pred_ids...>,
               vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables, vector_space<pred_ids...>&& predictions)
 {
   auto new_parameters=sample(qui,data,mt,myselect<Par_ids...>{},parameters,variables,predictions);
   auto new_variables=calculate(qui,data,myselect<Var_ids...>{},parameters,variables,predictions);
   auto new_predictions=sample(qui,data,mt,myselect<Pred_ids...>{},parameters,variables,predictions);


   if constexpr (new_parameters.size()+new_variables.size()+new_predictions.size()==0)
   {
     if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)+sizeof... (Pred_ids)==0)
        return std::make_tuple(std::move(parameters),std::move(variables),std::move(predictions));
     else
       return false;
   }
   else
   {
     using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
     using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
     using  new_pred_ids=transfer_t<pack_difference_t<Cs<Pred_ids...>,typename decltype (new_predictions)::myIds>,myselect<>>;
     return simulate(qui,data,mt,new_par_ids{},new_var_ids{},new_pred_ids{},
                     std::move(parameters)+std::move(new_parameters),
                     std::move(variables)+std::move(new_variables),
                     std::move(predictions)+std::move(new_predictions));

   }
 }


 template <class...F,class...ds, class x_random>
 auto simulate(const quimulun<F...>& qui, const vector_space<ds...>& data, x_random& mt)
 {
   using distribution_ids=extract_distribution_Id_t<Cs<F...>>;
   using prediction_ids=pack_intersection_t<distribution_ids,Cs<typename ds::myId...>>;
   using variables_ids=extract_function_Id_t<Cs<F...>>;
   using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,prediction_ids>,myselect<>>;
   using Prediction_ids=transfer_t<prediction_ids,myselect<>>;
   using Variables_ids=transfer_t<variables_ids,myselect<>>;

   return simulate(qui,data,mt,Parameters_ids{},Variables_ids{},Prediction_ids{}, vector_space<>{}, vector_space<>{}, vector_space<>{});


 }







//} // namespace qm




#endif // QM_TENSOR_MODEL_H
