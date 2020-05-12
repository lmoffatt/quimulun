#ifndef QM_TENSOR_MODEL_OPERATION_EVALUATION_NEW_H
#define QM_TENSOR_MODEL_OPERATION_EVALUATION_NEW_H

#include "qm_tensor_model_operations_new.h"



template < class inner_type,class... results, typename = std::enable_if_t<std::is_same_v<inner_type,Outer_Id_t<inner_type >>>>
auto& get_inner_type(Result<inner_type>,vector_tuple<results...>& x){
  return std::get<inner_type>(x.getTuple());
}

template < class... inner_types,class... results, typename = std::enable_if_t<(!std::is_same_v<inner_types,Outer_Id_t<inner_types >>&&...)>>
auto get_inner_type(Result<vector_tuple<inner_types...>>, vector_tuple<results...>& tu){

  auto & x= std::get<Outer_Id_t<vector_tuple<inner_types...>>>(tu.getTuple());

  return vector_tuple<decltype (get_inner_type(Result<inner_types>{},x))...>(get_inner_type(Result<inner_types>{},x)...);

}

template < class... inner_types,class... results, typename = std::enable_if_t<(!std::is_same_v<inner_types,Outer_Id_t<inner_types >>&&...)>>
auto get_inner_type(Result<vector_space<inner_types...>>, vector_tuple<results...>& tu){

  auto & x= std::get<Outer_Id_t<vector_space<inner_types...>>>(tu.getTuple());
  return vector_space(get_inner_type(Result<inner_types>{},x)...);
}

template <class... Xs, class... inner_types,class... results, typename = std::enable_if_t<(!std::is_same_v<inner_types,Outer_Id_t<inner_types >>&&...)>>
auto get_inner_type(Result<vector_field<vec<Xs...>,vector_space<inner_types...>>>, vector_tuple<results...>& tu){

  auto & x= std::get<Outer_Id_t<vector_field<vec<Xs...>,vector_space<inner_types...>>>>(tu.getTuple());
  return vector_space(get_inner_type(Result<inner_types>{},x)...);
}

template <class Id, class value_type,class... Xs, class... x_is,
          typename = std::enable_if_t<(!is_this_template_class_v<Out,Id> &&
                                       ! std::is_same_v<Nothing,decltype (vector_space<x_is...>{}[Id{}])>)>>
auto get_inner_type(Result<x_i<Id,value_type>>, vector_field<vec<Xs...>,vector_space<x_is...>>& tu){

  return x_i_vector_field_non_const(tu,Id{});
}

template <class Id, class value_type,class... Xs, class... x_is,
          typename = std::enable_if_t<(!std::is_same_v<Nothing,decltype (vector_space<x_is...>{}[Id{}])>)>>
//typename = std::enable_if_t<(!std::is_same_v<x_i<Id,value_type> const&,decltype (vector_space<x_is...>{}[Id{}])>)>>
auto get_inner_type(Result<x_i<Out<Id>,value_type>>, vector_field<vec<Xs...>,vector_space<x_is...>>& tu){

  return x_i(Out<Id>{},x_i_vector_field_non_const(tu,Id{}));
}


template <class Id, class value_type,class... x_is,
          typename = std::enable_if_t<(std::is_same_v<x_i<Id,value_type> const&,decltype (vector_space<x_is...>{}[Outer_Id_t<Id>{}])>)>>
auto get_inner_type(Result<x_i<Id,value_type>>, vector_space<x_is...>& v){
  return make_x_i_view(Id{},v[Id{}]());
}




template<class...v_is>
struct Outer_Id<
    vector_tuple<v_is...>
    > { using type=vector_tuple<Outer_Id_t<v_is>...>;};

template<class... xs,class... Xs>
struct Outer_Id<
    vector_field<vec<Xs...>,vector_space<xs...>>
    > { using type=vector_field<vec<Xs...>,vector_space<Outer_Id_t<xs>...>>;};

template<class... xs>
struct Outer_Id<
    vector_space<xs...>
    > { using type=vector_space<Outer_Id_t<xs>...>;};

