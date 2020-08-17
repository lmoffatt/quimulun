#ifndef QM_TENSOR_MODEL_EVALUATION_NEW_H
#define QM_TENSOR_MODEL_EVALUATION_NEW_H

#include "qm_tensor_model_declaration.h"
#include "qm_tensor_model_declaration_new.h"
#include "qm_vector_field_evaluation.h"
#include "qm_vector_tuple_declaration.h"
#include "qm_tensor_distribution_evaluation.h"


template <class anId, class X
          , typename =std::enable_if_t< (only_xi_of_fi_v<X>)&&(!is_this_template_class_v<non_const , anId>)  ,int> >
decltype(auto) get_xi_from_this_new(anId, X&& xi)//->std::conditional_t<std::is_same_v<anId,typename std::decay_t<X>::myId >, decltype (std::forward<X>(xi)),Nothing>
{
  if constexpr (std::is_same_v<anId,typename std::decay_t<X>::myId >)
    return std::forward<X>(xi);
  else
    return Nothing{};
}

template <class anId, class X
          , typename =std::enable_if_t< (only_xi_of_fi_v<X>) > >
decltype(auto) get_xi_from_this_new(non_const<anId>, X&& xi)
//   ->std::conditional_t<std::is_same_v<anId,typename std::decay_t<X>::myId >, decltype (std::forward<X>(xi)),Nothing>
{
  if constexpr (std::is_same_v<anId,typename std::decay_t<X>::myId >)
    return std::forward<X>(xi);
  else
    return Nothing{};
}


template <class anId, class V
          , typename =std::enable_if_t< !(only_xi_of_fi_v<V>) >
          >
auto get_xi_from_this_new(anId, V&& xi)->decltype (only_xi_or_fi(std::forward<V>(xi)[anId{}]))
{
  return only_xi_or_fi(std::forward<V>(xi)[anId{}]);
}


template <class anId, class...Datas
          , typename =std::enable_if_t<               ( !is_any_of_these_template_classes<all,Size_at_Index_new,pos_new,all_new,sub,applyPosition,Size,pass_id, std::tuple>::template value<anId>) >>
constexpr auto get_from_new(anId, Datas&&...ds)->decltype ((get_xi_from_this_new(anId{},std::forward<Datas>(ds))||...))
{
  //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
  return (get_xi_from_this_new(anId{},std::forward<Datas>(ds))||...);
}


template<class... xis>
auto toTuple(xis const&...x){ return x_i_tuple_view_const<std::decay_t<xis>...>{x...};}

template <class... anId, class...Datas>
auto get_from_new(std::tuple<anId...>, Datas&&...ds)//->decltype ((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...))
{

  if constexpr ((std::is_same_v<decltype (get_from_new(anId{},ds...)),Nothing > ||...))
  {   //  using test=typename Cs<anId...>::FAILS;
    // using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::FAILS;
    return Nothing{};
  }else
  {
    //    using test=typename Cs<anId...>::OK;
    //     using test2=typename decltype(std::forward_as_tuple(get_from(anId{},ds...)...))::OK;
    //    return x_i(Cs<anId...>{},v(std::forward_as_tuple(get_from(anId{},ds...)...),dimension_less{}));
    return toTuple(get_from_new(anId{},ds...)...);
  }}



template <class Id, class Index, class...Datas>
auto get_from_new(Size_at_Index_new<Id, Index>, Datas&&...ds)
{

  auto&& x=get_from_new(Id{},std::forward<Datas>(ds)...)();
  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<std::decay_t<decltype (x)>,Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return vector_field_size(x,Index{});
}




template <class Id, class...Datas>
auto get_from_new(Size<Id>, Datas&&...ds)
{

  auto x=get_from_new(Id{},std::forward<Datas>(ds)...);
  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return vector_space_size(x());
}
template <class Id,  class...Datas>
auto get_from_new(pos_new<Id>, Datas&&...ds)
{

  auto x=(get_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);
  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return vector_field_Position(Id{},x());
}

template <class Id,  class  IndexId, class...Datas>
auto get_from_new(applyPosition<Id,IndexId>, Datas&&...ds)
{
  // using test=typename Cs<applyPosition<Id,IndexId>, decltype(get_from_new(Id{},std::forward<Datas>(ds)...)),decltype(get_from_new(IndexId{},std::forward<Datas>(ds)...))>::tew;

  auto&& x=get_from_new(Id{},std::forward<Datas>(ds)...)();


  auto&& p=get_from_new(IndexId{},std::forward<Datas>(ds)...)();

  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<decltype (x),Nothing >||std::is_same_v<decltype (p),Nothing >)
    return Nothing{};
  else
    return vector_field_subElement(x,p);
}

