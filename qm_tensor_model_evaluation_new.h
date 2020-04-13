#ifndef QM_TENSOR_MODEL_EVALUATION_NEW_H
#define QM_TENSOR_MODEL_EVALUATION_NEW_H
#include "qm_tensor_model_declaration.h"
#include "qm_vector_field_evaluation.h"
#include "qm_vector_tuple_declaration.h"
#include "qm_tensor_distribution_evaluation.h"
template<class ...> struct Operation_for;
template<class ...> struct Operation_serial;
template<class ...> struct Operation_non_serial;


//template <class anId, class X
//          , typename =std::enable_if_t< (only_xi_of_fi_v<X>)&&(!is_this_template_class_v<non_const , anId>)  ,int> >
//auto get_xi_from_this_new(anId, X&& xi)->std::conditional_t<std::is_same_v<anId,typename std::decay_t<X>::myId >, decltype (std::forward<X>(xi)),Nothing>
//{
//  if constexpr (std::is_same_v<anId,typename std::decay_t<X>::myId >)
//    return std::forward<X>(xi);
//  else
//    return Nothing{};
//}

//template <class anId, class X
//          , typename =std::enable_if_t< (only_xi_of_fi_v<X>) > >
//auto get_xi_from_this_new(non_const<anId>, X&& xi)->
//    std::conditional_t<std::is_same_v<anId,typename std::decay_t<X>::myId >, decltype (std::forward<X>(xi)),Nothing>
//{
//  if constexpr (std::is_same_v<anId,typename std::decay_t<X>::myId >)
//    return std::forward<X>(xi);
//  else
//    return Nothing{};
//}



template <class anId, class V
          , typename =std::enable_if_t< !(only_xi_of_fi_v<V>) > >
auto get_xi_from_this_new(anId, V&& xi)->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
{
  return only_xi_or_fi(std::forward<V>(xi)[anId{}]);
}


template <class anId, class...Ops>
auto get_type_xi_from_this_new(anId, const Operation_non_serial<Ops...>& xi)//->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
{
  return xi[anId{}];
}

template <class anId, class...Ops>
auto get_type_xi_from_this_new(anId,const Operation_serial<Ops...>& xi)//->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
{
  // using testOperation=typename anId::testOperation;
  // using test2=typename decltype(xi[anId{}])::test;

  return xi[anId{}];
}



template <class anId, class V
          , typename =std::enable_if_t<
              (!is_this_template_class_v<Operation_serial,V>) &&
              !(only_xi_of_fi_v<V>) > >
auto get_type_xi_from_this_new(anId, V&& xi)//->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
{
  // using test=typename anId::test;
  // using test2=typename decltype(xi[anId{}])::test;
  return Result_t<decltype(only_xi_or_fi(std::forward<V>(xi)[anId{}]))>{};
}

//template <class anId, class... Xs,class... x_is>
//auto get_type_xi_from_this_new(anId, const vector_field<vec<Xs...>,vector_space<x_is...>>& )//->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
//{
//  return toResult(vec<Xs...>{},Result<std::decay_t<decltype(std::declval<const vector_space<x_is...>&>()[anId{}])>>{});
//}





template <class anId, class...Datas
          , typename =std::enable_if_t< ( !is_this_template_class<all,anId>::value)&&
                                          (!is_this_template_class<pass_id,anId>::value)&&
                                          (!is_this_template_class<Size_at_Index_new,anId>::value)&&
                                          (!is_this_template_class<non_const,anId>::value)&&

                                          ( !is_this_template_class<std::tuple,anId>::value) ,int>>
auto get_from_new(anId, Datas&&...ds)->decltype ((get_xi_from_this_new(anId{},std::forward<Datas>(ds))||...))
{
  //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
  return (get_xi_from_this_new(anId{},std::forward<Datas>(ds))||...);
}

template <class anId, class...Datas
          , typename =std::enable_if_t< ( !is_this_template_class<all,anId>::value)&&
                                          (!is_this_template_class<Size_at_Index_new,anId>::value)&&
                                          (!is_this_template_class<pass_id,anId>::value)&&
                                          (!is_this_template_class<non_const,anId>::value)&&

                                          ( !is_this_template_class<std::tuple,anId>::value) ,int>>