template<class Id, class Value>
struct Outer_Id<
    x_i<Id,Value>
    > { using type=x_i<Outer_Id_t<Id>,Value>;};




template<class> struct Current;





template<class R,class... Ops, class...Fs, class random, class... Datas>
void Invoke_on_elem(R& r,Instructions_serial<Ops...> ,const quimulun<Fs...>& f,random& mt,Datas&&...d)
{
  Invoke_on(r,Instructions_serial<Ops...> {},f, mt,std::forward<Datas>(d)...);
}

template<class R,class... Ops, class...Fs, class random, class... Datas>
void Invoke_on_elem(R& r,Instructions_sum<Ops...> ,const quimulun<Fs...>& f,random& mt,Datas&&...d)
{
  Invoke_on(r,Instructions_sum<Ops...> {},f, mt,std::forward<Datas>(d)...);
}




template<class ...x_is, class Position,class... Fs,class random,class... Datas>
void Invoke_on_Serial_pos(vector_space<x_is...>& ,Operation_vector<>,const quimulun<Fs...>& ,const Position& , random& ,Datas&&...)
{}


template<class ...x_is, class Op, class... Ops,class Fid,class... Fids,class Position,class... Fs,class random,class... Datas, typename =std::enable_if_t<!is_this_template_class_v<Out,Fid>>>
void Invoke_on_Serial_pos(vector_space<x_is...>& x,Operation_vector<Instruction<Op,Fid>,Instruction<Ops,Fids>...>,const quimulun<Fs...>& qui,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_pos(x[non_const<Fid>{}],Op{},qui[Fid{}], p,mt,qui,std::forward<Datas>(d)...);
  Invoke_on_Serial_pos(x,Operation_vector<Instruction<Ops,Fids>...>{},qui,p,mt,x[Fid{}],std::forward<Datas>(d)...);
}


template<class ...x_is, class Op, class... Ops,class Fid,class... Fids,class Position,class... Fs,class random,class... Datas>
void Invoke_on_Serial_pos(vector_space<x_is...>& x,Operation_vector<Instruction<Op,Out<Fid>>,Instruction<Ops,Fids>...>,const quimulun<Fs...>& qui,const Position& p, random& mt,Datas&&...d)
{
  Invoke_on_pos(x[non_const<Fid>{}],Op{},qui[Out<Fid>{}], p,mt,qui,std::forward<Datas>(d)...);
  Invoke_on_Serial_pos(x,Operation_vector<Instruction<Ops,Fids>...>{},qui,p,mt,x_i_view_const(Out<Fid>{},x[Fid{}]),std::forward<Datas>(d)...);
}

template<class ...Ups,class... Downs, class Value_type,class... Ops,class... Fids,class Position,class... Fs,class random,class... Datas>
void Invoke_on_Serial_pos(x_i<logpr<up<Ups...>,dn<Downs...>>,Value_type>& x,Operation_vector<Instruction<Ops,Fids>...>,const quimulun<Fs...>& qui,const Position& p, random& mt,Datas&&...d)
{
  (Invoke_on_sum(x,Ops{},qui[Fids{}], p,mt,qui,std::forward<Datas>(d)...),...);;
}





template< class ...x_is, class... Xs, class... Ops,class... Fids, template<class...>class Arguments,class ...Args, class... Fs, class random,class ...Datas>
void Invoke_on(vector_field<vec<Xs...>,vector_space<Outer_Id_t<x_is>...>>& out,
               Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Operation_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,const quimulun<Fs...>& qui, random& mt,Datas&&...d)
{
  using myResult_type=typename Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<Outer_Id_t<x_is>...>>>, Operation_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>::myResult_type;
  using myvec=vec<Xs...>;
  using myValue_type= typename myResult_type::value_type;


  myValue_type outf;
  auto p=myResult_type::begin();
  //    using test=typename Cs<Fids...,myvec,Xs...,decltype (get_from_new(Xs{},d...))...>::sgrek;
  //    using test2=typename Cs< Datas...>::datas;
  fill_vector_field_new(outf,p,myvec{},get_from_new/*_not_Nothing*/(Args{},std::forward<Datas>(d)...)...);
  //    fill_vector_field_new(outf,p,myvec{},get_from_new(Xs{},d...)...);

  out=myResult_type(std::move(outf));
  p=out.begin();
  std::vector<std::decay_t<decltype (p)>> allpos;
  do
  {
    allpos.push_back(p);
  } while(out.next(p));

  //#pragma omp parallel for
  for (std::size_t i=0; i<allpos.size(); ++i)
  {
    Invoke_on_Serial_pos(out(allpos[i]),Operation_vector<Instruction<Ops,Fids>...>{},qui,allpos[i],mt,std::forward<Datas>(d)...);

  }

}


