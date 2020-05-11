#ifndef QM_TENSOR_MODEL_OPERATIONS_NEW_H
#define QM_TENSOR_MODEL_OPERATIONS_NEW_H

#include "qm_tensor_model_evaluation_new.h"
template<class ...> struct Operation_for;
template<class ...> struct Operation_Coord;

template<class ...> struct Operation_sum;
template<class ...> struct Operation_serial;
template<class ...> struct Operation_non_serial;


template <class anId, class...Ops>
auto get_type_xi_from_this_new(anId, const Operation_non_serial<Ops...>& xi)->decltype (xi[anId{}])
{
  //  using test=typename std::conditional_t<is_this_template_class_v<Current,anId>,
  //                                           Cs<anId,decltype (xi[anId{}]),
  //                                              decltype (xi)
  //                                              >, std::vector<double>>::value_type;

  return xi[anId{}];
}

template <class anId, class...Ops>
auto get_type_xi_from_this_new(anId,const Operation_serial<Ops...>& xi)->decltype (xi[anId{}])
{
  // using testOperation=typename anId::testOperation;
  // using test2=typename decltype(xi[anId{}])::test;


  //  using test=typename std::conditional_t<is_this_template_class_v<Current,anId>,
  //                                           Cs<anId,decltype (xi[anId{}]),
  //                                              decltype (xi)
  //                                              >, std::vector<double>>::value_type;


  return xi[anId{}];
}



template <class anId, class V
          , typename =std::enable_if_t<
              (!is_this_template_class_v<Operation_serial,V>)
              && (!(only_xi_of_fi_v<V>)||(is_this_template_class_v<x_i_vector_field_const,V>))
              > >
auto get_type_xi_from_this_new(anId, V&& xi)
{
  //  using test=typename std::conditional_t<is_this_template_class_v<Current,anId>,
  //                                           Cs<anId,decltype (get_at(std::forward<V>(xi),anId{})),
  //                                              decltype (std::forward<V>(xi))
  //                                              >, std::vector<double>>::value_type;
  // using test2=typename decltype(xi[anId{}])::test;
  return Result_t<decltype(only_xi_or_fi(get_at(std::forward<V>(xi),anId{})))>{};
}


template <class anId, class V
          , typename =std::enable_if_t<
              (!is_this_template_class_v<Operation_serial,V>)
              && (only_xi_of_fi_v<V>)
              &&(!is_this_template_class_v<x_i_vector_field_const,V>)
              > >
auto get_type_xi_from_this_new(anId, V&& xi)->std::conditional_t<(std::is_same_v<anId,typename std::decay_t<V>::myId >),
                                                                   Result_t<decltype (xi)>,Nothing>
{
  if constexpr (std::is_same_v<anId,typename std::decay_t<V>::myId >)
    return Result_t<decltype (xi)>{};
  else
    return Nothing{};

}



template <class anId, class...Datas
          , typename =std::enable_if_t<
              ( !is_any_of_these_template_classes<all,Size_at_Index_new,pos_new,all_new,sub,subElement,Size,pass_id, std::tuple>::template value<anId>) >>
decltype(auto) get_type_from_new(anId, Datas&&...ds)
{
  //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
  return (get_type_xi_from_this_new(anId{},std::forward<Datas>(ds))||...);
}



template<class... xis>
constexpr auto toResult_tuple(Result<xis>...){ return Result<x_i_tuple_view_const<std::decay_t<xis>...>>{};}




template <class... anId, class...Datas>
auto get_type_from_new(std::tuple<anId...>, Datas&&...ds)//->decltype ((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...))
{

  if constexpr ((std::is_same_v<decltype (get_type_from_new(anId{},std::forward<Datas>(ds)...)),Nothing > ||...))
  {   //  using test=typename Cs<anId...>::FAILS;
    // using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::FAILS;
    return Nothing{};
  }else
  {
    // using test=typename decltype (toResult_tuple(get_type_from_new(anId{},std::forward<Datas>(ds)...)...))::test;
    //     using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::OK;
    //    return x_i(Cs<anId...>{},v(std::forward_as_tuple(get_from(anId{},ds...)...),dimension_less{}));
    return toResult_tuple(get_type_from_new(anId{},std::forward<Datas>(ds)...)...);
  }
}


template <class Id, class Index, class...Datas>
auto get_type_from_new(Size_at_Index_new<Id, Index>, Datas&&...ds)
{
  auto x=get_type_from_new(Id{},std::forward<Datas>(ds)...);
  using field_type=decltype (std::declval<typename decltype (x)::result_type>()());
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
  {
    //    using tes=typename Cs<Datas...>::data;
    //    using test=typename decltype(x)::tew;
    return Result<vector_field_size<field_type,Index>>{};
  }
}

template <class Id,  class...Datas>
auto get_type_from_new(Size<Id>, Datas&&...ds)
{
  //using test=typename Id::id;
  auto x=get_type_from_new(Id{},std::forward<Datas>(ds)...);
  //using test3=typename decltype (x)::x;

  using vector_type=std::decay_t<decltype (std::declval<typename decltype (x)::result_type>()())>;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //  using test2=typename Cs<decltype(std::forward<Datas>(ds)[Id{}])...>::tew;
    return Result<vector_space_size<vector_type>>{};
}




template <class Id,  class...Datas>
auto get_type_from_new(pos_new<Id>, Datas&&...ds)
{
  auto x=(get_type_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);
  using field_type=std::decay_t<decltype (std::declval<typename decltype (x)::result_type>()())>;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    return Result<vector_field_Position<Id,field_type>>{};
}






template <class Id,  class  IndexId, class...Datas>
auto get_type_from_new(subElement<Id,IndexId>, Datas&&...ds)
{
  auto x=get_type_from_new(Id{},std::forward<Datas>(ds)...);
  auto p=get_type_from_new(IndexId{},std::forward<Datas>(ds)...);

  using field_type=std::decay_t<decltype (std::declval<typename decltype (x)::result_type>()())>;
  using index_type=std::decay_t<decltype (std::declval<typename decltype (p)::result_type>()())>;

  // using test=typename Cs<Id, IndexId, decltype(x), decltype(p)>::test;
  //  using test2=typename Cs<Id, IndexId, decltype(p)>::test;

  if constexpr(std::is_same_v<decltype (x),Nothing >||std::is_same_v<decltype (p),Nothing >)
    return Nothing{};
  else
    return Result<vector_field_subElement<field_type,index_type>>{};
}