auto get_type_from_new(anId, Datas&&...ds)//->decltype ((get_type_xi_from_this_new(anId{},std::forward<Datas>(ds))||...))
{
  //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
  return (get_type_xi_from_this_new(anId{},std::forward<Datas>(ds))||...);
}

template <class Id, class Index, class...Datas>
auto get_from_new(Size_at_Index_new<Id, Index>, Datas&&...ds)
{

  auto x=(get_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);
  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return vector_field_size(x(),Index{});
}
template <class Id, class Index, class...Datas>
auto get_type_from_new(Size_at_Index_new<Id, Index>, Datas&&...ds)
{
  auto x=(get_type_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);
  using field_type=decltype (std::declval<typename decltype (x)::result_type>()());
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return Result<vector_field_size<field_type,Index>>{};
}



template <class anId, class...Datas, typename=std::enable_if_t<!is_this_template_class_v<pass_id,anId>>>
auto find_type_in_new(anId, Datas&&...ds)
{
  if constexpr (std::is_same_v<decltype (get_type_from_new(anId{},std::forward<Datas>(ds)...)),Nothing >)
    return Not_found<anId>{};
  else
    return Has_been_found{};
}






template<class T>
struct getId_of {using type=typename T::myId;};




template<class T>
struct getIds_of {using type=typename T::myIds;};

template< class ...x_is, class... Xs>
struct getIds_of<vector_field<vec<Xs...>,vector_space<x_is...>>>
{
  using type=typename vector_space<x_is...>::myIds;
};

template<class T>
using myIds_t=typename getIds_of<std::decay_t<T>>::type;


template<class T>
using myId_t=typename getId_of<std::decay_t<T>>::type;











template<class...Datas>
auto myInvoke(Glue_new,Datas&&...d)
{
  //using test=typename decltype (vector_space{make_view(std::forward<Datas>(d))...})::test;
  return vector_space{make_view(std::forward<Datas>(d))...};
}
template< class...Datas>
auto myInvoke(IndexCoordinate,Datas&&...d)
{
  return std::invoke(IndexCoordinate{},std::forward<Datas>(d)...);
}


template<class F,  class...Datas>
auto myInvoke(Calculator_new,F&& f,Datas&&...d)
{
  return myInvoke(std::forward<F>(f), std::forward<Datas>(d)...);
}


template<class...>struct Operation_vector{};




template<class Operator, class Id> struct Operation{};


template<class R,class Id,class G, class... Xs, class... Datas>
void myInvoke_elem_new(R& r,Calculator_new,const F_new<Id,G,Arguments<Xs...>>& f,Datas&&...d)
{
  r=std::invoke(f.f(), std::forward<Datas>(d)...);
}



template<class R,class Op,class Id,class G, class... Xs, class Position,class ...x_i,class... Datas>
auto myInvoke_pos_new(R& r,Op,const F_new<Id,G,Arguments<Xs...>>& f,const Position& p, const vector_space<x_i...>& x,Datas&&...d)
{
  myInvoke_elem_new(r(p),Op{},f.f(), get_type_from_new(Xs{},x,d...)(p)...);
}


template<class ...x_is, class Op, class... Ops,class Fid,class... Fids,class Position,class... Fs,class... Datas>
void myRecursiveInvoke_pos_new(vector_space<x_is...>& x,Operation_vector<Operation<Op,Fid>,Operation<Ops,Fids>...>,const quimulun<Fs...>& qui,const Position& p, Datas&&...d)
{
  myInvoke_pos_new(x[non_const<Fid>{}],Op{},qui[Fid{}], p,x,std::forward<Datas>(d)...);
  myRecursiveInvoke_pos_new(x,Operation_vector<Operation<Ops,Fids>...>{},qui,p,std::forward<Datas>(d)...);
}


template<class ...Operations>
struct Operation_non_serial
{

  using myResult_type=vector_tuple<typename Operations::myResult_type... >;

  static constexpr auto size() { return (0+...+Operations::size());}

  using myIds=pack_concatenation_t<Cs<>,typename Operations::myIds...>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    return (Nothing{}||...||Operations{}[Id{}]);
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    return (Operations{}(qui,std::forward<Datas>(d)...)+...);
  }

};

