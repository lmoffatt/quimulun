#ifndef QM_TENSOR_MODEL_OPERATIONS_NEW_H
#define QM_TENSOR_MODEL_OPERATIONS_NEW_H

#include "qm_tensor_model_evaluation_new.h"


template<class ...> struct Instructions_for_each;
template<class ...> struct Instructions_for_each_new;
template<class ...> struct Instructions_Update_for_each;
template<class ...> struct Instructions_Coord;

template<class ...> struct Instructions_sum;
template<class ...> struct Instructions_serial;
template<class ...> struct Instructions_non_serial;

template<class...>struct Instructions_vector{};

template<class Operator, class Id,class Argument, class... Argumentss> struct Instruction_new{};

template <class... Instuctions> struct serial_ins{};
template <class... Instuctions> struct non_serial_ins{};



template<class... vfs1, class... vfs2>
constexpr auto operator &&(Result<vector_tuple<vfs1...>>&&,Result<vector_tuple<vfs2...>>&&)
{
  return Result<vector_tuple<vfs1...,vfs2...>>{};
}
template< class ...x_is2, class... X2s>
constexpr auto operator+(
    Result<vector_tuple<>>&&,
    Result<vector_field<vec<X2s...>,vector_space<x_is2...>>>&&)
{

  return    Result<vector_tuple<vector_field<vec<X2s...>,vector_space<x_is2...>>>>{};

}

template<class ...x_is, class ...x_is2,class... Xs, class... X2s,class... Ops,class... Fids, template<class, class>class x_i,class e_i,class value_type,
          typename= std::enable_if_t<(same_pack_set(Cs<Xs...>{},Cs<X2s...>{})&&!is_this_template_class_v<logpr,e_i>)>>
constexpr auto operator+(
    Result<vector_field<vec<Xs...>,vector_space<x_is...>>>&&,
    Result<vector_field<vec<X2s...>,vector_space<x_i<e_i,value_type>,x_is2...>>>&&)
{

  return Result<vector_field<vec<Xs...>,decltype(std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i<e_i,value_type>,x_is2...>>())>>{};
}

template<class ...x_is, class ...vectorfields, class ...x_is2,class... Xs, class... X2s,
          typename= std::enable_if_t<!(same_pack_set(Cs<Xs...>{},Cs<X2s...>{}))>>
constexpr auto operator+(
    Result<vector_field<vec<Xs...>,vector_space<x_is...>>>&&,
    Result<vector_field<vec<X2s...>,vector_space<x_is2...>>>&&)
{

  return     Result<vector_tuple<vector_field<vec<Xs...>,vector_space<x_is...>>,vector_field<vec<X2s...>,vector_space<x_is2...>>>>{};

}




template<class ...x_is, class ...vectorfields, class ...x_is2,class... Xs, class... X2s,class... Ops,class... Fids, template<class, class>class x_i,class e_i,class value_type,
          typename= std::enable_if_t<(same_pack_set(Cs<Xs...>{},Cs<X2s...>{})&&!is_this_template_class_v<logpr,e_i>)>>
constexpr auto operator+(
    Result<vector_tuple<vector_field<vec<Xs...>,vector_space<x_is...>>,vectorfields...>>&&,
    Result<vector_field<vec<X2s...>,vector_space<x_i<e_i,value_type>,x_is2...>>>&&)
{

  return Result<
      vector_tuple<vector_field<vec<Xs...>,decltype(std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i<e_i,value_type>,x_is2...>>())>,vectorfields...>>{};
}




template<class ...x_is, class ...vectorfields, class ...x_is2,class... Xs, class... X2s,
          typename= std::enable_if_t<!(same_pack_set(Cs<Xs...>{},Cs<X2s...>{}))>>
constexpr auto operator+(
    Result<vector_tuple<vector_field<vec<Xs...>,vector_space<x_is...>>,vectorfields...>>&&,
    Result<vector_field<vec<X2s...>,vector_space<x_is2...>>>&&)
{

  return     Result<vector_tuple<vector_field<vec<Xs...>,vector_space<x_is...>>>>{}&&
         (Result<vector_tuple<vectorfields...>>{}+Result<vector_field<vec<X2s...>,vector_space<x_is2...>>>{});

}

template<class ...Ups, class ...Downs, class Value_one,
          class ...Ups2, class ...Downs2, class Value_two, class... Xs, class... X2s,
          typename= std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(Result<vector_field<vec<Xs...>,vector_space<x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>>>>&&,
                         Result<vector_field<vec<X2s...>,vector_space<x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>>>>&&)
{
  return
      Result<vector_field<vec<Xs...>,vector_space<decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>{})>>>{};

}




template<>
struct Instructions_non_serial<>
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