template <class Id,  class  IndexId, class...Datas>
auto get_type_from_new(sub<Id,IndexId>, Datas&&...ds)
{

  auto x=get_type_from_new(Id{},std::forward<Datas>(ds)...);

  using vector_type=std::decay_t<decltype (std::declval<typename decltype (x)::result_type>()())>;


  //  using test=typename std::conditional_t<is_this_template_class_v<non_const,Id>,Cs<Id,vector_type>, std::vector<double>>::value_type;
  //  using test2=typename std::conditional_t<is_this_template_class_v<non_const,Id>,Cs<IndexId,vector_type,decltype (get_type_xi_from_this_new(IndexId{},std::declval<vector_type&>()))>,std::vector<double>>::value_type;
  if constexpr (std::is_same_v<vector_type,Nothing >)
    return Nothing{};
  else
  {
    // using test=typename Cs<Id,IndexId, decltype (x), decltype (get_type_xi_from_this_new(IndexId{},std::declval<vector_type&>()))>::jajaj;

    return decltype(get_type_xi_from_this_new(IndexId{},std::declval<vector_type&>())){};

  }
}


template <class Id,  class...Datas>
auto get_type_from_new(all_new<Id>, Datas&&...ds)
{
  auto x=(get_type_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);
  using field_type=std::decay_t<decltype (std::declval<typename decltype (x)::result_type>())>;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    return Result<all_view_const_new<field_type>>{};
}



template <class anId, class...Datas, typename=std::enable_if_t<!is_this_template_class_v<pass_id,anId>>>
auto find_type_in_new(anId, Datas&&...ds)
{


  if constexpr (std::is_same_v<decltype (get_type_from_new(anId{},std::forward<Datas>(ds)...)),Nothing >)
    return Not_found<anId>{};
  else
    return Has_been_found{};
}


template<class...>struct Operation_vector{};

template<class Operator, class Id> struct Operation{};




template<>
struct Operation_non_serial<>
{

  using myResult_type=Nothing;

  static constexpr auto size() { return 0;}

  using myIds=Cs<>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    return Nothing{};
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& , Datas&&...)
  {
    return Nothing{};
  }

};



template<class Operation, class ...Operations>
struct Operation_non_serial<Operation,Operations...>
{

  using myResult_type=
      //std::conditional_t<(sizeof... (Operations) >0),
      vector_tuple<typename Operation::myResult_type,typename Operations::myResult_type... >
      //                  ,typename Operation::myResult_type>
      ;

  static constexpr auto size() { return (Operation::size()+...+Operations::size());}

  using myIds=pack_concatenation_t<typename Operation::myIds,typename Operations::myIds...>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    return (Operation{}[Id{}]||...||Operations{}[Id{}]);
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    return (Operation{}(qui,std::forward<Datas>(d)...)+...+Operations{}(qui,std::forward<Datas>(d)...));
  }

};

template<class Operation,class ...Operations>
struct Operation_serial<Operation,Operations...>
{

  using myResult_type=
      //    std::conditional_t<(sizeof... (Operations) >0),
      vector_tuple<typename Operation::myResult_type,typename Operations::myResult_type... >
      //                     ,typename Operation::myResult_type>
      ;

  using myIds=pack_concatenation_t<typename Operation::myIds,typename Operations::myIds...>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    //using test=typename Id::test;
    //using test2=typename Cs<decltype (Operation{}[Id{}]),decltype (Operations{}[Id{}])...>::test;
    return (Operation{}[Id{}]||...||Operations{}[Id{}]);
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    auto out0=Operation{}(qui,std::forward<Datas>(d)...);
    auto const& vec=out0;
    auto out1=Operation_serial<Operations...>{}(qui,vec,std::forward<Datas>(d)...);
    return std::move(out0)+std::move(out1);
  }

};


template<class... Operation_non_serials,class... Operation_fors >
auto operator+(Operation_serial<Operation_non_serials...>,Operation_non_serial<Operation_fors...> )
{
  return  Operation_serial<Operation_non_serials..., Operation_non_serial<Operation_fors...>>{};
}

template<template<class, class>class x_i,class Id, class value_type, class... Xs>
constexpr auto toResult(vec<Xs...>,Result<x_i<Id,value_type>>)
{
  return Result<x_i<Id,vector_field<vec<Xs...>,value_type>>>{};
}

template< class... Xs>
constexpr auto toResult(vec<Xs...>,Result<Nothing>)
{
  return Nothing{};
}



template< class ...x_is, class... Xs, class... Ops,class... Fids, template<class...>class Arguments,class ...Args>
struct Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>

{


  template<class Id>
  constexpr auto operator[](Id)const
  {
    // using test=typename std::conditional_t<is_this_template_class_v<non_const,Id>,decltype (std::declval<vector_space<x_is...>>()[Id{}]),std::vector<double>>::value_type;

    if constexpr (sizeof... (Xs)>0)
      return toResult(vec<Xs...>{},Result<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{});
    else
      return  Result_t<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{};



  }

  constexpr static auto size(){ return sizeof... (x_is);}

  using myIds=Cs<typename x_is::myId...>;


  using myResult_type=std::conditional_t< (sizeof...(Xs)>0 ),
                                           vector_field<vec<Xs...>,vector_space<x_is...>>
                                           ,vector_space<x_is...>>
      ;
  using myValue_type= vector_space<x_is...>;
  using myvec=vec<Xs...>;



};


template< class ...x_is, class... Xs, class... Ops,class... Fids, template<class...>class Arguments,class ...Args>
struct Operation_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>

{


  template<class Id>
  constexpr auto operator[](Id)const
  {
    // using test=typename std::conditional_t<is_this_template_class_v<non_const,Id>,decltype (std::declval<vector_space<x_is...>>()[Id{}]),std::vector<double>>::value_type;

    if constexpr (sizeof... (Xs)>0)
      return toResult(vec<Xs...>{},Result<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{});
    else
      return  Result_t<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{};



  }

  constexpr static auto size(){ return sizeof... (x_is);}

  using myIds=Cs<typename x_is::myId...>;


  using myResult_type=std::conditional_t< (sizeof...(Xs)>0 ),
                                           vector_field<vec<Xs...>,vector_space<x_is...>>
                                           ,vector_space<x_is...>>
      ;
  using myValue_type= vector_space<x_is...>;
  using myvec=vec<Xs...>;



};



template< class value_type,  class... Ops,class... Fids, class ...Args>
struct Operation_sum<Result<value_type>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>

{


  template<class Id>
  constexpr auto operator[](Id)const
  {
    return Nothing{};
  }

  constexpr static auto size(){ return 1;}

  using myIds=Cs<>;


  using myResult_type=value_type;
  using myValue_type= value_type;
  using myvec=vec<>;



};


