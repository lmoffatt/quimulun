#ifndef QM_TENSOR_MODEL_EVALUATION_H
#define QM_TENSOR_MODEL_EVALUATION_H

#include <qm_tensor_model_declaration.h>
#include <qm_vector_field_evaluation.h>
#include <qm_tensor_distribution_evaluation.h>

template <class anId, class...Datas>
auto get_from(all<anId>, Datas&&...ds)->decltype (make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...))())
{
 // using test=typename anId::this_id;
 return make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...))();
}

template <class anId, class...Datas>
auto get_from_pass_id(all<anId>, Datas&&...ds)->decltype (make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...)))
{
  //using test=typename anId::this_id;

  //  using test0=typename Cs<Datas...>::mit;
//  using test=typename decltype(make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...)))::ger;
 // using test2=typename decltype(get_xi_from_this(anId{},make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...))))::ger2;
  return
      //test{},
    //test0{},test{},test2{},
       make_all((get_xi_from_this(anId{},std::forward<Datas>(ds))||...));
}





template <class anId, class...Datas
          , typename =std::enable_if_t< (!is_this_template_class_v<all,anId>),int
                                      >>
auto get_from_pass_id(anId, Datas&&...ds)->decltype ((get_xi_from_this(anId{},std::forward<Datas>(ds))||...))
{
 // using test=typename anId::this_id;
  return
     //test{},
     (get_xi_from_this(anId{},std::forward<Datas>(ds))||...);
}

template <class anId, class...Datas>
auto get_from(pass_id<anId>, Datas&&...ds)->decltype (get_from_pass_id(anId{}, std::forward<Datas>(ds)...))
{
  return get_from_pass_id(anId{}, std::forward<Datas>(ds)...);
}




template <class anId, class...Datas

          , typename =std::enable_if_t<   (!is_this_template_class<pass_id,anId>::value)&&
                                          ( !is_this_template_class<std::tuple,anId>::value) ,int>>
auto get_from(non_const<anId>, Datas&&...ds)->decltype ((get_xi_from_this(non_const<anId>{},std::forward<Datas>(ds))||...)())
{
  // using test=typename anId::this_id;
//  auto&& x=((get_xi_from_this(non_const<anId>{},std::forward<Datas>(ds)))||...);
//  std::cerr<<"get_from: &x="<<&x<<"\n";
//  std::cerr<<"get_from: &x()="<<&x()<<"\n";

 // return x();
  return ((get_xi_from_this(non_const<anId>{},std::forward<Datas>(ds)))||...)();
}


template <class anId, class...Datas>
auto get_from(non_const<pass_id<anId>>, Datas&&...ds)->decltype ((only_xi_or_fi(get_xi_from_this(non_const<anId>{},std::forward<Datas>(ds)))||...))
{
  // using test=typename anId::this_id;
  return (only_xi_or_fi(get_xi_from_this(non_const<anId>{},std::forward<Datas>(ds)))||...);
}



template <class... anId, class...Datas>
auto get_from(std::tuple<anId...>, Datas&&...ds)//->decltype ((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...))
{

  if constexpr ((std::is_same_v<decltype (get_from(anId{},ds...)),Nothing > ||...))
  {   //  using test=typename Cs<anId...>::FAILS;
      // using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::FAILS;
    return Nothing{};
  }else
  {
//    using test=typename Cs<anId...>::OK;
//     using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::OK;
//    return x_i(Cs<anId...>{},v(std::forward_as_tuple(get_from(anId{},ds...)...),dimension_less{}));
    return v(std::forward_as_tuple(get_from(anId{},ds...)...),dimension_less{});
  }}

template <class anId, class...Datas>
auto find_in(pass_id<anId>, Datas&&...ds)
{
//  using test=typename anId::gegw;

  if constexpr (std::is_same_v<decltype (get_from_pass_id(anId{},std::forward<Datas>(ds)...)),Nothing >)
    return
        //test{},
        Not_found<anId>{};
  else
    return Has_been_found{};
}