template <class Id,  class  IndexId, class...Datas>
decltype(auto) get_from_new(sub<Id,IndexId>, Datas&&...ds)
{

  //  using test=typename Cs<sub<Id, IndexId>>::gewlj;
  //  using test2=typename Cs<Id,Result<decltype (get_from_new(Id{},std::forward<Datas>(ds)...))>>::test2;
  //  using test3=typename Cs<IndexId,Result<decltype (get_at(get_from_new(Id{},std::forward<Datas>(ds)...)(),IndexId{}))>>::test2;
  return get_at_recursive(get_from_new(Id{},std::forward<Datas>(ds)...)(),IndexId{});
}


template <class Id,   class...Datas>
auto get_from_new(all_new<Id>, Datas&&...ds)
{

  auto x=(get_xi_from_this_new(Id{},std::forward<Datas>(ds))||...);

  //using test=typename decltype (x)::trst;
  if constexpr(std::is_same_v<decltype (x),Nothing >)
    return Nothing{};
  else
    //using test=typename Cs<decltype(std::forward<Datas>(ds)[anId{}])...>::tew;
    return all_view_const_new(x);
}


template <class Id,   class...Datas>
auto get_from_new_not_Nothing(Id, Datas&&...ds)
    ->std::enable_if_t<!std::is_same_v<decltype (get_from_new(Id{},std::forward<Datas>(ds)...)),Nothing >,decltype (get_from_new(Id{},std::forward<Datas>(ds)...))>
{
  return get_from_new(Id{},std::forward<Datas>(ds)...);
}




template<class T>
struct getId_of {using type=typename T::myId;};


template<class T>
using myId_t=typename getId_of<std::decay_t<T>>::type;



template<class...Datas>
auto myInvoke(Glue_new,Datas&&...d)
{
  //using test=typename decltype (vector_space{make_view(std::forward<Datas>(d))...})::test;
  return vector_space{make_view(std::forward<Datas>(d))...};
}
template<class  T>
T myInvoke(Conditional_choice,const v<bool,p<>> accept, T&& one, T && other )
{
  return accept.value()? std::move(std::forward<T>(one)): std::move(std::forward<T>(other));
}



template< class...Datas>
auto myInvoke(IndexCoordinate,Datas&&...d)
{
  return std::invoke(IndexCoordinate{},std::forward<Datas>(d)...);
}


template <class F, class G, class...Datas>
auto myInvoke(FoG<F,G> f,Datas&&...d)
{
  return myInvoke(f.f(),myInvoke(f.g(),std::forward<Datas>(d)...));
}



template<class F,  class...Datas>
auto myInvoke(F&& f,Datas&&...d)->decltype (std::invoke(std::forward<F>(f), std::forward<Datas>(d)...))
{
  return std::invoke(std::forward<F>(f), std::forward<Datas>(d)...);
}



template<class F,  class...Datas>
auto myInvoke(Calculator_new,F&& f,Datas&&...d)
{
  return myInvoke(std::forward<F>(f), std::forward<Datas>(d)...);
}


template<class R, class... Datas>
void Invoke_on_elem(R& r,Calculator_new,Glue_new,Datas&&...d)
{
  r=myInvoke(Glue_new{}, std::forward<Datas>(d)...);
}

template<class R,class F, class... Datas>
void Invoke_on_elem(R& r,Calculator_new,F&& f,Datas&&...d)
{
  r=myInvoke(Calculator_new{},std::forward<F>(f), std::forward<Datas>(d)...);
}

template<class R,class Distribution,class random, class... Datas>
void Invoke_on_elem(R& r,Sample,Distribution&& f,random& mt,Datas&&...d)
{
  r=myInvoke(Sample{},std::forward<Distribution>(f), mt,std::forward<Datas>(d)...);
}

template<class R,class Distribution,class random, class... Datas>
void Invoke_on_elem(R& r,logProbability,Distribution&& f,random& ,Datas&&...d)
{
  r=r+myInvoke(logProbability{},std::forward<Distribution>(f), std::forward<Datas>(d)...);
}