template<class Op,class Id, class Idinside, class value_type, class... Xs, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new>::value<Id>>>
constexpr auto make_Operation_for(Op,sub<Id, Idinside>,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{

  return Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,vector_space<x_i<Idinside,value_type> > > > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};

template<class Op,class Id, class value_type, class... Xs, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new,sub>::value<Id>>>
constexpr auto make_Operation_Coord(Op,Id,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  // using test=typename Id::test;
  //   using test2=typename value_type::test2;


  return Operation_Coord< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};






template<class Op,class Id, class value_type, class... Xs, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new,sub>::value<Id>>>
constexpr auto make_Operation_for(Op,Id,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  // using test=typename Id::test;
  //   using test2=typename value_type::test2;


  return Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};


template<class Op,class Id, class nstep,class value_type, class... Xs, class... Args>
constexpr auto make_Operation_for(Op,Start_new<Id,nstep>,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  //using test=typename Id::test;
  //   using test2=typename value_type::test2;

  return //Operation_serial<
      Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};

template<class Op,class Id,class nstep, class value_type, class... Args>
constexpr auto make_Operation_for(Op,Start_new<Id,nstep>,Result<value_type>,Arguments<Args...>)
{
  //  using test=typename Id::test;
  //   using test3=typename Op::Op;
  //   using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};




template<class Op,class Id, class nstep,class value_type, class... Xs, class... Args>
constexpr auto make_Operation_for(Op,Next_new<Id,nstep>,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  //using test=typename Id::test;
  //   using test2=typename value_type::test2;

  return //Operation_serial<
      Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};

template<class Op,class Id,class nstep, class value_type, class... Args>
constexpr auto make_Operation_for(Op,Next_new<Id,nstep>,Result<value_type>,Arguments<Args...>)
{
  //  using test=typename Id::test;
  //   using test3=typename Op::Op;
  //   using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};



template<class Op,class Id,class Idinside, class value_type, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new>::value<Id>>>
constexpr auto make_Operation_for(Op,sub<Id,Idinside>,Result<value_type>,Arguments<Args...>)
{
  using test=typename Id::test;
  //   using test3=typename Op::Op;
  //   using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,vector_space< x_i<Idinside,value_type>>> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};



template<class Op,class Id, class value_type, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new,sub>::value<Id>>>
constexpr auto make_Operation_for(Op,Id,Result<value_type>,Arguments<Args...>)
{
  //  using test=typename Id::test;
  //   using test3=typename Op::Op;
  //   using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};


template<class Op,class Id, class value_type, class... Args, typename=std::enable_if_t<!is_any_of_these_template_classes<Start_new,Next_new,sub>::value<Id>>>
constexpr auto make_Operation_for(Op,Id,Result<value_type>,Arguments_xi<Args...>)
{
  //   using test=typename Id::test;
  //   using test3=typename Op::Op;
  //   using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments_xi<Args...>>{};
};







template<class Id, class value_type, class... Args>
constexpr auto make_Operation_for(logProbability,Id,Result<value_type>,Arguments<Args...>)
{
  // using test=typename Id::test;
  // using test2=typename value_type::test2;
  return Operation_sum< Result< x_i<logpr<up<Id>,dn<Args...>>,value_type> >,Operation_vector< Operation<logProbability,Id> > , Arguments<Id, Args...>>{};
};
template<class Id, class value_type, class... Xs, class... Args>
constexpr auto make_Operation_for(logProbability,Id,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  //  using test=typename Id::test;
  //  using test2=typename value_type::test2;

  return Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<logpr<up<Id>,dn<Args...>>,value_type> > >>,Operation_vector< Operation<logProbability,Id> > , Arguments<Args...>>{};
};


template<>
struct Operation_serial<>
{
  template<class Id>
  constexpr auto operator[](Id)const
  {
    return Nothing{};
  }
  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& , Datas&&...)
  {
    return Nothing{};
  }

  using myResult_type=vector_tuple<>;




};



template<class Op,class Id, class F, class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >&&!std::is_same_v<std::decay_t<F>,Conditional_choice >>>
auto apply_calculate_result(Op op,Id, Result<F>,Arguments<Result<x_is>...>)
{
  using myvec= decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{}));

  auto pos=transfer_t<myvec, Position<>>{};




  // using test=typename Cs<Id, myvec,F>::apply_calculate_resul;

  // using test=typename Cs<x_is...>::pos;

  //  using test2=typename decltype (pos)::pos;
  return Result<vector_field<myvec,std::decay_t<decltype(myInvoke(op,std::declval<F>(),std::declval<x_is>()()(pos)...))>>>{};

}

template<class Op, class Id, class... x_is>
auto apply_calculate_result(Op op,Id, Result<Conditional_choice>,Arguments<Result<x_is>...>)
{
  using myvec= decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{}));

  auto pos=transfer_t<myvec, Position<>>{};
  // using test0=typename Cs<Id, get_Field_Indexes_t<x_is>... >::apply_calculate_result_su;
  // using test=typename Cs<Id, myvec,Conditional_choice,decltype(std::declval<x_is&>()())...,decltype(std::declval<x_is&>()()(pos))... >::apply_calculate_result_su;


  return Result<vector_field<myvec,std::decay_t<decltype(myInvoke(op,Conditional_choice{},std::declval<x_is&>()()(pos)...))>>>{};

}



template<class Op,class Id,class F, class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >>>
auto apply_calculate_result_sum(Op op,Id,Result<F>,Arguments<Result<x_is>...>)
{
  using myvec= decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{}));

  auto pos=transfer_t<myvec, Position<>>{};
  // using test=typename Cs<Id, myvec,F>::apply_calculate_result_su;


  return Result<std::decay_t<decltype(myInvoke(op,std::declval<F>(),std::declval<x_is>()()(pos)...))>>{};

}

template<class Op,class F, class Id,class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >>>
auto apply_calculate_coordinate_result(Op op,const F& f,Id, Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  // using test=typename Cs<Id,myvec,F>::apply_calculate_coordinate_resul;

  auto pos=transfer_t<myvec, Position<>>{};
  // using vector_type=std::decay_t<decltype(myInvoke(op,f,std::declval<x_is>()()(pos)...))>;
  // using value_type=typename vector_type::value_type;
  //using test3=typename Cs<Id,decltype (std::declval<x_is>()()(pos))...,value_type,myvec>::declty;

  //  return Result<vector_field<myvec,value_type>>{};
  return Result<vector_field<myvec,vector_field<vec<Id>,typename std::decay_t<decltype(myInvoke(op,f,std::declval<x_is>()()(pos)...))>::value_type>>>{};

}



template<class Op, class Id, class... x_is>
auto apply_calculate_result(Op op,Id, Result<Glue_new>,Arguments<Result<x_is>...>)
{
  using res_value_type=std::decay_t<decltype(myInvoke(op,Glue_new{},std::declval<x_is const &>()...))>;
  return Result<res_value_type>{};
}






template<class Op,class Id, class F, class Random, class... x_is>
auto apply_sample_result(Op op,Id, Result<F>,Random& mt,Arguments<Result<x_is>...>)
{
  // using test=typename Cs<x_is...>::aqui_Index_struct;
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  // using test=typename Cs<Id, myvec,F>::apply_sample_resul;

  auto pos=transfer_t<myvec, Position<>>{};

  return Result<vector_field<myvec,typename std::decay_t<decltype( myInvoke_Operation_Sample(op,std::declval<F>(),mt,std::declval<x_is&>()()(pos)...))>::result_type>>{};
}
template<class Op,class Id, class F, class Random, class... x_is,class... xx_is>
auto apply_sample_result(Op op,Id, Result<F>,Random& mt,Arguments<Result<x_is>...>,Index_struct<Result<xx_is>...>)
{
  //  using test=typename Cs<xx_is...>::aqui_Index_struct;
  typedef decltype (((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})<<...<<get_Field_Indexes_t<xx_is>{})) myvec;
  // using test=typename Cs<Id, myvec,F>::apply_sample_resul;

  auto pos=transfer_t<myvec, Position<>>{};

  return Result<vector_field<myvec,typename std::decay_t<decltype( myInvoke_Operation_Sample(op,std::declval<F>(),mt,std::declval<x_is&>()()(pos)...))>::result_type>>{};
}