template< class Value_type, class... Ops,class... Fids, template<class...>class Arguments,class ...Args, class... Fs, class random,class ...Datas>
void Invoke_on(Value_type& out,
               Instructions_sum<Result<Value_type>, Operation_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,const quimulun<Fs...>& qui, random& mt,Datas&&...d)
{

  auto p=Position<>{};
  Invoke_on_Serial_pos(out,Operation_vector<Instruction<Ops,Fids>...>{},qui,p,mt,std::forward<Datas>(d)...);

}




template< class x_is, class... Xs, class Op,class Fid, template<class...>class Arguments,class ...Args, class... Fs, class random,class ...Datas>
void Invoke_on(vector_field<vec<Xs...>,vector_space<x_is>>& out,Instructions_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is>>>, Operation_vector<Instruction<Op,Fid>>,Arguments<Args...>>,const quimulun<Fs...>& qui, random& mt,Datas&&...d)
{
  using myValue_type= vector_space<x_is>;
  using myResult_type=typename Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is>>>, Operation_vector<Instruction<Op,Fid>>,Arguments<Args...>>::myResult_type;
  using myvec=vec<Xs...>;

  using Id=myId_t<x_is>;

  out=consolidate<myValue_type>(vec<Id>{},get_from_new(Args{},qui,d...)...);

  auto p=out.begin(Id{});
  std::vector<std::decay_t<decltype (p)>> allpos;
  do
  {
    allpos.push_back(p);
  } while(out.next(p));


//  do {
//    out(Id{},p)=g_(get_from(Xs{},data...)(p)...);

//  } while (out.next(p));
//  return x_i(Id{},std::move(out));


#pragma omp parallel for
  for (std::size_t i=0; i<allpos.size(); ++i)
  {

    Invoke_on_pos(out(Id{},allpos[i])[Id{}],Op{},qui[Fid{}], allpos[i],mt,qui,std::forward<Datas>(d)...);

  }


}



template< class ...x_is,  class... Ops,class... Fids, template<class...>class Arguments,class ...Args, class... Fs, class random,class ...Datas>
void Invoke_on(vector_space<x_is...>& out,Instructions_Coord<Result<vector_field<vec<>,vector_space<x_is...>>>, Operation_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,const quimulun<Fs...>& qui, random &mt,Datas&&...d)
{
  auto o=vector_space<x_is...>{};
  //using myValue_type= vector_space<x_is...>;
  auto p=Position<>{};
  Invoke_on_Serial_pos(out(p),Operation_vector<Instruction<Ops,Fids>...>{},qui,p,mt,std::forward<Datas>(d)...);

}




template< class ...x_is,  class... Ops,class... Fids, template<class...>class Arguments,class ...Args, class... Fs, class random,class ...Datas>
void Invoke_on(vector_space<x_is...>& out,Instructions_for_each<Result<vector_field<vec<>,vector_space<x_is...>>>, Operation_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,const quimulun<Fs...>& qui, random &mt,Datas&&...d)
{
  //  auto o=vector_space<x_is...>{};
  //using myValue_type= vector_space<x_is...>;
  auto p=Position<>{};
  Invoke_on_Serial_pos(out(p),Operation_vector<Instruction<Ops,Fids>...>{},qui,p,mt,std::forward<Datas>(d)...);

}



template <class... Ops,class...Fs,class...Datas, class x_random, class...  results>