template<class Operation,class ...Operations>
struct Operation_serial<Operation,Operations...>
{

  using myResult_type=vector_tuple<typename Operation::myResult_type,typename Operations::myResult_type... >;

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


template<class Op,class Id, class... Operations, class... Args>
constexpr auto make_Operation(Op,Id,Operation_serial<Operations...>,Arguments<Args...>)
{

  //  using test=typename Id::test;
  //  using test2=typename  Operation_serial<Operations...>::myResult_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,typename Operation_serial<Operations...>::myResult_type> > >>,Operation_vector< Operation<Operation_serial<Operations...>,Id> > , Arguments<Args...>>{};
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



template< class ...x_is, class... Xs, class... Ops,class... Fids, class ...Args>
struct Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>

{


  template<class Id>
  constexpr auto operator[](Id)const
  {
    //     using test=typename Id::test;
    //     using test2=typename decltype (toResult(vec<Xs...>{},Result<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{}))::test;
    return toResult(vec<Xs...>{},Result<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[Id{}])>>{});
  }

  constexpr static auto size(){ return sizeof... (x_is);}

  using myIds=Cs<typename x_is::myId...>;


  using myResult_type=vector_field<vec<Xs...>,vector_space<x_is...>>;
  using myValue_type= vector_space<x_is...>;
  using myvec=vec<Xs...>;

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    if constexpr (sizeof... (Xs)>0)
    {
      myValue_type outf;
      auto p=myResult_type::begin();

      fill_vector_field(outf,p,myvec{},get_from(Xs{},d...)...);

      myResult_type out(std::move(outf));
      p=out.begin();
      std::vector<std::decay_t<decltype (p)>> allpos;
      do
      {
        allpos.push_back(p);
      } while(out.next(p));

#pragma omp parallel for
      for (std::size_t i=0; i<allpos.size(); ++i)
      {
        myRecursiveInvoke_pos_new(out(allpos[i]),Operation_vector<Operation<Ops,Fids>...>{},qui,allpos[i],std::forward<Datas>(d)...);

      }
      return out;
    }
    else
    {
      myValue_type out;
      auto p=Position<>{};
      myRecursiveInvoke_pos_new(out(p),Operation_vector<Operation<Ops,Fids>...>{},qui,p,std::forward<Datas>(d)...);
      return out;
    }



  }


};







template<class Op,class Id, class value_type, class... Xs, class... Args>
constexpr auto make_Operation(Op,Id,Result<vector_field<vec<Xs...>,value_type>>,Arguments<Args...>)
{
  // using test=typename Id::test;
  // using test2=typename value_type::test2;

  return Operation_for< Result< vector_field<vec<Xs...>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
};

template<class Op,class Id, class value_type, class... Args>
constexpr auto make_Operation(Op,Id,Result<value_type>,Arguments<Args...>)
{
  // using test=typename Id::test;
  // using test2=typename value_type::test2;
  return Operation_for< Result< vector_field<vec<>,vector_space< x_i<Id,value_type> > >>,Operation_vector< Operation<Op,Id> > , Arguments<Args...>>{};
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





template<class Op,class F, class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >>>
auto apply_calculate_result(Op op,Result<F>,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  auto pos=transfer_t<myvec, Position<>>{};
  return Result<vector_field<myvec,std::decay_t<decltype(myInvoke(op,std::declval<F>(),std::declval<x_is>()()(pos)...))>>>{};

}


template<class Op,class F, class Id,class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >>>
auto apply_calculate_coordinate_result(Op op,F&& f,Id, Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})<<vec<Id>{}) myvec;
  auto pos=transfer_t<myvec, Position<>>{};
  return Result<vector_field<myvec,std::decay_t<decltype(myInvoke(op,f,std::declval<x_is>()()(pos)...))>>>{};

}



template<class Op, class... x_is>
auto apply_calculate_result(Op op,Result<Glue_new>,Arguments<Result<x_is>...>)
{
  using res_value_type=std::decay_t<decltype(myInvoke(op,Glue_new{},std::declval<x_is const &>()...))>;
  return Result<res_value_type>{};
}