template<class Op,class F,class Id, class Random, class ...Xs,class... XXs,class... x_is, class ...xx_is>
auto apply_sample_op_result(Op op,Result<F>,Id, Random& mt,Arguments<Xs...>,Index_struct<XXs...>,Arguments<Result<x_is>...>,Index_struct<Result<xx_is>...> )
{
  typedef decltype (((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})<<...<<get_Field_Indexes_t<xx_is>{})) myvec;
  // using test=typename Cs<Id,myvec,F>::apply_calculate_op_resul;
  auto pos=transfer_t<myvec, Position<>>{};

  //  using tId=typename Id::Id;
  //  using test=typename myvec::test;
  using Operation=std::decay_t<decltype( myInvoke_Sample_Operation(op,std::declval<F>(),mt,std::declval<x_is&>()()(pos)...))>;

  auto Res= Result<vector_field<myvec,Outer_Id_t<typename Operation::myResult_type>>>{};
  // using test2=typename decltype (make_Operation(Operation{},Id{},Res,Arguments<Xs...>{}))::Operation;
  return make_Operation_for(Operation{},Id{},Res,Arguments<Xs...,XXs...>{});

}

template<class Op,class F,class Id, class Random, class ...Xs,class... x_is>
auto apply_sample_op_result(Op op,Result<F>,Id, Random& mt,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;

  //using test=typename Cs<Id,Arguments<Xs...>,myvec,F>::apply_sample_op_resul;

  auto pos=transfer_t<myvec, Position<>>{};


  using Operation=std::decay_t<decltype( myInvoke_Sample_Operation(op,std::declval<F>(),mt,std::declval<x_is&>()()(pos)...))>;

  auto Res= Result<vector_field<myvec,Outer_Id_t<typename Operation::myResult_type>>>{};

  //using testResult=typename Cs<Id,decltype (Res)>::re;
  //using testResultmake=typename Cs<Id,decltype (make_Operation(Operation{},Id{},Res,Arguments<Xs...>{}))>::make;

  return make_Operation_for(Operation{},Id{},Res,Arguments<Xs...>{});

}



template<class Op,class F,class Id, class Random, class ...Xs,class... x_is>
auto apply_sample_op_result(Op op,Result<F>,Id, Random& mt,Arguments_xi<Xs...>,Arguments_xi<Result<x_is>...>)
{
  // typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;

  // using test=typename Cs<Id,F>::apply_sample_op_resul;

  // auto pos=transfer_t<myvec, Position<>>{};


  using Operation=std::decay_t<decltype( myInvoke_Sample_Operation(op,std::declval<F>(),mt,std::declval<x_is&>()...))>;

  auto Res= Result<Outer_Id_t<typename Operation::myResult_type>>{};
  return make_Operation_for(Operation{},Id{},Res,Arguments_xi<Xs...>{});

}






template<class Id,class Distribution, class... Xs,class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const D<Id,Distribution,Arguments<Xs...>>& ,Rnd & mt,const Datas&...ds)
{
  auto res=(find_type_in_new(Xs{},ds...)&&...&&Has_been_found{});
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=Result<Distribution>{};
    auto Res=apply_sample_result(Sample{},Id{},d,mt,Arguments(get_type_from_new(Xs{},ds...)...));
    return make_Operation_for(Sample{},Id{},Res,Arguments<Xs...>{});
  }

}