void Invoke_on(vector_tuple<results...>& out,Instructions_non_serial<Ops...>, const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  (Invoke_on(std::get<Outer_Id_t<typename Ops::myResult_type>>(out.getTuple()),Ops{},qui,mt,std::forward<Datas>(d)...),...);
  // (Invoke_on(std::get<typename Ops::myResult_type>(out.getTuple()),Ops{},qui,mt,std::forward<Datas>(d)...),...);
}





template <class...Fs,class...Datas, class x_random, class...  results>
void Invoke_on(vector_tuple<results...>& ,Instructions_serial<>, const quimulun<Fs...>& , x_random& , Datas&&... )
{
}



template <class Op,class... Ops,class...Fs,class...Datas, class x_random, class...  results>

void Invoke_on(vector_tuple<results...>& out,Instructions_serial<Op,Ops...>, const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  Invoke_on(std::get<Outer_Id_t<typename Op::myResult_type>>(out.getTuple()),Op{},qui,mt,std::forward<Datas>(d)...);
  Invoke_on(out,Instructions_serial<Ops...>{},qui,mt,get_inner_type(Result<typename Op::myResult_type>{},out),std::forward<Datas>(d)...);
}






template <class... Ops,class...Fs,class...Datas, class x_random>
auto Execute_on(const Instructions_serial<Ops...> op,const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  //  auto out2=(typename Ops::myResult_type{},...);

  auto out=typename Instructions_serial<Ops...>::myResult_type{};
  Invoke_on(out,op,qui,mt,std::forward<Datas>(d)...);
  return out;
}







template <class...Fs,class...Datas, class x_random>
auto sample_loop_Operation(const quimulun<Fs...>& qui, x_random& mt, Datas&&... d)
{
  using distribution_ids=extract_distribution_Id_new_t<Cs<Fs...>>;


  using variables_ids=extract_function_Id_new_t<Cs<Fs...>>;


  using Parameters_ids=transfer_t<pack_difference_t<distribution_ids,pack_concatenation_t<myIds_t<Datas>...>>,myselect<>>;
  using Variables_ids=transfer_t<variables_ids,myselect<>>;

  auto param_ids=Parameters_ids{} | filter_by<Not<is_any_of_these_template_classes<Start_new,Next_new>>>{};
  auto start_param_ids=Parameters_ids{} | filter_by<is_any_of_these_template_classes<Start_new>>{};
  auto next_param_ids=Parameters_ids{} | filter_by<is_any_of_these_template_classes<Next_new>>{};

  auto var_ids=Variables_ids{} | filter_by<Not<is_any_of_these_template_classes<Start_new,Next_new>>>{};
  auto start_var_ids=Variables_ids{} | filter_by<is_any_of_these_template_classes<Start_new>>{};
  auto next_var_ids=Variables_ids{} | filter_by<is_any_of_these_template_classes<Next_new>>{};

  auto init_op= sample_parameters_calculate_variables_loop_init_Operation(qui,mt,param_ids,var_ids, Instructions_serial<>{},std::forward<Datas>(d)...);

  auto loop_param_ids=param_ids | filter_by<Not<is_in_this_set<myIds_t<decltype (init_op)>>>>{};
  auto loop_var_ids=var_ids | filter_by<Not<is_in_this_set<myIds_t<decltype (init_op)>>>>{};

  auto start_op=sample_parameters_calculate_variables_loop_start_Operation(qui,mt,loop_param_ids,loop_var_ids,start_param_ids,start_var_ids,Instructions_serial<>{}, init_op,std::forward<Datas>(d)...);


  //using show_init_op=typename Cs<decltype (init_op)>::test;

  //using test=typename Cs<decltype (start_op)>::test;

  auto init=typename decltype(init_op)::myResult_type{};
  Invoke_on(init,init_op,qui,mt,std::forward<Datas>(d)...);


  //auto init= Execute_on(init_op,qui,mt,std::forward<Datas>(d)...);
  auto loop=typename decltype(start_op)::myResult_type{};

  Invoke_on(loop,start_op,qui,mt,init,std::forward<Datas>(d)...);

  return loop;


}




















#endif // QM_TENSOR_MODEL_OPERATION_EVALUATION_NEW_H