template<class Op,class F, class Random, class... x_is>
auto apply_sample_result(Op op,Result<F>,Random& mt,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  auto pos=transfer_t<myvec, Position<>>{};

  return Result<vector_field<myvec,typename std::decay_t<decltype( myInvoke_Operation(op,std::declval<F>(),mt,std::declval<x_is>()()(pos)...))>::result_type>>{};
}


template<class Op,class F,class Id, class Random, class ...Xs,class... XXs,class... x_is, class ...xx_is>
auto apply_sample_op_result(Op op,Result<F>,Id, Random& mt,Arguments<Xs...>,Index_struct<XXs...>,Arguments<Result<x_is>...>,Index_struct<Result<xx_is>...> )
{
  typedef decltype (((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})<<...<<get_Field_Indexes_t<xx_is>{})) myvec;
  auto pos=transfer_t<myvec, Position<>>{};

  using Operation=std::decay_t<decltype( myInvoke_Operation(op,std::declval<F>(),mt,std::declval<x_is>()()(pos)...))>;

  auto Res= Result<vector_field<myvec,typename Operation::myResult_type>>{};
  return make_Operation(Operation{},Id{},Res,Arguments<Xs...>{});

}

template<class Op,class F,class Id, class Random, class ...Xs,class... x_is>
auto apply_sample_op_result(Op op,Result<F>,Id, Random& mt,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  auto pos=transfer_t<myvec, Position<>>{};

  using Operation=std::decay_t<decltype( myInvoke_Operation(op,std::declval<F>(),mt,std::declval<x_is>()()(pos)...))>;

  auto Res= Result<vector_field<myvec,typename Operation::myResult_type>>{};
  return make_Operation(Operation{},Id{},Res,Arguments<Xs...>{});

}