template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param>
auto myInvoke(const Fr<all<Id>,G,Xrandom,Xs...>& f, x_rand& x, Param&& ... par)
{

    auto  out=std::invoke(f.f(),x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}
template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param>
auto myInvoke_parallel_for(const Fr<all<Id>,G,Xrandom,Xs...>& f, x_rand& x, Param&& ... par)
{

  auto  out=std::invoke(f.f(),x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}


template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param,
          typename=std::enable_if_t<!is_this_template_class<all,Id>::value&&!is_this_template_class<pos,Id>::value >>
auto myInvoke(const Fr<Id,G,Xrandom,Xs...>& f, x_rand&& mt,Param&& ... par)
{


  //using test=typename decltype (get_from(non_const<Xrandom>{},std::forward<x_rand>(mt)))::ja;
//  using test2=typename decltype (std::forward<x_rand>(mt))::ja;

 // auto& mymt=get_from(non_const<Xrandom>{},std::forward<x_rand>(mt));
 // std::cerr<<"myInvoke: &mt="<<&mt<<"\n";
//  std::cerr<<"myInvoke: &get_from(non_const<Xrandom>{},std::forward<x_rand>(mt))="<<&get_from(non_const<Xrandom>{},std::forward<x_rand>(mt))<<"\n";

  auto  out=apply_random(f.f(),get_from(non_const<Xrandom>{},std::forward<x_rand>(mt)),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}
template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param,
          typename=std::enable_if_t<!is_this_template_class<all,Id>::value&&!is_this_template_class<pos,Id>::value >>
auto myInvoke_parallel_for(const Fr<Id,G,Xrandom,Xs...>& f, x_rand&& x,Param&& ... par)
{
  auto  out=apply_random_parallel_for(f.f(),get_from(non_const<Xrandom>{},std::forward<x_rand>(x)),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}

template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param, typename=std::enable_if_t<!is_this_template_class<all,Id>::value>>
auto myInvoke(const Fr<pos<Id>,G,Xrandom,Xs...>& f, x_rand& x, Param&& ... par)
{
  auto  out=apply_random_pos(f.f(),x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}

template<class Id,class G, class Xrandom,class... Xs,class x_rand,class... Param, typename=std::enable_if_t<!is_this_template_class<all,Id>::value>>
auto myInvoke_parallel_for(const Fr<pos<Id>,G,Xrandom,Xs...>& f, x_rand& x,Param&& ... par)
{
  auto  out=apply_random_pos_parallel_for(f.f(),x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);
  return x_i(Id{},std::move(out));

}




template<class Id,class G, class... Xs,class... Param, typename=std::enable_if_t<!is_this_template_class<all,Id>::value>>
auto myInvoke(const F<Id,G,Xs...>& f,  Param&& ... par)
{
  auto  out=apply(f.f(),get_from(Xs{},std::forward<Param>(par)...)...);

  return x_i(Id{},std::move(out));

}

template<class Id,class G, class... Xs,class... Param, typename=std::enable_if_t<!is_this_template_class<all,Id>::value>>
auto myInvoke_parallel_for(const F<Id,G,Xs...>& f, Param&& ... par)
{
  auto  out=apply_parallel_for(f.f(),get_from(Xs{},std::forward<Param>(par)...)...);

  return x_i(Id{},std::move(out));

}

template<class Id,class G, class... Xs,class... Param>
auto myInvoke(const F<all<Id>,G,Xs...>& f,  Param&& ... par)
{
  auto  out=std::invoke(f.f(),get_from(Xs{},std::forward<Param>(par)...)...);

  return x_i(Id{},std::move(out));

}

template<class Id,class G, class... Xs,class... Param>
auto myInvoke_parallel_for(const F<all<Id>,G,Xs...>& f,  Param&& ... par)
{
  auto  out=std::invoke(f.f(),get_from(Xs{},std::forward<Param>(par)...)...);

  return x_i(Id{},std::move(out));

}

template<class Id,class G, class... Xs,class... Datas>
auto calculate(const F<Id,G,Xs...>& f, Datas&&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from(Xs{},std::forward<Datas>(d)...)),Nothing>||...))
  {
      // using testId=typename Id::failstest_Id;
    // using data=typename Cs<Datas...>::data;
   // using test=typename Cs<Id,Xs...>::fails_Xs;
    return Nothing{};

  }else
  {
     // using testId=typename Id::test_Id;
     // using test=typename Cs<Id,Xs...>::pass_Xs;

    return myInvoke(f,std::forward<Datas>(d)...);
  }
}


template<class Id,class IOld, class... Datas>
auto calculate(const View<Id,IOld>& , Datas&&... d)
{
  auto res=find_in(IOld{},std::forward<Datas>(d)...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else
  {
    // using testId=typename Id::test_Id;
    // using test=typename Cs<Id,Xs...>::pass_Xs;

    return x_i_view_const(Id{},get_from(IOld{},std::forward<Datas>(d)...)());
  }
}




struct Calculator
{
  template<class ...Xs>
  auto operator()(Xs&&...xs)const
  {
    return calculate(std::forward<Xs>(xs)...);
  }
};

struct Calculator_random
{
  template<class ...Xs>
  auto operator()(Xs&&...xs)const
  {
    return random_calculate(std::forward<Xs>(xs)...);
  }
};



struct Calculator_logPrior
{
  template<class ...Xs>
  auto operator()(Xs&&...xs)const
  {
    return logPrior(std::forward<Xs>(xs)...);
  }
};

struct Calculator_logLikelihood
{
  template<class ...Xs>
  auto operator()(Xs&&...xs)const
  {
    return logLikelihood(std::forward<Xs>(xs)...);
  }
};

struct Calculator_sampleParameters
{
  template<class ...Xs>
  auto operator()(Xs&&...xs)const
  {
    return sampleParameters(std::forward<Xs>(xs)...);
  }
};





template<class Id,class Calc,class Fi, class... Xs,class... Datas>
auto calculate(const Fq<Id,Calc,Fi,Xs...>& , Datas&&... d)
{
  auto res=(find_in(Fi{},std::forward<Datas>(d)...)&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else

  {

    auto  out=apply(Calc{},get_from(Fi{},d...),get_from(Xs{},d...)...);

    return x_i(Id{},std::move(out));
  }
}






template<class Id,class Calc,class Fi, class Xrandom, class... Xs,class x_random,class... Datas>
auto calculate(const Frq<Id,Calc,Fi,Xrandom,Xs...>& , x_random&& mt,Datas&&... d)
{
   auto res=
      ((find_in(Fi{},std::forward<Datas>(d)...)&&find_in(non_const<Xrandom>{},std::forward<x_random>(mt)))&&...&&
       find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else
  {

    auto  out=apply_random_calculate(Calc{},get_from(Fi{},d...),get_from(non_const<Xrandom>{},std::forward<x_random>(mt)),get_from(Xs{},d...)...);

    return x_i(Id{},std::move(out));
  }
}



template<class Id,class Calc,class Fi, class Xr, class Posi,class... Xs,class x_random,class... Datas>
auto calculate(const Frq<pos<Id>,Calc,Fi,Xr,Posi,Xs...>& , x_random&& mt,Datas&&... d)
{
  auto res=((find_in(Fi{},std::forward<Datas>(d)...)&&find_in(non_const<Xr>{},std::forward<x_random>(mt)))&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else
   {

    auto  out=apply_random_calculate_pos(Calc{},get_from(Fi{},d...),get_from(non_const<Xr>{},std::forward<x_random>(mt)),get_from(Xs{},std::forward<Datas>(d)...)...);

    return x_i(Id{},std::move(out));
  }
}



template<class Id,class Calc,class Fi, class... Xs,class... Datas>
auto calculate_parallel_for(const Fq<Id,Calc,Fi,Xs...>& , Datas&&... d)
{
  auto res=(find_in(Fi{},std::forward<Datas>(d)...)&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else

  {

    auto  out=apply_parallel_for(Calc{},get_from(Fi{},d...),get_from(Xs{},d...)...);

    return x_i(Id{},std::move(out));
  }
}






template<class Id,class Calc,class Fi, class Xrandom, class... Xs,class x_random,class... Datas>
auto calculate_parallel_for(const Frq<Id,Calc,Fi,Xrandom,Xs...>& , x_random&& mt,Datas&&... d)
{

  auto res=((find_in(Fi{},std::forward<Datas>(d)...)&&find_in(non_const<Xrandom>{},std::forward<x_random>(mt)))&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e= Error(Arguments_not_found(Id{},res));
    return e;
  }else
  {

    auto  out=apply_random_calculate_parallel_for(Calc{},get_from(Fi{},d...),get_from(non_const<Xrandom>{},std::forward<x_random>(mt)),get_from(Xs{},d...)...);

    return x_i(Id{},std::move(out));
  }
}



template<class Id,class Calc,class Fi, class Xrandom, class Posi,class... Xs,class x_random,class... Datas>
auto calculate_parallel_for(const Frq<pos<Id>,Calc,Fi,Xrandom,Posi,Xs...>& , x_random&& x,Datas&&... d)
{

  auto res=((find_in(Fi{},std::forward<Datas>(d)...)&&find_in(non_const<Xrandom>{},std::forward<x_random>(x)))&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    return Error(Arguments_not_found(Id{},res));
  }else
  {

    auto  out=apply_random_calculate_pos_parallel_for(Calc{},get_from(Fi{},d...),get_from(non_const<Xrandom>{},std::forward<x_random>(x)),Posi{},get_from(Xs{},std::forward<Datas>(d)...)...);

    return x_i(Id{},std::move(out));
  }
}

template<class Id,class G, class... Xs,class... Datas>
auto calculate_parallel_for(const F<Id,G,Xs...>& f, Datas&&... d)
{
  auto res=(find_in(Xs{},std::forward<Datas>(d)...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    // using testId=typename Id::failstest_Id;
    // using data=typename Cs<Datas...>::data;
    // using test=typename Cs<Id,Xs...>::fails_Xs;
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
   }else
  {
    // using testId=typename Id::test_Id;
    // using test=typename Cs<Id,Xs...>::pass_Xs;

    return myInvoke_parallel_for(f,std::forward<Datas>(d)...);
  }
}




template<class Id,class IOld, class... Datas>
auto calculate_parallel_for(const View<Id,IOld>& , Datas&&... d)
{
  auto res=find_in(IOld{},std::forward<Datas>(d)...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e= Error(Arguments_not_found(Id{},res));
    return e;
  }else
  {
    // using testId=typename Id::test_Id;
    // using test=typename Cs<Id,Xs...>::pass_Xs;

    return x_i_view_const(Id{},get_from(IOld{},std::forward<Datas>(d)...));
  }
}

template <class Id, class F, class Distribution, class G, class Xrandom,class ...Xs, class x_random,class... Datas >
auto calculate(const Sampler<Id,F,Distribution,G,Xs...>& f, x_random&& mt, Datas&&... d)
{
  auto res=(find_in(non_const<Xrandom>{},std::forward<x_random>(mt))&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else
  {
    return myInvoke(f,std::forward<x_random>(mt),std::forward<Datas>(d)...);
  }
}


template<class Id,class G, class Xrandom,class... Xs,class x_random,class... Datas>
auto calculate(const Fr<Id,G,Xrandom,Xs...>& f,x_random&& mt, Datas&&... d)
{
  auto res=(find_in(non_const<Xrandom>{},std::forward<x_random>(mt))&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else
  {
      return myInvoke(f,std::forward<x_random>(mt),std::forward<Datas>(d)...);
  }
}


template<class Id,class G, class Xrandom,class... Xs,class... Datas>
auto calculate_parallel_for(const Fr<Id,G,Xrandom,Xs...>& f, Datas&&... d)
{
  auto res=(find_in(non_const<Xrandom>{},std::forward<Datas>(d)...)&&...&&find_in(Xs{},std::forward<Datas>(d)...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;

  }else

  {
    //  using testId=typename Id::test_Id;
    //  using test=typename Cs<Id,Xs...>::pass_Xs;

    return myInvoke_parallel_for(f,std::forward<Datas>(d)...);
  }
}

template <class...Fs,class... Ids,class...Datas>
auto logP(const quimulun<Fs...>& qui, myselect<Ids...>, Datas&&... d)
{
  auto s=(Invoke_on_this(logProbability{},qui[Ids{}],qui,std::forward<Datas>(d)...)+...);
  return s;
}


template <class...Fs,class... Ids,class...Datas>
auto FIM(const quimulun<Fs...>& qui, myselect<Ids...>, Datas&&... d)
{
  auto s=(FIM(qui[Ids{}],qui,std::forward<Datas>(d)...)+...);
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
auto Parameters(const quimulun<Fs...>& ,const vector_space<ds...>& ,Sim&& simulation_results)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;
  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return  std::forward<Sim>(simulation_results)|Parameters_ids{};
}

template <class...Fs,class...ds, class Sim, typename =std::enable_if_t<is_this_template_class<vector_space,std::decay_t<Sim>>::value>>
auto Variables(const quimulun<Fs...>& ,const vector_space<ds...>& ,Sim&& simulation_results)
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
auto calculate_Id(const quimulun<Fs...>& qui, Id, Datas&&...d)
{
  return calculate(qui[Id{}],qui,std::forward<Datas>(d)...);
}

template < class... Fs,class Id,class...Ds2,  class... Datas>
auto calculate_Id_parallel_for(const quimulun<Fs...>& qui, Id, Datas&&...d)
{
  return calculate_parallel_for(qui[Id{}],qui,std::forward<Datas>(d)...);
}


template <class...Fs,class...ds,class... Var_ids, class...Datas >
auto calculate(const quimulun<Fs...>& qui, myselect<Var_ids...>, Datas&&... d)
{
  return (vector_space<>{}+...+calculate_Id(qui,Var_ids{},std::forward<Datas>(d)...));
}
template <class...Fs,class...ds,class... Var_ids, class...Datas >
auto calculate_parallel_for(const quimulun<Fs...>& qui, myselect<Var_ids...>,  Datas&&... d)
{
  return (vector_space<>{}+...+calculate_Id_parallel_for(qui,Var_ids{},std::forward<Datas>(d)...));
}


template <class...Fs,class...ds,class... Var_ids, class...Datas >
auto calculate_set(const quimulun<Fs...>& qui, myselect<Var_ids...>, Datas&&... d)
{
  return (vector_space<>{}+...+calculate_Id(qui,Var_ids{},std::forward<Datas>(d)...));
}




template < class... Fs,class Id, class Random, class... Datas>
auto sample_Id(const quimulun<Fs...>& qui, Random& mt,Id, Datas&&...d)
{
  return Invoke_on_this(Sample{},qui[Id{}],mt,qui,std::forward<Datas>(d)...);
}


template <class...Fs,class...ds, class x_random,class... Par_ids, class...Datas >
auto sample(const quimulun<Fs...>& qui, x_random& mt,myselect<Par_ids...>,Datas&&...d)
{
 // std::cerr<<"sample: &mt="<<&mt<<"\n";

  return (vector_space<>{}+...+sample_Id(qui,mt,Par_ids{},std::forward<Datas>(d)...));
}



template <class...Fs,class...ds,class...Datas, class...Var_ids, class...var_ids>
auto calculate(const quimulun<Fs...>& qui,
               myselect<Var_ids...>,vector_space<var_ids...>&& variables, Datas&&...d)
{
  auto new_variables=calculate_set(qui,myselect<Var_ids...>{},variables,std::forward<Datas>(d)...);


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(new_variables);
  //    return check_if_reacheable(Cs<Var_ids...>{});
    }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return calculate(qui,new_var_ids{},std::move(variables)+std::move(new_variables),std::forward<Datas>(d)...);

  }
}



template <class...Fs,class...ds, class...Datas>
auto calculate(const quimulun<Fs...>& qui,  Datas&&...d)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;
  return calculate(qui,Variables_ids{}, vector_space<>{},std::forward<Datas>(d)...);
}



template <class...Fs,class...ds,class... Var_ids, class...Datas >
auto calculate_set_parallel_for(const quimulun<Fs...>& qui, myselect<Var_ids...>, Datas&&... d)
{
  return (vector_space<>{}+...+calculate_Id_parallel_for(qui,Var_ids{},std::forward<Datas>(d)...));
}


template <class...Fs,class...ds,class... Datas, class...Var_ids, class...var_ids>
auto calculate_parallel_for(const quimulun<Fs...>& qui,
               myselect<Var_ids...>,vector_space<var_ids...>&& variables, Datas&&... d)
{
  auto new_variables=calculate_set_parallel_for(qui,myselect<Var_ids...>{},variables,std::forward<Datas>(d)...);


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(new_variables);
  //    return check_if_reacheable(Cs<Var_ids...>{});
  }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return calculate_parallel_for(qui,new_var_ids{},std::move(variables)+std::move(new_variables),std::forward<Datas>(d)...);

  }
}



template <class...Fs,class...ds, class...Datas>
auto calculate_parallel_for(const quimulun<Fs...>& qui, Datas&&...d)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;
  return calculate_parallel_for(qui,Variables_ids{}, vector_space<>{},std::forward<Datas>(d)...);
}



template < class Id,class... Fs,class x_random,class...Ds2,  class... Datas>
auto random_calculate_Id(const quimulun<Fs...>& qui,x_random&& mt, Id,Datas&&...d)
{
  return calculate(qui[Id{}],std::forward<x_random>(mt),qui,std::forward<Datas>(d)...);
}

template < class... Fs,class x_random,class Id,class...Ds2,  class... Datas>
auto random_calculate_Id_parallel_for(const quimulun<Fs...>& qui,x_random&& mt, Id, Datas&&...d)
{
  //using test=typename Cs<Datas...>::ger;
  return calculate_parallel_for(qui[Id{}],std::forward<x_random>(mt),qui,std::forward<Datas>(d)...);
}


template <class...Fs,class x_random,class...ds,class... Var_ids, class...Datas >
auto random_calculate_this(const quimulun<Fs...>& qui, x_random&& mt,myselect<Var_ids...>,  Datas&&... d)
{
  //std::cerr<<"random_calculate_this: &mt="<<&mt<<"\n";
  return (vector_space<>{}+...+random_calculate_Id(qui,std::forward<x_random>(mt),Var_ids{},std::forward<Datas>(d)...));
}


template <class...Fs,class x_random,class...ds,class... Var_ids, class...Datas >
auto random_calculate_this_parallel_for(const quimulun<Fs...>& qui, x_random&& mt,myselect<Var_ids...>, Datas&&... d)
{
  return (vector_space<>{}+...+random_calculate_Id_parallel_for(qui,std::forward<x_random>(mt),Var_ids{},std::forward<Datas>(d)...));
}



template <class...Fs,class x_random,class...ds,class... Datas, class...Var_ids, class...var_ids>
auto random_calculate(const quimulun<Fs...>& qui,
                      x_random&& mt,
               myselect<Var_ids...>,
                      vector_space<var_ids...>&& variables, Datas&&... d)
{
  auto new_variables=random_calculate_this(qui,std::forward<x_random>(mt),myselect<Var_ids...>{},variables,std::forward<Datas>(d)...);
  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(new_variables);
  }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return random_calculate(qui,std::forward<x_random>(mt),new_var_ids{},std::move(variables)+std::move(new_variables),std::forward<Datas>(d)...);

  }
}


template <class...Fs,class x_random,class...ds,class... Datas, class...Var_ids, class...var_ids>
auto random_calculate_parallel_for(const quimulun<Fs...>& qui,
                      x_random&& mt,
                      myselect<Var_ids...>,
                      vector_space<var_ids...>&& variables, Datas&&... d)
{
  auto new_variables=random_calculate_this_parallel_for(qui,std::forward<x_random>(mt),myselect<Var_ids...>{},variables,std::forward<Datas>(d)...);
  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(variables);
    else
      return check_if_reacheable(new_variables);

  }
  else
  {
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return random_calculate_parallel_for(qui,std::forward<x_random>(mt),new_var_ids{},std::move(variables)+std::move(new_variables),std::forward<Datas>(d)...);

  }
}





template <class...Fs,class x_random,class...ds, class...Datas>
auto random_calculate(const quimulun<Fs...>& qui,x_random&& mt, Datas&&...d)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return random_calculate(qui,std::forward<x_random>(mt),Variables_ids{}, vector_space<>{},std::forward<Datas>(d)...);


}


template <class...Fs,class x_random,class...ds, class...Datas>
auto random_calculate_parallel_for(const quimulun<Fs...>& qui,x_random&& mt,  Datas&&...d)
{
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return random_calculate_parallel_for(qui,std::forward<x_random>(mt),Variables_ids{}, vector_space<>{},std::forward<Datas>(d)...);


}





template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class... par_ids, class...var_ids>
auto sample_parameters_calculate_variables(const quimulun<Fs...>& qui,  x_random&& mt,
            myselect<Par_ids...>,myselect<Var_ids...>,
            vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables,Datas&&... d)
{
  auto new_parameters=sample(qui,std::forward<x_random>(mt),myselect<Par_ids...>{},parameters,variables,std::forward<Datas>(d)...);
  auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,std::forward<Datas>(d)...);


  if constexpr (new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)==0)
      return std::move(parameters)+std::move(variables);
    else
      return check_if_reacheable(new_parameters+new_variables);
 //     return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>>{});
  }
  else
  {

    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    return sample_parameters_calculate_variables(qui,std::forward<x_random>(mt),new_par_ids{},new_var_ids{},
                                                 std::move(parameters)+clean_Error<vector_space<>>(std::move(new_parameters)),
                                                 std::move(variables)+clean_Error<vector_space<>>(std::move(new_variables)),std::forward<Datas>(d)...);

  }
}