template<class Instruction, class ...Instructions>
struct Instructions_non_serial<Instruction,Instructions...>
{

  using myResult_type=
      //std::conditional_t<(sizeof... (Instructions) >0),
      vector_tuple<typename Instruction::myResult_type,typename Instructions::myResult_type... >
      //                  ,typename Instruction::myResult_type>
      ;

  static constexpr auto size() { return (Instruction::size()+...+Instructions::size());}

  using myIds=pack_concatenation_t<typename Instruction::myIds,typename Instructions::myIds...>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    return (Instruction{}[Id{}]||...||Instructions{}[Id{}]);
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    return (Instruction{}(qui,std::forward<Datas>(d)...)+...+Instructions{}(qui,std::forward<Datas>(d)...));
  }

};

template<class Instruction,class ...Instructions>
struct Instructions_serial<Instruction,Instructions...>
{

  using myResult_type=
      //    std::conditional_t<(sizeof... (Instructions) >0),
      vector_tuple<typename Instruction::myResult_type,typename Instructions::myResult_type... >
      //                     ,typename Instruction::myResult_type>
      ;

  using myIds=pack_concatenation_t<typename Instruction::myIds,typename Instructions::myIds...>;


  template<class Id>
  constexpr auto operator[](Id)const
  {
    //using test=typename Id::test;
    //using test2=typename Cs<decltype (Instruction{}[Id{}]),decltype (Instructions{}[Id{}])...>::test;
    return (Instruction{}[Id{}]||...||Instructions{}[Id{}]);
  }

  template<class... Fs, class ...Datas>
  auto operator ()(const quimulun<Fs...>& qui, Datas&&...d)
  {
    auto out0=Instruction{}(qui,std::forward<Datas>(d)...);
    auto const& vec=out0;
    auto out1=Instructions_serial<Instructions...>{}(qui,vec,std::forward<Datas>(d)...);
    return std::move(out0)+std::move(out1);
  }

};


template<class... Instructions_non_serials,class... Instructions_fors >
auto operator+(Instructions_serial<Instructions_non_serials...>,Instructions_non_serial<Instructions_fors...> )
{
  return  Instructions_serial<Instructions_non_serials..., Instructions_non_serial<Instructions_fors...>>{};
}


template< class ...x_is, class... Xs, class... Ops,class... Fids, class ...Arguments>
struct Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments...>

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










template< class... Xs, class x, class... Ops,class... Fids, class ...Arguments>
struct Instructions_for_each_new<vec<Xs...>,Result<x>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments...>

{
  template<class Id>
  constexpr auto operator[](Id)const
  {

//    using test=typename std::conditional_t<!std::is_same_v<Nothing,decltype (get_at(std::declval<x&>(),Id{})) >,
//        Cs<Id,decltype (get_at(std::declval<x&>(),Id{})),x>,std::vector<double>>::value_type;
    return  Result_t<std::decay_t<decltype (get_at(std::declval<x&>(),Id{}))>>{};
  }

  constexpr static auto size(){ return size_of_pack(myIds_t<x>{});}

  using myIds=myIds_t<x>;


   using myResult_type=x;
  //  using myValue_type= vector_space<x_is...>;
  //  using myvec=vec<Xs...>;

};


template<  class ...x_is, class... Xs, class... Pos, class... Ops,class... Fids, class ...Arguments>
struct Instructions_Update_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, vec<Pos...>,Instructions_vector<Instruction<Ops,Fids>...>,Arguments...>
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
struct Instructions_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>

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
struct Instructions_sum<Result<value_type>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>

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

template<>
struct Instructions_serial<>
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





template<class ...Ops, class... Ts>
constexpr auto operator+(Instructions_non_serial<Ops...>&&,
                         Instructions_Coord<Ts...>&&)
{
  return    Instructions_non_serial<Ops...,Instructions_Coord<Ts...>>{};
}

template<class Inst, class...Insts, class... Ts, typename=std::enable_if_t<!is_this_template_class_v<Error,Inst>>>
constexpr auto operator+(Instructions_non_serial<Instructions_Coord<Ts...>,Insts...>&&,
                         Inst&&)
{
  return    Instructions_non_serial<Instructions_Coord<Ts...>>{}&& (Instructions_non_serial<Insts...>{}+Inst{});
}



template<class ...Ops, class... Ts>
constexpr auto operator+(Instructions_non_serial<Ops...>&&,
                         Instructions_Update_for_each<Ts...>&&)
{
  return    Instructions_non_serial<Ops...,Instructions_Update_for_each<Ts...>>{};
}