template<class R,class Id,class G, class... Xs, class... Datas>
void Invoke_on_elem(R& r,Calculator_new,const F_new<Id,G,Arguments<Xs...>>& f,Datas&&...d)
{
  r=std::invoke(f.f(), std::forward<Datas>(d)...);
}

template<class R,class Op,class Id,class Calc,class Fi, class... Xs, class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const Dq_new<Id,Calc,Fi,Arguments<Xs...>>& ,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},get_from_new(Fi{},std::forward<Datas>(d)...)(),mt, get_from_new(Xs{},std::forward<Datas>(d)...)()(p)...);
}

template<class R,class Op,class Id,class Calc,class Fi, class... Xs, class ...XXs,class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const Dq_new<Id,Calc,Fi,Arguments<Xs...>, Index_struct<XXs...>> ,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},get_from_new(Fi{},std::forward<Datas>(d)...)(),mt, get_from_new(Xs{},std::forward<Datas>(d)...)()(p)...);
}

template<class R,class Op,class Id,class Calc,class Fi, class... Xs, class Position,class random,class... Datas>
auto Invoke_on_pos(R& r,Op,const Dq_new<Id,Calc,Fi,Arguments_xi<Xs...>>& ,const Position& , random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},get_from_new(Fi{},std::forward<Datas>(d)...)(),mt, get_from_new(Xs{},std::forward<Datas>(d)...)...);
}
template<class R,class Op,class Id,class Calc,class Fi, class... Xs, class ...x_is,class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const Dq_new<Id,Calc,Fi,Arguments<Xs...>, Arguments_xi<x_is...>> ,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},get_from_new(Fi{},std::forward<Datas>(d)...)(),mt,
                 get_from_new(Xs{},std::forward<Datas>(d)...)()(p)...,get_from_new(x_is{},std::forward<Datas>(d)...)...);
}


template<class R,class Op,class Id,class Calc,class Fi, class... Xs, class Position,class random,class... Datas>
auto Invoke_on_pos(R& r,Op,const Fq_new<Id,Calc,Fi,Arguments<Xs...>>& ,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},get_from_new(Fi{},std::forward<Datas>(d)...)(), mt,get_from_new(Xs{},std::forward<Datas>(d)...)()(p)...);
}

template<class R,class Op,class Id,class Calc,class... Fs, class... Xs, class Position,class random,class... Datas>
auto Invoke_on_pos(R& r,Op,const Fq_new<Id,Calc,quimulun<Fs...>,Arguments<Xs...>>& fq,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},fq.Fq(), mt,get_from_new(Xs{},std::forward<Datas>(d)...)()(p)...);
}

template<class R,class Op,class Id,class Calc,class... Fs, class... Xs, class Position,class random,class... Datas>
auto Invoke_on_pos(R& r,Op,const Fq_new<Id,Calc,quimulun<Fs...>,Arguments_xi<Xs...>>& fq,const Position& , random& mt,Datas&&...d)
{
  Invoke_on_elem(r(),Op{},fq.Fq(), mt,get_from_new(Xs{},std::forward<Datas>(d)...)...);
}


template<class R,class Op,class Id,class G, class... Xs, class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const Coord_new<Id,G,Arguments<Xs...>>& f,const Position& p,random&, Datas&&...d)
{
  Invoke_on_elem(r().value(),Op{},f.get_G(), get_from_new(Xs{},d...)()(p)...);
}

template<class R,class Op,class Id, class... Xs, class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const F_new<Id,Glue_new,Arguments<Xs...>>& f,const Position& ,random&, Datas&&...d)
{
  Invoke_on_elem(r(),Op{},f.f(), get_from_new(Xs{},d...)...);

}

template<class R,class Op,class Id, class Distribution, class... Xs, class Position,class random,class... Datas>
void Invoke_on_sum(R& r,Op,const D<Id,Distribution,Arguments<Xs...>>& ,const Position& ,random& mt, Datas&&...d)
{
  auto && y=get_from_new(Id{},std::forward<Datas>(d)...)();

  auto loglik=std::decay_t<decltype (r())>{};
  auto pin=y.begin();

  do {
    Invoke_on_elem(loglik,Op{},Distribution{}, mt,y(pin),get_from_new(Xs{},d...)()(pin)...);
    r()=r()+loglik;
  } while(y.next(pin));

}