template <class...Fs,class...Datas, class x_random>
auto sample(const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,pack_concatenation_t<typename std::decay_t<Datas>::myIds...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return sample_parameters_calculate_variables(qui,mt,Parameters_ids{},Variables_ids{}, vector_space<>{}, vector_space<>{},std::forward<Datas>(d)...);


}


template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class... par_ids, class...var_ids>
auto sampleParameters(const quimulun<Fs...>& qui,  x_random& mt,
                      myselect<Par_ids...>,myselect<Var_ids...>,
                      vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables,  Datas&&...d)
{
  auto new_parameters=sample(qui,mt,myselect<Par_ids...>{},parameters,variables,std::forward<Datas>(d)...);
  using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
  if constexpr (new_par_ids::size==0)
    return std::move(parameters)+std::move(new_parameters);
  else
  {
    auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,std::forward<Datas>(d)...);
    if constexpr (new_parameters.size()+new_variables.size()==0)
      return check_if_reacheable(new_parameters);
//      return check_if_reacheable(Cs<Par_ids...>{});
    else
    {
      using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
      return sampleParameters(qui,mt,new_par_ids{},new_var_ids{},
                    std::move(parameters)+std::move(new_parameters),
                    std::move(variables)+std::move(new_variables),std::forward<Datas>(d)...);

    }
  }
}