template<class Inst, class...Insts, class... Ts, typename=std::enable_if_t<!is_this_template_class_v<Error,Inst>>>
constexpr auto operator+(Instructions_non_serial<Instructions_Update_for_each<Ts...>,Insts...>&&,
                         Inst&&)
{
  return    Instructions_non_serial<Instructions_Update_for_each<Ts...>>{}&& (Instructions_non_serial<Insts...>{}+Inst{});
}



template<class ...x_is, class ...x_is2,class... Xs, class... X2s,class... Ops,class... Fids, template<class, class>class x_i,class e_i,class value_type, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<(same_pack_set(Cs<Xs...>{},Cs<X2s...>{})&&!is_this_template_class_v<logpr,e_i>)>>
constexpr auto operator+(
    Instructions_non_serial<
        Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
        Operations...>&&,
    Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i<e_i,value_type>,x_is2...>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i>>())::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,typename x_is::myId...>>,Arguments<Args...>>;

  return    Instructions_non_serial<
      Instructions_for_each<
          Result<
              vector_field<vec<Xs...>,decltype(std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i<e_i,value_type>,x_is2...>>())>
              >,
          Instructions_vector<Instruction<Ops,Fids>...,Instruction<Op,Fid>>,ArgsOut >,
      Operations...>{};

}











template<class x1, class... Xs, class... X2s,class... Ops,class... Fids, class x2, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename=std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(
    Instructions_non_serial<
        Instructions_for_each_new<vec<Xs...>,Result<x1>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
        Operations...>&&,
    Instructions_for_each_new<vec<X2s...>,Result<x2>,Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i>>())::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,pack_concatenation_t<Cs<Args...>,myIds_t<x1>>>,Arguments<Args...>>;

  return    Instructions_non_serial<
      Instructions_for_each_new<vec<Xs...>,
                                decltype(Result<x1>{}+
                                         Result<x2>{}),
                                Instructions_vector<Instruction<Ops,Fids>...,Instruction<Op,Fid>>,ArgsOut >,
      Operations...>{};

}






template<class ...Ups, class ...Downs, class Value_one,
          class ...Ups2, class ...Downs2, class Value_two, class... Xs, class... X2s,class... Ops,class... Fids, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(Instructions_non_serial<
                             Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
                             Operations...>&&,
                         Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,logv<double,unit_one>>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,logv<double,unit_two>>{})::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,logpr<up<Ups...>,dn<Downs...>>>>,Arguments<Args...>>;

  return    Instructions_non_serial<
      Instructions_for_each<
          Result<
              vector_field<vec<Xs...>,vector_space<decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>{})>
                           >>,
          Instructions_vector<Instruction<Ops,Fids>...,Instruction<Op,Fid>>,ArgsOut >,
      Operations...>{};

}



template<class ...Ups, class ...Downs, class Value_one,
          class ...Ups2, class ...Downs2, class Value_two, class... Xs, class... X2s,class... Ops,class... Fids, class Op, class Fid,class... Operations, class... Args, class...Args2,
          typename= std::enable_if_t<same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
constexpr auto operator+(
    Instructions_non_serial<
        Instructions_for_each_new<vec<Xs...>,Result<vector_field<vec<Xs...>,vector_space<x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
        Operations...>&&,
    Instructions_for_each_new<vec<X2s...>,Result<vector_field<vec<X2s...>,vector_space<x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,logv<double,unit_one>>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,logv<double,unit_two>>{})::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...,logpr<up<Ups...>,dn<Downs...>>>>,Arguments<Args...>>;

  return    Instructions_non_serial<
      Instructions_for_each_new<vec<Xs...>,
                                Result<
                                    vector_field<vec<Xs...>,vector_space<decltype (x_i<logpr<up<Ups...>,dn<Downs...>>,Value_one>{}+x_i<logpr<up<Ups2...>,dn<Downs2...>>,Value_two>{})>
                                                 >>,
                                Instructions_vector<Instruction<Ops,Fids>...,Instruction<Op,Fid>>,ArgsOut >,
      Operations...>{};

}






//template<class ...x_is, class... Xs, class... Ops,class... Fids, class x_i, class ...X2s,class Op, class Fid,class... Operations,class... Args, class... Args2>
//auto operator+(Instructions_non_serial<
//                   Instructions_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
//                   Operations...>&&,
//               Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
//{
//  return    Instructions_non_serial<
//             Instructions_Coord<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>>{}&&
//         (Instructions_non_serial<Operations...>{}+
//          Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>{});

//}




template<class ...x_is,class ...x_is2, class... Xs, class... Ops,class... Fids, class x_i, class ...X2s,class Op, class Fid,class... Operations,class... Args, class... Args2,
          typename=std::enable_if_t<!same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