template<class R,class Op,class Id, class Distribution, class... Xs, class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const D<Id,Distribution,Arguments<Xs...>>& ,const Position& p,random& mt, Datas&&...d)
{
  Invoke_on_elem(r(),Op{},Distribution{}, mt,get_from_new(Xs{},d...)()(p)...);
}

template<class R,class Op,class Id, class Distribution, class... Xs, class ...XXs,class Position,class random,class... Datas>
void Invoke_on_pos(R& r,Op,const D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>>& ,const Position& p,random& mt, Datas&&...d)
{
  Invoke_on_elem(r(),Op{},Distribution{}, mt,get_from_new(Xs{},d...)()(p)...);

}


template<class R,class Op,class Id,class G, class... Xs, class Position,class random,class... Datas, typename=std::enable_if_t<!std::is_same_v<G,Glue_new >>>
auto Invoke_on_pos(R& r,Op,const F_new<Id,G,Arguments<Xs...>>& f,const Position& p,random&, Datas&&...d)
{
  // using test=typename Cs<Cs<Xs,decltype (get_from_new(Xs{},d...))>...>::ajgeg;

  Invoke_on_elem(r(),Op{},f.f(), get_from_new(Xs{},d...)()(p)...);
}




template <class> struct extract_distribution_Id_new{using type=Cs<>;};

template <class> struct extract_step_distribution_Id_new{using type=Cs<>;};
template <class> struct extract_step_function_Id_new{using type=Cs<>;};



template< class Id,class Distribution, class... Xs>
struct extract_distribution_Id_new<
    D<Id,Distribution,Arguments<Xs...>>

    >{using type=Cs<Id>;};

template< class Id,class Distribution, class... Xs, class...XXs>
struct extract_distribution_Id_new<
    D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>>

    >{using type=Cs<Id>;};



template<class Id,class Calc,class Fi, class... Xs>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments<Xs...>>
    >{using type=Cs<Id>;};

template<class Id,class Calc,class Fi, class... Xs>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments_xi<Xs...>>
    >{using type=Cs<Id>;};


template<class Id,class Calc,class Fi, class... Xs, class... x_is>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments<Xs...>,Arguments_xi<x_is...>>
    >{using type=Cs<Id>;};



template<class Id,class Calc,class Fi, class... Xs, class... Is>
struct extract_distribution_Id_new<
    Dq_new<Id,Calc,Fi,Arguments<Xs...>, Index_struct<Is...>>
    >{using type=Cs<Id>;};



template<class C> using extract_distribution_Id_new_t=typename extract_distribution_Id_new<C>::type;
template<class C> using extract_step_distribution_Id_new_t=typename extract_distribution_Id_new<C>::type;


template <class...> struct extract_function_Id_new;
template <class C> struct extract_function_Id_new<C>
{using type=Cs<>;};

template< class Id,class G, class... Xs>
struct extract_function_Id_new<Coord_new<Id,G,Xs...>>{using type=Cs<Id>;};

template<class Id,class Calc,class Fi, class... Xs>
struct extract_function_Id_new<
    Fq_new<Id,Calc,Fi,Arguments<Xs...>>
    >{using type=Cs<Id>;};
template<class Id,class Calc,class Fi, class... Xs>
struct extract_function_Id_new<
    Fq_new<Id,Calc,Fi,Arguments_xi<Xs...>>
    >{using type=Cs<Id>;};



template<class Id,class G, class... Xs>
struct extract_function_Id_new<
    F_new<Id,G,Arguments<Xs...>>
    >{using type=Cs<Id>;};

template<class Id,class G, class Y, class ...Ops,class ...Xs>
struct extract_function_Id_new<
  Update_new<Id,G,Arguments<Y>, Operations<Ops...>,Arguments<Xs...>>
    >{using type=Cs<Id>;};




template< class... Ids>
struct extract_distribution_Id_new<Cs<Ids...>>{using type=pack_concatenation_t<extract_distribution_Id_new_t<Ids>...>;};




template<class C> using extract_function_Id_new_t=typename extract_function_Id_new<C>::type;

template< class... Ids>
struct extract_function_Id_new<Cs<Ids...>>{using type=pack_concatenation_t<extract_function_Id_new_t<Ids>...>;};





#endif // QM_TENSOR_MODEL_EVALUATION_NEW_H