template <class...Fs,class...Datas, class x_random>
auto sampleParameters(const quimulun<Fs...>& qui, x_random& mt, Datas&&... d )
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids, pack_concatenation_t<typename std::decay_t<Datas>::myIds...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return sampleParameters(qui,mt,Parameters_ids{},Variables_ids{}, vector_space<>{}, vector_space<>{},std::forward<Datas>(d)...);


}



template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class ... Pred_ids ,class... par_ids, class...var_ids,class ... pred_ids >
auto simulate(const quimulun<Fs...>& qui,  x_random& mt,myselect<Par_ids...>,myselect<Var_ids...>,myselect<Pred_ids...>,
              vector_space<par_ids...>&& parameters, vector_space<var_ids...>&& variables, vector_space<pred_ids...>&& predictions, Datas&&...d)
{
  auto new_parameters=sample(qui,mt,myselect<Par_ids...>{},parameters,variables,predictions,std::forward<Datas>(d)...);
  auto new_variables=calculate(qui,myselect<Var_ids...>{},parameters,variables,predictions,std::forward<Datas>(d)...);
  auto new_predictions=sample(qui,mt,myselect<Pred_ids...>{},parameters,variables,predictions,std::forward<Datas>(d)...);


  if constexpr (new_parameters.size()+new_variables.size()+new_predictions.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)+sizeof... (Pred_ids)==0)
      return std::make_tuple(std::move(parameters),std::move(variables),std::move(predictions));
    else
      return check_if_reacheable(new_parameters+new_variables+new_predictions);
  //  return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>,Cs<Pred_ids...>>{});
  }
  else
  {
    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;
    using  new_pred_ids=transfer_t<pack_difference_t<Cs<Pred_ids...>,typename decltype (new_predictions)::myIds>,myselect<>>;
    return simulate(qui,mt,new_par_ids{},new_var_ids{},new_pred_ids{},
                    std::move(parameters)+std::move(new_parameters),
                    std::move(variables)+std::move(new_variables),
                    std::move(predictions)+std::move(new_predictions),std::forward<Datas>(d)...);

  }
}


template <class...Fs,class...Datas, class x_random>
auto simulate(const quimulun<Fs...>& qui,  x_random& mt, Datas&&... d)
{
  using distribution_ids=extract_distribution_Id_t<Cs<Fs...>>;
  using prediction_ids=pack_intersection_t<distribution_ids,pack_concatenation_t<typename std::decay_t<Datas>::myIds...>>;
  using variables_ids=extract_function_Id_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,prediction_ids>,myselect<>>;
  using Prediction_ids=transfer_t<prediction_ids,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return simulate(qui,mt,Parameters_ids{},Variables_ids{},Prediction_ids{}, vector_space<>{}, vector_space<>{}, vector_space<>{},std::forward<Datas>(d)...);


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
      return check_if_reacheable(new_variables);
   //   return check_if_reacheable(Cs<Var_ids...>{});
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



#endif // QM_TENSOR_MODEL_EVALUATION_H