auto operator+(Instructions_non_serial<
                   Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,
                   Operations...>&&,
               Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i,x_is2...>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{
  return    Instructions_non_serial<
             Instructions_for_each<Result<vector_field<vec<Xs...>,vector_space<x_is...>>>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>>{}&&
         (Instructions_non_serial<Operations...>{}+
          Instructions_for_each<Result<vector_field<vec<X2s...>,vector_space<x_i,x_is2...>>>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>{});

}


template<class x1,class x2, class... Xs, class... Ops,class... Fids,  class ...X2s,class Op, class Fid,class... Operations,class... Args, class... Args2,
          typename=std::enable_if_t<!same_pack_set(Cs<Xs...>{},Cs<X2s...>{})>>
auto operator+(Instructions_non_serial<
                   Instructions_for_each_new<vec<Xs...>,Result<x1>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>,Operations...>&&,
               Instructions_for_each_new<vec<X2s...>,Result<x2>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{
  return    Instructions_non_serial<
             Instructions_for_each_new<vec<Xs...>,Result<x1>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>>{}&&
         (Instructions_non_serial<Operations...>{}+
          Instructions_for_each_new<vec<X2s...>,Result<x2>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>{});

}





template<class Value_type,class Value_type2,class... Ops,class... Fids,  class Op, class Fid,class... Args, class...Args2>
constexpr auto operator+(    Instructions_sum<Result<Value_type>, Instructions_vector<Instruction<Ops,Fids>...>,Arguments<Args...>>&&,
                         Instructions_sum<Result<Value_type2>, Instructions_vector<Instruction<Op,Fid>>,Arguments<Args2...>>&&)
{

  //using test=typename decltype (std::declval<vector_space<x_is...>>()+std::declval<vector_space<x_i>>())::vector_sum;


  using ArgsOut=transfer_t<pack_difference_t<Cs<Args2...>,Cs<Args...>>,Arguments<Args...>>;

  return
      Instructions_sum<
          Result<
              decltype(std::declval<Value_type>()+std::declval<Value_type2>()) >,
          Instructions_vector<Instruction<Ops,Fids>...,Instruction<Op,Fid>>,ArgsOut >{};

}




template< class... Ops>
auto operator+(Instructions_non_serial<>&&,
               Instructions_for_each<Ops...>
                   &&)
{
  return    Instructions_non_serial<Instructions_for_each<Ops...>
                                 >{};
}

template< class... Ops>
auto operator+(Instructions_non_serial<>&&,
               Instructions_for_each_new<Ops...>
                   &&)
{
  return    Instructions_non_serial<Instructions_for_each_new<Ops...>
                                 >{};
}



template<class... Ts, class... Es>
auto operator+(Instructions_non_serial<Error<Es...>>&&,
               Instructions_for_each<Ts...>&&)
{
  return    Instructions_non_serial<Instructions_for_each<Ts...>>{};
}


template<class... Ts, class... Es>
auto operator+(Instructions_non_serial<Error<Es...>>&&,
               Instructions_for_each_new<Ts...>&&)
{
  return    Instructions_non_serial<Instructions_for_each_new<Ts...>>{};
}



template<class... Ts, class... Es>
auto operator+(Instructions_non_serial<Error<Ts...>>&&,
               Error<Es...>&&)
{
  return    Instructions_non_serial<Error<Ts...,Es...>>{};
}


template<class... Ts, class... Es>
auto operator+(Instructions_non_serial<Ts...>&&,
               Error<Es...>&&)
{
  return    Instructions_non_serial<Ts...>{};
}


template<class... Ts>
auto operator+(Instructions_non_serial<>&&,
               Error<Ts...>&&)
{
  return    Instructions_non_serial<Error<Ts...>>{};
}




template<class... Ops1, class... Ops2>
auto operator&&(Instructions_non_serial<Ops1...>&&,
                Instructions_non_serial<Ops2...>&&)
{
  return    Instructions_non_serial<Ops1..., Ops2...>{};
}


template<class... Ops1, class... Ops2>
auto operator+(Instructions_non_serial<Ops1...>&&,
               Instructions_non_serial<Ops2...>&&)
{
  return    (Instructions_non_serial<Ops1...>{}+...+ Ops2{});
}




template<class, class, class, class> struct Instructions_loop;


template<class... init_ops, class... start_ops, class... loop_ops,class... next_ops >
struct Instructions_loop<Instructions_serial<init_ops...>, Instructions_serial<start_ops...>, Instructions_serial<loop_ops...>, Instructions_serial<next_ops...>>
{
  Instructions_loop(Instructions_serial<init_ops...>, Instructions_serial<start_ops...>, Instructions_serial<loop_ops...>, Instructions_serial<next_ops...>){}
};












#endif // QM_TENSOR_MODEL_OPERATIONS_NEW_H