template<class Id,class Distribution, class... Xs, class... XXs,class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>> ,Rnd & mt,const Datas&...ds)
{
  auto res=((...&&find_type_in_new (Xs{},ds...))&&...&&find_type_in_new (XXs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=Result<Distribution>{};
    auto Res=apply_sample_result(Sample{},Id{},d,mt,Arguments(get_type_from_new(Xs{},ds...)...),Index_struct(get_type_from_new(XXs{},ds...)...));
    using ArgsOut=transfer_t<pack_difference_t<Cs<XXs...>,Cs<Xs...>>,Arguments<Xs...>>;
    // using test=typename decltype (make_Operation(Sample{},Id{},Res,ArgsOut{}))::D_struct;
    return make_Operation_for(Sample{},Id{},Res,ArgsOut{});
  }

}


template<class Id,class Sampler,class Fi, class... Xs,
          class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const Dq_new<Id,Sampler,Fi,Arguments<Xs...>>& ,Rnd & mt,const Datas&...ds)
{
  //using test=typename Id::id;
  auto res=(find_type_in_new (Fi{},ds...)&&...&&find_type_in_new (Xs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto f=get_type_from_new(Fi{},ds...);
    return apply_sample_op_result(Sampler{},f,Id{},mt,Arguments<Xs...>{},Arguments(get_type_from_new(Xs{},ds...)...));
  }

}

template<class Id,class Sampler,class Fi, class... Xs,
          class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const Dq_new<Id,Sampler,Fi,Arguments_xi<Xs...>>& ,Rnd & mt,const Datas&...ds)
{
  //using test=typename Id::id;
  auto res=(find_type_in_new (Fi{},ds...)&&...&&find_type_in_new (Xs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto f=get_type_from_new(Fi{},ds...);
    return apply_sample_op_result(Sampler{},f,Id{},mt,Arguments_xi<Xs...>{},Arguments_xi(get_type_from_new(Xs{},ds...)...));
  }

}







template<class Id,class Sampler,class Fi, class... Xs,class ...XXs,
          class... Fs,class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const Dq_new<Id,Sampler,Fi,Arguments<Xs...>,Index_struct<XXs...>>,Rnd & mt,const Datas&...ds)
{
  auto res=((find_type_in_new (Fi{},ds...)&&...&&find_type_in_new (Xs{},ds...))&&...&&find_type_in_new (XXs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto f=get_type_from_new(Fi{},ds...);
    return apply_sample_op_result(Sampler{},f,Id{},mt,Arguments<Xs...>{},Index_struct<XXs...>{},Arguments(get_type_from_new(Xs{},ds...)...),
                                  Index_struct(get_type_from_new(XXs{},ds...)...));
  }

}




template<class... Fs,class Id,class G, class... Xs, class...Datas>
auto Calculate_Operation_on_this(Calculator_new,const Coord_new<Id,G,Arguments<Xs...>>& coord,const Datas&...ds)
{
  auto res=(find_type_in_new(Xs{},ds...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    // using test=typename Id::Id;
    // using test2=typename G::G;



    auto d=coord.get_G();
    auto Res=apply_calculate_coordinate_result(Calculator_new{},d,Id{},Arguments(get_type_from_new(Xs{},ds...)...));
    //using test=typename Cs<Id,decltype (Res),decltype (make_Operation_Coord(Calculator_new{},Id{},Res, Arguments<Xs...>{}))>::jajaja;
    return make_Operation_Coord(Calculator_new{},Id{},Res, Arguments<Xs...>{});
  }

}

template<class Op,class F, class Id,class ...Xs,class... x_is,
          typename=std::enable_if_t<(!std::is_same_v<std::decay_t<F>,Glue_new >)&&(!std::is_same_v<Op,logPrior_new >)&&(!std::is_same_v<Op,logLikelihood_new >)>>
auto apply_calculate_op_result(Op op,Result<F> ,Id,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  //using test=typename Op::Op;
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  //using test=typename Cs<Id,myvec,F>::apply_calculate_op_resul;

  auto pos=transfer_t<myvec, Position<>>{};
  using  Operation=std::decay_t<decltype(myInvoke_Calculate_Operation(op,std::declval<F>(),std::declval<x_is&>()()(pos)...))>;
  auto res= Result<vector_field<myvec,Outer_Id_t<typename Operation::myResult_type>>>{};
  return make_Operation_for(Operation{},Id{},res,Arguments<Xs...>{});

}
template<class F, class Id,class ...Xs,class... x_is, typename=std::enable_if_t<(!std::is_same_v<std::decay_t<F>,Glue_new >)>>
auto apply_calculate_op_result(logPrior_new op,Result<F> ,Id,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  // using test=typename Id::logPrior_new;
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  //using test=typename Cs<logPrior_new,Id,myvec,F>::apply_calculate_op_resul;


  auto pos=transfer_t<myvec, Position<>>{};
  using  Operation=std::decay_t<decltype(myInvoke_Calculate_Operation(op,std::declval<F>(),std::declval<x_is&>()()(pos)...))>;
  auto res= Result<vector_field<myvec,typename Operation::myResult_type>>{};
  return make_Operation_for(Operation{},Id{},res,Arguments<Xs...>{});

}
template<class F, class Id,class ...Xs,class... x_is, typename=std::enable_if_t<(!std::is_same_v<std::decay_t<F>,Glue_new >)>>
auto apply_calculate_op_result(logLikelihood_new op,Result<F> ,Id,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;


  auto pos=transfer_t<myvec, Position<>>{};
  using  Operation=std::decay_t<decltype(myInvoke_Calculate_Operation(op,std::declval<F>(),std::declval<x_is&>()()(pos)...))>;
  auto res= Result<vector_field<myvec,typename Operation::myResult_type>>{};
  return make_Operation_for(Operation{},Id{},res,Arguments<Xs...>{});

}


template<class Id,class Calc, class Fi, class... Xs, class...Datas>
auto Calculate_Operation_on_this(Calculator_new,const Fq_new<Id,Calc,Fi,Arguments<Xs...>>& ,const Datas&...ds)
{
  auto res=(find_type_in_new(Fi{},ds...)&&...&&find_type_in_new(Xs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto f=get_type_from_new(Fi{},ds...);
    //  using test=typename Id::Id;
    //using test2=typename decltype (apply_calculate_op_result(Calc{},f,Id{},Arguments<Xs...>{},Arguments(get_type_from_new(Xs{},ds...)...)))::decltyepe;
    return apply_calculate_op_result(Calc{},f,Id{},Arguments<Xs...>{},Arguments(get_type_from_new(Xs{},ds...)...));
  }

}



template<class Calculator_new,class Id,class G, class... Xs, class...Datas>
auto Calculate_Operation_on_this(Calculator_new,F_new<Id,G,Arguments<Xs...>>, Datas&&...ds)
{
  auto res=(find_type_in_new(Xs{},std::forward<Datas>(ds)...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto Res=apply_calculate_result(Calculator_new{},Id{},Result<G>{},Arguments(get_type_from_new(Xs{},std::forward<Datas>(ds)...)...));
    //  using test=typename std::conditional_t<is_this_template_class_v<sub,Id>,decltype (make_Operation(Calculator_new{},Id{},Res, Arguments<Xs...>{})), std::vector<double>>::value_type;
    return make_Operation_for(Calculator_new{},Id{},Res, Arguments<Xs...>{});
  }

}


template<class Id,class Distribution, class... Xs,class...Datas>
auto Calculate_Operation_on_this(logProbability,const D<Id,Distribution,Arguments<Xs...>>& ,const Datas&...ds)
{
  auto res=(find_type_in_new(Id{},ds...)&&...&&find_type_in_new(Xs{},ds...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    //    using test=typename Id::Id;

    auto Res=apply_calculate_result_sum(logProbability{},Id{},Result<Distribution>{},Arguments(get_type_from_new(Id{},ds...),get_type_from_new(Xs{},ds...)...));
    //  using test2=typename decltype (Res)::Res;
    //  using test3=typename decltype (make_Operation(logProbability{},Id{},Res, Arguments<Xs...>{}))::make_Operation;
    return make_Operation_for(logProbability{},Id{},Res, Arguments<Xs...>{});
  }

}



template < class Id, class... Fs,class Random, class... Datas>
auto sample_Operation_Id(const quimulun<Fs...>& qui, Random& mt,Id, Datas&&...d)
{
  return Sample_Operation_on_this(Sample{},qui[Id{}],mt,qui,std::forward<Datas>(d)...);
}

template < class Calculator_new,class Id, class... Fs, class... Datas>
auto calculate_Operation_Id(Calculator_new,const quimulun<Fs...>& qui, Id, Datas&&...d)
{
  return Calculate_Operation_on_this(Calculator_new{},qui[Id{}],qui,std::forward<Datas>(d)...);
}

template <class...Fs,class...ds, class x_random,class... Par_ids, class...Datas >
auto sample_Operation_on(const quimulun<Fs...>& qui, x_random& mt,myselect<Par_ids...>,Datas&&...d)
{

  return (Operation_non_serial<>{}+...+sample_Operation_Id(qui,mt,Par_ids{},std::forward<Datas>(d)...));
}



template<class ...Ops, class... Ts>
constexpr auto operator+(Operation_non_serial<Ops...>&&,
                         Operation_Coord<Ts...>&&)
{
  return    Operation_non_serial<Ops...,Operation_Coord<Ts...>>{};
}




template<class ...x_is, class... Xs, class... X2s,class... Ops,class... Fids, template<class, class>class x_i,class e_i,class value_type, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<(same_pack_set(Cs<Xs...>{},Cs<X2s...>{})&&!is_this_template_class_v<logpr,e_i>)>>
constexpr auto operator+(Operation_non_serial<
                             Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>,
                             Operations...>&&,
                         Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i<e_i,value_type>>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i>>())::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,typename x_is::myId...>>,Arguments<Args...>>;

  return    Operation_non_serial<
      Operation_for<
          Result<
              vector_field<vec<Xs...>,decltype(std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i<e_i,value_type>>>())>
              >,
          Operation_vector<Operation<Ops,Fids>...,Operation<Op,Fid>>,ArgsOut >,
      Operations...>{};

}






template<class ...Ups, class ...Downs, class Value_one,
          class ...Ups2, class ...Downs2, class Value_two, class... Xs, class... X2s,class... Ops,class... Fids, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(Operation_non_serial<
                             Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>,
                             Operations...>&&,
                         Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,logv<double,unit_one>>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,logv<double,unit_two>>{})::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,logpr<up<Ups...>,dn<Downs...>>>>,Arguments<Args...>>;

  return    Operation_non_serial<
      Operation_for<
          Result<
              vector_field<vec<Xs...>,vector_space<decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>{})>
                           >>,
          Operation_vector<Operation<Ops,Fids>...,Operation<Op,Fid>>,ArgsOut >,
      Operations...>{};

}


template<class ...x_is, class... Xs, class... Ops,class... Fids, class x_i, class ...X2s,class Op, class Fid,class... Operations,class... Args, class... Args2>
auto operator+(Operation_non_serial<
                   Operation_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>,
                   Operations...>&&,
               Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{
  return    Operation_non_serial<
             Operation_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>>{}&&
         (Operation_non_serial<Operations...>{}+
          Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>{});

}




template<class ...x_is, class... Xs, class... Ops,class... Fids, class x_i, class ...X2s,class Op, class Fid,class... Operations,class... Args, class... Args2,
          typename=std::enable_if_t<!same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
auto operator+(Operation_non_serial<
                   Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>,
                   Operations...>&&,
               Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{
  return    Operation_non_serial<
             Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>>{}&&
         (Operation_non_serial<Operations...>{}+
          Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>{});

}



template<class Value_type,class Value_type2,class... Ops,class... Fids,  class Op, class Fid,class... Args, class...Args2>
constexpr auto operator+(    Operation_sum<Result<Value_type>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>&&,
                         Operation_sum<Result<Value_type2>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i>>())::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...>>,Arguments<Args...>>;

  return
      Operation_sum<
          Result<
              decltype(std::declval<Value_type>()+std::declval<Value_type2>()) >,
          Operation_vector<Operation<Ops,Fids>...,Operation<Op,Fid>>,ArgsOut >{};

}




template< class... Ops>
auto operator+(Operation_non_serial<>&&,
               Operation_for<Ops...>
                   &&)
{
  return    Operation_non_serial<Operation_for<Ops...>
                              >{};
}

template<class... Ts, class... Es>
auto operator+(Operation_non_serial<Error<Es...>>&&,
               Operation_for<Ts...>&&)
{
  return    Operation_non_serial<Operation_for<Ts...>>{};
}

template<class... Ts, class... Es>
auto operator+(Operation_non_serial<Error<Ts...>>&&,
               Error<Es...>&&)
{
  return    Operation_non_serial<Error<Ts...,Es...>>{};
}


template<class... Ts, class... Es>
auto operator+(Operation_non_serial<Ts...>&&,
               Error<Es...>&&)
{
  return    Operation_non_serial<Ts...>{};
}


template<class... Ts>
auto operator+(Operation_non_serial<>&&,
               Error<Ts...>&&)
{
  return    Operation_non_serial<Error<Ts...>>{};
}




template<class... Ops1, class... Ops2>
auto operator&&(Operation_non_serial<Ops1...>&&,
                Operation_non_serial<Ops2...>&&)
{
  return    Operation_non_serial<Ops1..., Ops2...>{};
}


template<class... Ops1, class... Ops2>
auto operator+(Operation_non_serial<Ops1...>&&,
               Operation_non_serial<Ops2...>&&)
{
  return    (Operation_non_serial<Ops1...>{}+...+ Ops2{});
}








template <class Op,class...Fs,class...ds,class... Par_ids, class...Datas >
auto calculate_Operation_on(Op,const quimulun<Fs...>& qui, myselect<Par_ids...>,Datas&&...d)
{

  return (Operation_non_serial<>{}+...+calculate_Operation_Id(Op{},qui,Par_ids{},std::forward<Datas>(d)...));
}








template<class V,class...T>
auto clean_Error(Operation_non_serial<Error<T...>>&&){return V{};}



template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class... parOp_non_serials,class... varOp_non_serials>
auto sample_parameters_calculate_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                           myselect<Par_ids...>,myselect<Var_ids...>,
                                           Operation_serial<parOp_non_serials...>&& parameters,Operation_serial<varOp_non_serials...>&& variables,Datas&&... d)
{
  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},parameters,variables,std::forward<Datas>(d)...);