template<class... Fs,class Id,class Distribution, class... Xs,class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const D<Id,Distribution,Arguments<Xs...>>& ,Rnd & mt,const Datas&...ds)
{
  auto res=(find_type_in_new(Xs{},ds...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=Result<Distribution>{};
    auto Res=apply_sample_result(Sample{},d,mt,Arguments(get_type_from_new(Xs{},ds...)...));
    return make_Operation(Sample{},Id{},Res,Arguments<Xs...>{});
  }

}

template<class Id,class Sampler,class Fi, class... Xs,
          class... Fs,class Rnd, class...Datas>
auto Sample_Operation_on_this(Sample,const Dq_new<Id,Sampler,Fi,Arguments<Xs...>>& ,Rnd & mt,const Datas&...ds)
{
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
auto Calculate_Operation_on_this(Calculator_new,const Coord_new<Id,G,Arguments<Xs...>>& ,const Datas&...ds)
{
  auto res=(find_type_in_new(Xs{},ds...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto d=G{};
    auto Res=apply_calculate_coordinate_result(Calculator_new{},d,Id{},Arguments(get_type_from_new(Xs{},ds...)...));
    return make_Operation(Calculator_new{},Id{},Res, Arguments<Xs...>{});
  }

}

template<class Op,class F, class Id,class ...Xs,class... x_is, typename=std::enable_if_t<!std::is_same_v<std::decay_t<F>,Glue_new >>>
auto apply_calculate_op_result(Op op,Result<F> ,Id,Arguments<Xs...>,Arguments<Result<x_is>...>)
{
  typedef decltype ((vec<>{}<<...<<get_Field_Indexes_t<x_is>{})) myvec;
  auto pos=transfer_t<myvec, Position<>>{};
  auto res= Result<vector_field<myvec,typename std::decay_t<decltype(myInvoke_Operation(op,std::declval<F>(),std::declval<x_is>()()(pos)...))>::myResult_type>>{};
  return make_Operation(Op{},Id{},res,Arguments<Xs...>{});

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
    return apply_calculate_op_result(Calc{},f,Id{},Arguments<Xs...>{},Arguments(get_type_from_new(Xs{},ds...)...));
  }

}



template<class Calculator_new,class Id,class G, class... Xs, class...Datas>
auto Calculate_Operation_on_this(Calculator_new,F_new<Id,G,Arguments<Xs...>>,const Datas&...ds)
{
  auto res=(find_type_in_new(Xs{},ds...)&&...);
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    auto e=Error(Arguments_not_found(Id{},res));
    return e;
  }else

  {
    auto Res=apply_calculate_result(Calculator_new{},Result<G>{},Arguments(get_type_from_new(Xs{},ds...)...));
    return make_Operation(Calculator_new{},Id{},Res, Arguments<Xs...>{});
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
    auto Res=apply_calculate_result(logProbability{},Result<Distribution>{},Arguments(get_type_from_new(Id{},ds...),get_type_from_new(Xs{},ds...)...));
    return make_Operation(Calculator_new{},Id{},Res, Arguments<Xs...>{});
  }

}



template < class... Fs,class Id, class Random, class... Datas>
auto sample_Operation_Id(const quimulun<Fs...>& qui, Random& mt,Id, Datas&&...d)
{
  return Sample_Operation_on_this(Sample{},qui[Id{}],mt,qui,std::forward<Datas>(d)...);
}

template < class Calculator_new,class... Fs,class Id,  class... Datas>
auto calculate_Operation_Id(Calculator_new,const quimulun<Fs...>& qui, Id, Datas&&...d)
{
  return Calculate_Operation_on_this(Calculator_new{},qui[Id{}],qui,std::forward<Datas>(d)...);
}

template <class...Fs,class...ds, class x_random,class... Par_ids, class...Datas >
auto sample_Operation_on(const quimulun<Fs...>& qui, x_random& mt,myselect<Par_ids...>,Datas&&...d)
{

  return (Operation_non_serial<>{}+...+sample_Operation_Id(qui,mt,Par_ids{},std::forward<Datas>(d)...));
}




template<class ...x_is, class... Xs, class... X2s,class... Ops,class... Fids, class x_i, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(Operation_non_serial<
                             Operation_for<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Operation<Ops,Fids>...>,Arguments<Args...>>,
                             Operations...>&&,
                         Operation_for<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Operation_vector<Operation<Op,Fid>>,Arguments<Args2...>>&&)
{



  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,typename x_is::myId...>>,Arguments<Args...>>;

  return    Operation_non_serial<
      Operation_for<
          Result<
              vector_field<vec<Xs...>,vector_space<x_is...,x_i>>
              >,
          Operation_vector<Operation<Ops,Fids>...,Operation<Op,Fid>>,ArgsOut >,
      Operations...>{};

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




template <class...Fs,class...Datas, class x_random,class... Par_ids, class...Var_ids,class... Op_non_serials>
auto sample_parameters_calculate_variables_Operation(const quimulun<Fs...>& qui,  x_random&& mt,
                                                     myselect<Par_ids...>,myselect<Var_ids...>,
                                                     Operation_serial<Op_non_serials...>&& operations,Datas&&... d)
{
  auto new_parameters=sample_Operation_on(qui,mt,myselect<Par_ids...>{},operations,std::forward<Datas>(d)...);
  auto new_variables=calculate_Operation_on(Calculator_new{},qui,myselect<Var_ids...>{},operations,std::forward<Datas>(d)...);



  if constexpr (new_parameters.size()+new_variables.size()==0)
  {
    if constexpr(sizeof... (Par_ids)+sizeof... (Var_ids)==0)
      return std::move(operations);
    else
      return check_if_reacheable(new_parameters+new_variables);
    //     return check_if_reacheable(Cs<Cs<Par_ids...>,Cs<Var_ids...>>{});
  }
  else
  {

    using  new_par_ids=transfer_t<pack_difference_t<Cs<Par_ids...>,typename decltype (new_parameters)::myIds>,myselect<>>;
    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;

    return sample_parameters_calculate_variables_Operation(qui,std::forward<x_random>(mt),new_par_ids{},new_var_ids{},
                                                           operations+(clean_Error<Operation_non_serial<>>(std::move(new_parameters))+
                                                                         clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}


template <class Op, class...Fs,class...Datas,  class...Var_ids,class... Op_non_serials>
auto calculate_variables_Operation(Op,const quimulun<Fs...>& qui,  myselect<Var_ids...>,
                                   Operation_serial<Op_non_serials...>&& operations,Datas&&... d)
{
  auto new_variables=calculate_Operation_on(Op{},qui,myselect<Var_ids...>{},operations,std::forward<Datas>(d)...);



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

    using  new_var_ids=transfer_t<pack_difference_t<Cs<Var_ids...>,typename decltype (new_variables)::myIds>,myselect<>>;

    return calculate_variables_Operation(Op{},qui,new_var_ids{},
                                         operations+(clean_Error<Operation_non_serial<>>(std::move(new_variables))),std::forward<Datas>(d)...);

  }
}







template <class> struct extract_distribution_Id_new{using type=Cs<>;};

template< class Id,class Distribution, class... Xs>
struct extract_distribution_Id_new<
    D<Id,Distribution,Arguments<Xs...>>

    >{using type=Cs<Id>;};

template<class Id,class Calc,class Fi, class... Xs>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments<Xs...>>
    >{using type=Cs<Id>;};


template<class Id,class Calc,class Fi, class... Xs, class... Is>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments<Xs...>, Index_struct<Is...>>
    >{using type=Cs<Id>;};



template<class C> using extract_distribution_Id_new_t=typename extract_distribution_Id_new<C>::type;


template <class...> struct extract_function_Id_new;
template <class C> struct extract_function_Id_new<C>
{using type=Cs<>;};

template< class Id,class G, class... Xs>
struct extract_function_Id_new<Coord_new<Id,G,Xs...>>{using type=Cs<Id>;};

template<class Id,class Calc,class Fi, class... Xs>
struct extract_function_Id_new<
    Fq_new<Id,Calc,Fi,Arguments<Xs...>>
    >{using type=Cs<Id>;};

template<class Id,class G, class... Xs>
struct extract_function_Id_new<
    F_new<Id,G,Arguments<Xs...>>
    >{using type=Cs<Id>;};


template< class... Ids>
struct extract_distribution_Id_new<Cs<Ids...>>{using type=pack_concatenation_t<extract_distribution_Id_new_t<Ids>...>;};




template<class C> using extract_function_Id_new_t=typename extract_function_Id_new<C>::type;

template< class... Ids>
struct extract_function_Id_new<Cs<Ids...>>{using type=pack_concatenation_t<extract_function_Id_new_t<Ids>...>;};







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
  auto s= (Operation_non_serial<>{}+...+calculate_Operation_Id(logProbability{},qui,Ids{},std::forward<Datas>(d)...));
 // auto s=(Calculate_Operation_on_this(logProbability{},qui[Ids{}],qui,std::forward<Datas>(d)...)+...);


    using  new_var_ids=transfer_t<pack_difference_t<Cs<Ids...>,typename decltype (s)::myIds>,myselect<>>;

    if constexpr (new_var_ids{}.size > 0)
      return check_if_reacheable(s);
    else
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



template <class Op, class...Fs,class...Datas>
auto calculate_Operation(Op,const quimulun<Fs...>& qui,  Datas&&... d)
{
//  using test=typename Op::geg;
  using variables_ids=extract_function_Id_new_t<Cs<Fs...>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  return calculate_variables_Operation(Op{},qui,Variables_ids{}, Operation_serial<>{},std::forward<Datas>(d)...);
}




template <template<class, class>class f_i,class e_i, class Value_type,class...Datas, class x_random>
auto myInvoke_Operation(Sample,const f_i<e_i,Value_type>& f, x_random& mt, Datas&&... d)
{
  return sample_Operation(f(),mt,std::forward<Datas>(d)...);
}






template <class Op, template<class, class>class f_i,class e_i, class Value_type,class...Datas>
auto myInvoke_Operation(Op,const f_i<e_i,Value_type>& f, Datas&&... d)
{
  return calculate_Operation(Op{},f(),std::forward<Datas>(d)...);
}



template <class Distribution,class...Datas, class x_random>
auto myInvoke_Operation(Sample s,Distribution di, x_random& mt, Datas&&... d)
{
  using res_value_type=decltype (myInvoke(s,di,mt,std::forward<Datas>(d)...));
  return Result<res_value_type>{};
}




#endif // QM_TENSOR_MODEL_EVALUATION_NEW_H