  //using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,myIds_t<decltype(new_parameters)>>,myselect<>>;
  auto  new_par_idv=remove_succesfull_Operations(myselect<Par_ids...>{},new_parameters);
  if constexpr (new_par_idv.size==0)
    return std::move(parameters)+clean_Error<Operation_non_serial<>>(std::move(new_parameters));
  else
  {
    auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},parameters,variables,std::forward<Datas>(d)...);
    if constexpr (new_parameters.size()+new_variables.size()==0)
      return check_if_reacheable(new_parameters);
    //      return check_if_reacheable(Cs<Par_ids...>{});
    else
    {
      //   using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t<decltype(new_variables)>>,myselect<>>;
      auto  new_var_idv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);

      return sample_parameters_calculate_Operation(qui,mt,new_par_idv,new_var_idv,
                                                   std::move(parameters)+clean_Error<Operation_non_serial<>>(std::move(new_parameters)),
                                                   std::move(variables)+clean_Error<Operation_non_serial<>>(std::move(new_variables)),std::forward<Datas>(d)...);

    }
  }
}


template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class... Op_non_serials>
auto sample_parameters_calculate_variables_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                                     myselect<Par_ids...>,myselect<Var_ids...>,
                                                     Operation_serial<Op_non_serials...>&& operations,Datas&&... d)
{
  //using test=typename myselect<Par_ids...>::Par_id;
  //using test2=typename myselect<Var_ids...>::Var_id;


  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},operations,std::forward<Datas>(d)...);
  auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},operations,std::forward<Datas>(d)...);



  if constexpr (new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)==0)
      return std::move(operations);
    else
      return check_if_reacheable(new_parameters+new_variables);
  }
  else
  {

    //    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,myIds_t<decltype (new_parameters)>>,myselect<>>;
    auto  new_par_idv=remove_succesfull_Operations(myselect<Par_ids...>{},new_parameters);
    auto  new_var_idv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);

    //  using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t< decltype (new_variables)>>,myselect<>>;

    return sample_parameters_calculate_variables_Operation(qui,std::forward<x_random>(mt),new_par_idv,new_var_idv,
                                                           operations+(clean_Error<Operation_non_serial<>>(std::move(new_parameters))+
                                                                         clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}


template <class Op, class...Fs,class...Datas,  class...Var_ids,class... Op_non_serials>
auto calculate_variables_Operation(Op,const quimulun<Fs...>& qui,  myselect<Var_ids...>,
                                   Operation_serial<Op_non_serials...>&& operations,Datas&&... d)
{

  //using test=typename myselect<Var_ids...>::var_ids;
  auto new_variables=calculate_Operation_on(Op{},qui,myselect<Var_ids...>{},operations,std::forward<Datas>(d)...);

  //using test=typename decltype (new_variables)::nerw;
  //using test2=typename std::index_sequence<new_variables.size()>::nerw;


  if constexpr (new_variables.size()==0)
  {
    if constexpr(sizeof... (Var_ids)==0)
      return std::move(operations);
    else
      return check_if_reacheable(new_variables);
    //     return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>>{});
  }
  else
  {

    //    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t<decltype (new_variables)>>,myselect<>>;
    auto  new_var_idv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);

    return calculate_variables_Operation(Op{},qui,new_var_idv,
                                         operations+(clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}



template <class...Fs,class...Datas, class x_random>
auto sampleParameters_Operation(const quimulun<Fs...>& qui, x_random& mt, Datas&&... d )
{
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_new_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids, pack_concatenation_t<myIds_t<Datas>...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;


  return sample_parameters_calculate_Operation(qui,mt,Parameters_ids{},Variables_ids{},Operation_serial<>{},Operation_serial<>{},std::forward<Datas>(d)...);


}





template <class...Fs,class...Datas, class x_random>
auto sample_Operation(const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;
  using variables_ids=extract_function_Id_new_t<Cs<Fs...>>;
  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,pack_concatenation_t<myIds_t<Datas>...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return sample_parameters_calculate_variables_Operation(qui,mt,Parameters_ids{},Variables_ids{}, Operation_serial<>{},std::forward<Datas>(d)...);


}
template <class...Fs,class... Ids,class...Datas>
auto logP_Operation_on(const quimulun<Fs...>& qui, myselect<Ids...>, Datas&&... d)
{
  auto s= (...+calculate_Operation_Id(logProbability{},qui,Ids{},std::forward<Datas>(d)...));
  // auto s=(Calculate_Operation_on_this(logProbability{},qui[Ids{}],qui,std::forward<Datas>(d)...)+...);


  //  using  new_var_ids=transfer_t<pack_difference_t<Cs<Ids...>,myIds_t< decltype (s)>>,myselect<>>;
  //  using test=typename new_var_ids::test;
  //    if constexpr (new_var_ids{}.size > 0)

  //      return check_if_reacheable(s);
  //    else
  return s;


}




template <class...Fs,class...ds, class...ps, class... vs>
auto logPrior_Operation(const quimulun<Fs...>& qui, const vector_space<ds...>& data, const vector_space<ps...>& parameters, const vector_space<vs...>& variables )
{
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;
  using parameters_ids=pack_difference_t<distribution_ids,Cs<typename ds::myId...>>;

  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return logP_Operation_on(qui,Parameters_ids{},data,parameters,variables);
}


template <class...Fs,class ds, class ...Datas>
auto calculate_Operation(logPrior_new,const quimulun<Fs...>& qui, const ds& data, Datas&&...d )
{
  //using test=typename ds::test;
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;
  using parameters_ids=pack_difference_t<distribution_ids,typename ds::myIds>;

  using Parameters_ids=transfer_t<parameters_ids,myselect<>>;
  return logP_Operation_on(qui,Parameters_ids{},data,std::forward<Datas>(d)...);
}


template <class...Fs,class ds, class ...Datas>
auto calculate_Operation(logLikelihood_new,const quimulun<Fs...>& qui, const ds& data, Datas&&...d )
{

  //using test=typename ds::test;
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;
  using likelihood_ids=pack_intersection_t<distribution_ids,typename ds::myIds>;

  using Likelihood_ids=transfer_t<likelihood_ids,myselect<>>;
  return logP_Operation_on(qui,Likelihood_ids{},data,std::forward<Datas>(d)...);
}

template <class Op, class...Fs,class...Datas>
auto calculate_Operation(Op,const quimulun<Fs...>& qui,  Datas&&... d)
{
  //using test=typename quimulun<Fs...>::quimul;
  using variables_ids=extract_function_Id_new_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return calculate_variables_Operation(Op{},qui,Variables_ids{}, Operation_serial<>{},std::forward<Datas>(d)...);
}


template <template<class, class>class f_i,class e_i, class Value_type,class...Datas, class x_random>
auto myInvoke_Sample_Operation(Sample,const f_i<e_i,Value_type>& f, x_random& mt, Datas&&... d)
{
  return sample_Operation(f(),mt,std::forward<Datas>(d)...);
}

template <template<class, class>class f_i,class e_i, class Value_type,class...Datas, class x_random>
auto myInvoke_Sample_Operation(SampleParameters,const f_i<e_i,Value_type>& f, x_random& mt, Datas&&... d)
{
  //  using test=typename e_i::ei;
  //  using test2=typename Cs<Datas...>::Data;
  //  using  test3=typename Cs<e_i,decltype (sampleParameters_Operation(f(),mt,std::forward<Datas>(d)...))>::SampleParam;
  return sampleParameters_Operation(f(),mt,std::forward<Datas>(d)...);
}

template<class... target_Ids, class OperationSet>
auto remove_succesfull_Operations(myselect<target_Ids...>,  OperationSet)
{

  using   Operation_Ids=myIds_t<OperationSet>;

  return transfer_t<pack_concatenation_t<Cs<>,
                                         std::conditional_t<(is_in_pack(Target_Id_t<target_Ids>{},Operation_Ids{})),Cs<>,
                                                            Cs<target_Ids>>...>, myselect<>>{};

}






template <class Op, template<class, class>class f_i,class e_i, class Value_type,class...Datas>
auto myInvoke_Calculate_Operation(Op,const f_i<e_i,Value_type>& f, Datas&&... d)
{
  //using test=typename Op::Op;
  return calculate_Operation(Op{},f(),std::forward<Datas>(d)...);
}



template <class Distribution,class...Datas, class x_random>
auto myInvoke_Operation_Sample(Sample s,Distribution di, x_random& mt, Datas&&... d)
{
  using res_value_type=decltype (myInvoke(s,di,mt,std::forward<Datas>(d)...));
  return Result<res_value_type>{};
}


template<class, class, class, class> struct Operation_loop;


template<class... init_ops, class... start_ops, class... loop_ops,class... next_ops >
struct Operation_loop<Operation_serial<init_ops...>, Operation_serial<start_ops...>, Operation_serial<loop_ops...>, Operation_serial<next_ops...>>
{
  Operation_loop(Operation_serial<init_ops...>, Operation_serial<start_ops...>, Operation_serial<loop_ops...>, Operation_serial<next_ops...>){}
};



template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class...start_par_ids, class... next_par_ids, class...start_var_ids, class... next_var_ids,  class... Op_non_serials>
auto sample_parameters_calculate_variables_loop_init_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                                               myselect<Par_ids...>,myselect<Var_ids...>,
                                                               Operation_serial<Op_non_serials...>&& operations,Datas&&... d)
{
  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},operations,std::forward<Datas>(d)...);
  auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},operations,std::forward<Datas>(d)...);
  if constexpr (new_parameters.size()+new_variables.size()==0)
  {
    return std::move(operations);
  }
  else
  {

    //    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,myIds_t<decltype (new_parameters)>>,myselect<>>;
    //    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t< decltype (new_variables)>>,myselect<>>;

    auto  new_par_idv=remove_succesfull_Operations(myselect<Par_ids...>{},new_parameters);
    auto  new_var_idv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);


    return sample_parameters_calculate_variables_loop_init_Operation(qui,std::forward<x_random>(mt),new_par_idv,new_var_idv,
                                                                     operations+(clean_Error<Operation_non_serial<>>(std::move(new_parameters))+
                                                                                   clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}


template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class...start_par_ids,class...start_par_nsteps,
          class...start_var_ids,class...start_var_nsteps,class...start_Ops>
auto sample_parameters_calculate_variables_loop_start_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                                                myselect<Par_ids...>,myselect<Var_ids...>,
                                                                myselect<Start_new<start_par_ids,start_par_nsteps>...>,
                                                                myselect<Start_new<start_var_ids,start_var_nsteps>...>,
                                                                Operation_serial<start_Ops...>&& start_ops,Datas&&... d)
{

  auto new_start_parameters=sample_Operation_on(qui,mt,myselect<Start_new<start_par_ids,start_par_nsteps>...>{} ,start_ops,std::forward<Datas>(d)...);
  auto new_start_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Start_new<start_var_ids,start_var_nsteps>...>{},start_ops,std::forward<Datas>(d)...);

  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},start_ops,std::forward<Datas>(d)...);
  auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},start_ops,std::forward<Datas>(d)...);
  if constexpr (new_start_parameters.size()+new_start_variables.size()+new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)+sizeof... (start_par_ids)+sizeof... (start_var_ids)==0)
      return std::move(start_ops);
    else
      return check_if_reacheable(new_parameters+new_variables+new_start_parameters+new_start_variables);
  }
  else
  {
    auto new_start_par_idv= remove_succesfull_Operations(myselect<start_par_ids...>{},new_start_parameters);

    auto new_start_var_idv= remove_succesfull_Operations(myselect<start_var_ids...>{},new_start_variables);

    //    using new_start_variables_myIds=myIds_t<decltype (new_start_variables)>;
    //    using  new_start_var_ids=transfer_t<pack_concatenation_t<Cs<>,
    //                                             std::conditional_t<(is_in_pack(start_var_ids{},new_start_variables_myIds{})),Cs<>,
    //                                                                                 Cs<Start_new<start_var_ids,start_var_nsteps>>>...>, myselect<>>;



    //    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,myIds_t<decltype (new_parameters)>>,myselect<>>;
    auto  new_par_idsv=remove_succesfull_Operations(myselect<Par_ids...>{},new_parameters);
    //    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t< decltype (new_variables)>>,myselect<>>;
    auto  new_var_idsv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);

    //using test=typename Cs<start_par_ids...>::ids;
    //using test2=typename new_start_var_ids::ids;


    return sample_parameters_calculate_variables_loop_start_Operation(qui,std::forward<x_random>(mt),new_par_idsv,new_var_idsv,new_start_par_idv,
                                                                      new_start_var_idv,
                                                                      start_ops+(clean_Error<Operation_non_serial<>>(std::move(new_start_parameters))+
                                                                                   clean_Error<Operation_non_serial<>>(std::move(new_start_variables))+
                                                                                   clean_Error<Operation_non_serial<>>(std::move(new_parameters))+
                                                                                   clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}



template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class...next_par_ids,class...next_par_nsteps,
          class...next_var_ids,class...next_var_nsteps,class...step_Ops>
auto sample_parameters_calculate_variables_loop_next_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                                               myselect<Par_ids...>,myselect<Var_ids...>,
                                                               myselect<Next_new<next_par_ids,next_par_nsteps>...>,
                                                               myselect<Next_new<next_var_ids,next_var_nsteps>...>,
                                                               Operation_serial<step_Ops...>&& step_ops,Datas&&... d)
{

  auto new_next_parameters=sample_Operation_on(qui,mt,myselect<Next_new<next_par_ids,next_par_nsteps>...>{} ,step_ops,std::forward<Datas>(d)...);
  auto new_next_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Next_new<next_var_ids,next_var_nsteps>...>{},step_ops,std::forward<Datas>(d)...);

  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},step_ops,std::forward<Datas>(d)...);
  auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},step_ops,std::forward<Datas>(d)...);
  if constexpr (new_next_parameters.size()+new_next_variables.size()+new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)+sizeof... (next_par_ids)+sizeof... (next_var_ids)==0)
      return std::move(step_ops);
    else
      return check_if_reacheable(new_parameters+new_variables+new_next_parameters+new_next_variables);
  }
  else
  {
    auto new_next_par_idv= remove_succesfull_Operations(myselect<next_par_ids...>{},new_next_parameters);

    auto new_next_var_idv= remove_succesfull_Operations(myselect<next_var_ids...>{},new_next_variables);

    //    using new_next_variables_myIds=myIds_t<decltype (new_next_variables)>;
    //    using  new_next_var_ids=transfer_t<pack_concatenation_t<Cs<>,
    //                                             std::conditional_t<(is_in_pack(next_var_ids{},new_next_variables_myIds{})),Cs<>,
    //                                                                                 Cs<Next_new<next_var_ids,next_var_nsteps>>>...>, myselect<>>;



    //    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,myIds_t<decltype (new_parameters)>>,myselect<>>;
    auto  new_par_idsv=remove_succesfull_Operations(myselect<Par_ids...>{},new_parameters);
    //    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,myIds_t< decltype (new_variables)>>,myselect<>>;
    auto  new_var_idsv=remove_succesfull_Operations(myselect<Var_ids...>{},new_variables);

    //using test=typename Cs<next_par_ids...>::ids;
    //using test2=typename new_next_var_ids::ids;


    return sample_parameters_calculate_variables_loop_next_Operation(qui,std::forward<x_random>(mt),new_par_idsv,new_var_idsv,new_next_par_idv,
                                                                     new_next_var_idv,
                                                                     step_ops+(clean_Error<Operation_non_serial<>>(std::move(new_next_parameters))+
                                                                                 clean_Error<Operation_non_serial<>>(std::move(new_next_variables))+
                                                                                 clean_Error<Operation_non_serial<>>(std::move(new_parameters))+
                                                                                 clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}









#endif // QM_TENSOR_MODEL_OPERATIONS_NEW_H