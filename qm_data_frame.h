#ifndef QM_DATA_FRAME_H
#define QM_DATA_FRAME_H
#include "qm_tensor_model.h"

template <class> struct row_type;
template <class> struct row_type_w_unit;
template <class V> using row_type_t=typename row_type<V>::type;
template <class V> using row_type_w_unit_t=typename row_type_w_unit<V>::type;


template<class> struct cols;
template<class> struct cols_w_unit;

template <class C> using cols_t=typename cols<C>::type;
template <class C> using cols_w_unit_t=typename cols_w_unit<C>::type;




template<class TYPE,class myunit,class...Is>
void insert_at(v<TYPE,myunit>& me,const Position<Is...>& , row_type_t<v<TYPE,myunit>>&& r)
{
  me.value()=std::get<TYPE>(std::move(r));
}
template<class TYPE,class myunit,class...Is>
void insert_at(v<TYPE,myunit>& me,const Position<Is...>& , row_type_w_unit_t<v<TYPE,myunit>>&& r)
{
  me.value()=std::get<TYPE>(std::move(r));
}
template<class TYPE,class myunit,class...Is, class ...us, typename=std::enable_if_t<is_in_pack<myunit,us...>(),int>>
void insert_at(v<TYPE,myunit>& me,const Position<Is...>& , std::tuple<TYPE,std::variant<us...>>&& r)
{
  assert(std::get<myunit>(std::get<1>(r))==myunit{});
  me.value()=std::get<TYPE>(std::move(r));
}


template<class TYPE,class myunit> struct cols<v<TYPE,myunit>>{
  using type=Cs<value_k>;
};
template<class TYPE,class myunit> struct cols_w_unit<v<TYPE,myunit>>{
  using type=Cs<value_k,unit_k>;
};
template<class TYPE,class myunit> struct row_type<v<TYPE,myunit>>{
  using type=std::tuple<TYPE>;
};
template<class TYPE,class myunit> struct row_type_w_unit<v<TYPE,myunit>>{
  using type=std::tuple<TYPE,myunit>;
};







template<class T,class... Units> struct cols<logv<T,Units...>>{
  using type=Cs<value_k>;
};
template<class T,class... Units> struct row_type<logv<T,Units...>>{
  using type=std::tuple<T>;
};
template<class T,class... Units> struct cols_w_unit<logv<T,Units...>>{
  using type=Cs<value_k,unit_k>;
};
template<class T,class... Units> struct row_type_w_unit<logv<T,Units...>>{
  using type=std::tuple<T,logv_units<Units...>>;
};

template<class T,class... Units,class...Is>
void insert_at(logv<T,Units...>& me,const Position<Is...>& , row_type_t<logv<T,Units...>>&& r)
{
  me.value()=std::get<T>(std::move(r));
}
template<class T,class... Units,class...Is>
void insert_at(logv<T,Units...>& me,const Position<Is...>& , row_type_w_unit_t<logv<T,Units...>>&& r)
{
  me.value()=std::get<T>(std::move(r));
  me.units()=std::get<logv_units<Units...>>(std::move(r));
}

template<class...> struct vec_frame;


template<> struct vec_frame<>
{
  template <class T>
  using to_vector=T;

  static inline constexpr auto index_size=0;

  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& ,ValueType&& e)
  {
    x=std::move(e);
  }
  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& p,row_type_t<ValueType>&& e)
  {
    ::insert_at(x,p,std::move(e));
  }

  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& p,row_type_w_unit_t<ValueType>&& e)
  {
    ::insert_at(x,p,std::move(e));
  }


  template <class Vector, class Position>
  static auto& get(Vector& x, const Position&)
  {
    return x;
  }
  template <class Vector, class Position>
  static auto& get(const Vector& x,const Position&)
  {
    return x;
  }
  template <class Vector, class Position>
  static bool next(const Vector& ,Position& ) {return false;}

};
template<class I0, class...I> struct vec_frame<I0,I...>{

  static inline constexpr auto index_size=sizeof... (I)+1;

  template <class T>
  using to_vector=std::vector<typename vec<I...>::template to_vector<T>>;

  template <class ValueType, class Positionx>
  static auto& get(std::vector<ValueType>& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }
  template <class ValueType, class Positionx>
  static auto& get(const std::vector<ValueType>& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }



  template <class ValueType, class Positionx, class elementType>
  static void insert_at(std::vector<ValueType>& x,const Positionx& p,elementType&& e)
  {
    if (x.size()==p[I0{}]())
      push_back(x,std::forward<elementType>(e));
    else
      vec_frame<I...>::insert_at(x.at(p[I0{}]()),p, std::forward<elementType>(e));
  }


  template <class ValueType>
  static std::vector<ValueType>& push_back(std::vector<ValueType>& x,ValueType&& e)
  {
    x.push_back(std::forward<ValueType>(e));
    return x;
  }
  template <class ValueType>
  static std::vector<ValueType> push_back(std::vector<ValueType>&& x,ValueType&& e)
  {
    x.push_back(std::forward<ValueType>(e));
    return x;
  }

  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,row_type_t<ValueType> >,std::vector<ValueType>&>{

    x.emplace_back(std::move(e));
    return x;
  }
  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>&& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,row_type_t< ValueType> >,std::vector<ValueType>>{

    x.emplace_back(std::move(e));
    return x;
  }

  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,row_type_w_unit_t< ValueType> >,std::vector<ValueType>&>{

    x.emplace_back(std::move(e));
    return x;
  }
  template <class ValueType,class... e_types>
  static auto push_back(std::vector<ValueType>&& x,std::tuple<e_types...>&& e)
      ->std::enable_if_t<std::is_same_v<std::tuple<e_types...>,row_type_w_unit_t< ValueType>  >,std::vector<ValueType>>{

    x.emplace_back(std::move(e));
    return x;
  }



  template <class ValueType, class elementType, typename=std::enable_if_t<!std::is_same_v<ValueType,elementType >,int>>
  static std::vector<ValueType>&  push_back(std::vector<ValueType>& x,elementType&& e)
  {
    auto v=ValueType{};
    return push_back(x,push_back(std::move(v),std::forward<elementType>(e)));
  }
  template <class ValueType, class elementType, typename=std::enable_if_t<!std::is_same_v<ValueType,elementType >,int>>
  static std::vector<ValueType>  push_back(std::vector<ValueType>&& x,elementType&& e)
  {
    auto v=ValueType{};
    return push_back(x,push_back(std::move(v),std::forward<elementType>(e)));
  }







  template <class ValueType, class Positionx>
  static auto& get(ValueType& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }
  template <class ValueType, class Positionx>
  static auto& get(const ValueType& x,const Positionx& p)
  {
    return vec<I...>::get(x.at(p[I0{}]()),p);
  }




  template <class Vector, class Position>
  static auto& get_I(I0,const Vector& x,const Position& )
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i,const Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }



  template <class Vector, class Position>
  static auto& get_I(I0,Vector& x,const Position& )
  {
    return x;
  }


  template <class In,class Vector, class Position>
  static auto& get_I(In i, Vector& x,const Position& p)
  {
    return vec<I...>::get_I(i,x.at(p[I0{}]()),p);
  }


  template <class Vector, class Position>
  static auto size(I0,const Vector& x,const Position& )
  {
    return x.size();
  }

  template <class In,class Vector, class Position>
  static auto size(In i,const Vector& x,const Position& p)->std::enable_if_t<is_in_pack<In,I...>(),std::size_t>
  {
    return vec<I...>::size(i,x.at(p[I0{}]()),p);
  }


  template <class Vector, class Position>
  static bool next(const Vector& x,Position& p)
  {
    if(vec<I...>::next(x.at(p[I0{}]()),p))
      return true;
    else {
      ++p[I0{}]();
      if (p[I0{}]()<x.size())
        return true;
      else {
        p[I0{}]()=0;
        return false;
      }
    }
  }
};


template<class Value_type,class... Xs,class...Is>
void insert_at(vector_field<vec<Xs...>,Value_type> &me,const Position<Is...>& p, Value_type&& r)
{
  vec_frame<Xs...>::insert_at(me.value(),p,std::move(r));
}
template<class Value_type,class... Xs,class...Is>
void insert_at(vector_field<vec<Xs...>,Value_type> &me,const Position<Is...>& p, element_type_t<vector_field<vec<Xs...>,Value_type>>&& r)
{
  vec_frame<Xs...>::insert_at(me.value(),p,std::move(r));
}

template<class Value_type,class... Xs,class...Is>
void insert_at(vector_field<vec<Xs...>,Value_type> &me,const Position<Is...>& p, row_type_t<vector_field<vec<Xs...>,Value_type>>&& r)
{
  vec_frame<Xs...>::insert_at(me.value(),p,std::move(r));
}



template<class Value_type,class... Xs,class...Is>
void insert_at(vector_field<vec<Xs...>,Value_type> &me,const Position<Is...>& p, row_type_w_unit_t<vector_field<vec<Xs...>,Value_type>>&& r)
{
  vec_frame<Xs...>::insert_at(me.value(),p,std::move(r));
}


template<class Value_type,class... Xs>
struct cols<vector_field<vec<Xs...>,Value_type>>
{
  using type=cols_t<Value_type>;
};

template<class Value_type,class... Xs>
struct row_type<vector_field<vec<Xs...>,Value_type>>
{
  using type=row_type_t<Value_type>;
};
template<class Value_type,class... Xs>
struct cols_w_unit<vector_field<vec<Xs...>,Value_type>>
{
  using type=cols_w_unit_t<Value_type>;
};
template<class Value_type,class... Xs>
struct row_type_w_unit<vector_field<vec<Xs...>,Value_type>>
{
  using type=row_type_w_unit_t<Value_type>;
};

template<class e_i, class Value_type>
struct cols<x_i<e_i,Value_type>>{
  using type=recursive_t<e_i,cols_t<Value_type>>;
};

template<class e_i, class Value_type>
struct cols_w_unit<x_i<e_i,Value_type>>{
  using type=recursive_t<e_i,cols_w_unit_t<Value_type>>;
};
template<class e_i, class Value_type>
struct row_type<x_i<e_i,Value_type>>{
  using type=row_type_t<Value_type>;
};
template<class e_i, class Value_type>
struct row_type_w_unit<x_i<e_i,Value_type>>{
  using type=row_type_w_unit_t<Value_type>;
};


// using element_type= element_type_t<Value_type> ;
template<class e_i, class Value_type>
struct element_type_impl<x_i<e_i,Value_type>>{
  using type=element_type_t<Value_type>;
};




template<class e_i, class Value_type,class...Is, class element_type, typename =std::enable_if_t<std::is_same_v<element_type, element_type_t<x_i<e_i,Value_type>>>>>
void insert_at(x_i<e_i,Value_type>&me,const Position<Is...>& p, x_i<e_i,element_type>&& r)
{
  insert_at(me(),p,std::move(r)());
}

template<class e_i, class Value_type,class...Is>
void insert_at(x_i<e_i,Value_type>&me,const Position<Is...>& p, row_type_t<x_i<e_i,Value_type>>&& r)
{
  insert_at(me(),p,std::move(r));
}
template<class e_i, class Value_type,class...Is, class ...Ts>
void insert_at(x_i<e_i,Value_type>&me,const Position<Is...>& p, std::tuple<Ts...>&& r)
{
  insert_at(me(),p,std::move(r));
}

template <class> struct myrow_type;
template <class> struct myrow_type_w_unit;
template <class V> using myrow_type_t=typename myrow_type<V>::type;
template <class V> using myrow_type_w_unit_t=typename myrow_type_w_unit<V>::type;


template<class> struct mycols;
template<class> struct mycols_w_unit;

template <class C> using mycols_t=typename mycols<C>::type;
template <class C> using mycols_w_unit_t=typename mycols_w_unit<C>::type;



template <class> struct cell_type;
template <class> struct cell_type_w_unit;
template <class V> using cell_type_t=typename cell_type<V>::type;
template <class V> using cell_type_w_unit_t=typename cell_type_w_unit<V>::type;


template<class...x_is>
struct cell_type<vector_space<x_is...>>{using type=pack_to_column_t<row_type_t<x_is>...>;};


template<class...x_is>
struct cell_type_w_unit<vector_space<x_is...>>{using type=pack_to_column_t<row_type_w_unit_t<x_is>...>;};




template<class...x_is>
struct row_type<vector_space<x_is...>>
{using type=pack_concatenation_t<
      std::tuple<std::variant<typename x_is::ei...>>,cell_type_t<vector_space<x_is...>>>;
};

template<class...x_is>
struct row_type_w_unit<vector_space<x_is...>>
{using type=pack_concatenation_t<
      std::tuple<std::variant<typename x_is::ei...>>,cell_type_w_unit_t<vector_space<x_is...>>>;
};



template <class...x_is,class... I, class... Is, std::size_t...In>
bool is_same(vector_space<x_is...> const& me,const Position<Is...>& p,const myrow_type_t<vector_space<x_is...>>& r, Cs<I...>,std::index_sequence<In...>)
{
  return ((std::get<In>(r)==at(me,p,I{}))&&...);

}

template<class dependent_type,class...Ds>
struct cols< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=pack_concatenation_t<

      recursive_t<primitive_k,cols_t<dependent_type>>,
      recursive_t<derivative_k,cols_t<derivative_type>>>;
};

template<class dependent_type,class...Ds>
struct cols_w_unit< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=pack_concatenation_t<

      recursive_t<primitive_k,cols_w_unit_t<dependent_type>>,
      recursive_t<derivative_k,cols_w_unit_t<derivative_type>>>;
};

template<class dependent_type,class...Ds>
struct row_type< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=decltype(std::tuple_cat(
      row_type_t<dependent_type>{},row_type_t<derivative_type>{}));
};

template<class dependent_type,class...Ds>
struct row_type_w_unit< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=decltype(std::tuple_cat(
      row_type_w_unit_t<dependent_type>{},row_type_w_unit_t<derivative_type>{}));
};


template <class...x_is,class... I, class... Is, std::size_t...In>
bool is_same(vector_space<x_is...> const& me,const Position<Is...>& p,const myrow_type_w_unit_t<vector_space<x_is...>>& r, Cs<I...>,std::index_sequence<In...>)
{
  return ((std::get<In>(r)==me(p,I{}))&&...);

}

template <class...x_is,class... Ind, class... Is, std::size_t...In>
bool is_same_index(vector_space<x_is...> const& ,Index<Ind...>,const Position<Is...>& p,const myrow_type_t<vector_space<x_is...>>& r, std::index_sequence<In...>)
{
  return ((std::get<In>(r)==p[Index<Ind...>{}]())&&...);

}
template <class...x_is,class... Ind, class... Is, std::size_t...In>
bool is_same_index(vector_space<x_is...> const& ,Index<Ind...>,const Position<Is...>& p,const myrow_type_w_unit_t<vector_space<x_is...>>& r, std::index_sequence<In...>)
{
  return ((std::get<In>(r)==p[Index<Ind...>{}]())&&...);

}
template <class...x_is,class I, class... Is>
bool is_same(vector_space<x_is...> const& me,I,const Position<Is...>& p,const myrow_type_t<vector_space<x_is...>>& r)
{
  using xi=std::decay_t<decltype(me[I{}])>;
  using colsi=cols_t<xi>;
  auto index_cols=pack_index_cs(colsi{},mycols_t<vector_space<x_is...>>{});
  return is_same(me,p,r,colsi{},index_cols);
}


template <class...x_is,class I, class... Is>
bool is_same(vector_space<x_is...> const& me,I,const Position<Is...>& p,const myrow_type_w_unit_t<vector_space<x_is...>>& r)
{
  using xi=std::decay_t<decltype(me[I{}])>;
  using colsi_w_unit=cols_w_unit_t<xi>;
  auto index_cols_w_unit=pack_index_cs(me,colsi_w_unit{},mycols_w_unit_t<vector_space<x_is...>>{});
  return is_same(me,p,r,colsi_w_unit{},index_cols_w_unit);
}




template <class...x_is,class... Ind, class... Is>
bool is_same(vector_space<x_is...> const& me,Index<Ind...>,const Position<Is...>& p,const myrow_type_t<vector_space<x_is...>>& r)
{
  auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<myrow_type_t<vector_space<x_is...>>,Cs<>>{});
  return is_same_index(me,Index<Ind...>{},p,r,index_rows);
}

template <class...x_is,class... Ind, class... Is>
bool is_same(vector_space<x_is...> const& me,Index<Ind...>,const Position<Is...>& p,const myrow_type_w_unit_t<vector_space<x_is...>>& r)
{
  auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<myrow_type_w_unit_t<vector_space<x_is...>>,Cs<>>{});
  return is_same_index(me,Index<Ind...>{},p,r,index_rows);
}


template <class I, class...x_is,class... Is, std::size_t...In>
void insert_at(Cs<I>,vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_t<vector_space<x_is...>>&& r,std::index_sequence<In...>)
{
  (insert_at(me[non_const<I>{}],p,std::make_tuple(std::get<In>(std::move(r))...)));

}

template <class I, class...x_is,class... Is, std::size_t...In>
void insert_at(Cs<I>,vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_w_unit_t<vector_space<x_is...>>&& r,std::index_sequence<In...>)
{
  (insert_at(me[non_const<I>{}],p,std::make_tuple(std::get<In>(std::move(r))...)));

}
template <class xi, class...x_is,class... Is>
void insert_at(Cs<xi>,vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_t<vector_space<x_is...>>&& r)
{
  using colsi=cols_t< xi>;
  auto index_cols=pack_index_cs(colsi{},mycols_t<vector_space<x_is...>>{});
  insert_at(Cs<typename xi::ei>{},me,p,std::move(r),index_cols);
}

template <class xi, class...x_is,class... Is>
void insert_at(Cs<xi>,vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_w_unit_t<vector_space<x_is...>>&& r)
{
  using colsi=cols_w_unit_t<xi>;
  auto index_cols=pack_index_cs(colsi{},mycols_w_unit_t<vector_space<x_is...>>{});
  //using test=typename colsi::test_colsi;
  //using test2=typename mycols_w_unit_t<vector_space>::test_mycols;
  insert_at(Cs<typename xi::ei>{},me,p,std::move(r),index_cols);
}

template <class...x_is,class... Is>
void insert_at(vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_t<vector_space<x_is...>>&& r)
{

  (insert_at(Cs<x_is>{},me,p,std::move(r)),...);
}
template <class...x_is,class... Is>
void insert_at(vector_space<x_is...> & me,const Position<Is...>& p, myrow_type_w_unit_t<vector_space<x_is...>>&& r)
{

  (insert_at(Cs<x_is>{},me,p,std::move(r)),...);
}
template <class...x_is,class... Is, class rowtype>
void insert_at(vector_space<x_is...> & me,const Position<Is...>& p, row_type_t<vector_space<x_is...>>&& r)
{
  auto v=p[Index<typename x_is::ei...>{}]();
  auto [r_ei, r_value]=distribute(Cs<std::variant<typename x_is::ei...>>{},
                                    transfer_t<cell_type_t<vector_space<x_is...>>,Cs<>>{},std::move(r));

  auto& rvalue=r_value;
  std::visit([&me,&rvalue,&p](auto ei){insert_at(me[ei],p,std::move(rvalue));},v);


}

template <class...x_is,class... Is
          >//,std::enable_if_t<!std::is_same_v<row_type_w_unit,row_type >,int>>
void insert_at(vector_space<x_is...> & me,const Position<Is...>& p, row_type_w_unit_t<vector_space<x_is...>>&& r)
{
  auto v=p[Index<typename x_is::ei...>{}]();
  auto [r_ei, r_value]=distribute(Cs<std::variant<typename x_is::ei...>>{},
                                    transfer_t<cell_type_w_unit_t<vector_space<x_is...>>,Cs<>>{},std::move(r));

  auto& rvalue=r_value;
  std::visit([&me,&rvalue,&p](auto ei){insert_at(me[non_const<decltype(ei)>{}],p,std::move(rvalue));},v);


}




template<class...x_is,class...Is>
auto next_pos(vector_space<x_is...>const & me,const Position<Is...>& p,const myrow_type_t<vector_space<x_is...>>& r)
{
  auto pres=(std::pair{p,false}|...|
               [&me,&r](auto p)
               {
                 if (p.second)
                 {
                   /// lower indexes set to zero after high index increases
                   p.first[Is{}]={};
                   return p;
                 }
                 else
                     if (is_same(me,Is{},p.first,r))
                   return p;
                 else
                 {
                   p.second=true;
                   ++p.first[Is{}];
                   return p;
                 }
               });

  return pres.first;
}


template<class...x_is,class...Is>
auto next_pos(vector_space<x_is...>const & me,const Position<Is...>& p,const myrow_type_w_unit_t<vector_space<x_is...>>& r)
{
  auto pres=(std::pair{p,false}|...|
               [&me,&r](auto p)
               {
                 if (p.second)
                 {
                   /// lower indexes set to zero after high index increases
                   p.first[Is{}]={};
                   return p;
                 }
                 else if (
                     is_same(me,Is{},p.first,r)
                     )
                   return p;
                 else
                 {
                   p.second=true;
                   ++p.first[Is{}];
                   return p;
                 }
               });

  return pres.first;
}


template<class...x_is,class...Is>
auto insert(vector_space<x_is...>& me,const Position<Is...>& p, myrow_type_t<vector_space<x_is...>>&& r)
{
  auto p1=next_pos(me,p,r);
  insert_at(me,p1,std::move(r));
  return p1;
}

template<class...x_is,class...Is>
auto insert(vector_space<x_is...>& me,const Position<Is...>& p, myrow_type_w_unit_t<vector_space<x_is...>>&& r)
{
  auto p1=next_pos(me,p,r);
  insert_at(me,p1,std::move(r));
  return p1;
}

template<class...x_is> struct mycols<vector_space<x_is...>>
{
  using type=pack_concatenation_t<cols_t<x_is>...>;
};

template<class...x_is> struct myrow_type<vector_space<x_is...>>
{
  using type=decltype (std::tuple_cat(row_type_t<x_is>{}...));
};

template<class...x_is> struct mycols_w_unit<vector_space<x_is...>>
{
  using type=pack_concatenation_t<cols_w_unit_t<x_is>...>;
};
template<class...x_is> struct myrow_type_w_unit<vector_space<x_is...>>
{
  using type=decltype (std::tuple_cat(row_type_w_unit_t<x_is>{}...));
};


template<class...x_is> struct cols<vector_space<x_is...>>
{
  using type=pack_unique_t<pack_concatenation_t<Cs<index_k>,pack_concatenation_t<cols_t< typename x_is::value_type>...>>>;
};

template<class...x_is> struct cols_w_unit<vector_space<x_is...>>
{
  using type=pack_unique_t<pack_concatenation_t<Cs<index_k>,pack_concatenation_t<cols_w_unit_t<typename x_is::value_type>...>>>;
};


template<class...x_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame(os,v,mycols_w_unit_t<vector_space<x_is...>>{});
}

template<class...x_is>
std::ostream& to_DataTable(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame(os,v,mycols_t<vector_space<x_is...>>{});
}




template<class...x_is>
std::istream& from_DataFrame(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>> cols_w_unit_t<x_is>{}),...);
  auto p=d_begin(v);
  auto r=myrow_type_w_unit_t<std::decay_t<decltype(v)>>{};
  if (ss)
  {
    std::getline(is, s);
    auto scopy=s;
    ss.clear();
    ss.str(s);
    ss>>r;
    insert_at(v,p,std::move(r));
  }
  while (ss)
  {
    std::getline(is, s);
    auto scopy=s;
    ss.clear();
    ss.str(s);
    ss>>r;
    p=insert(v,p,std::move(r));
  }

  return is;
}





template<class dependent_type,class... Ds,class...Is>
void insert_at( Derivative<dependent_type,vector_space<Ds...>>& me,
               const Position<Is...>& p, row_type_t<Derivative<dependent_type,vector_space<Ds...>>>&& r)
{
  using derivative_type=typename Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  auto [f_r,Df_r]=distribute(transfer_t<row_type_t<dependent_type>,Cs<>>{},
                                transfer_t<row_type_t<derivative_type>,Cs<>>{},
                                std::move(r));
  insert_at(me.f(),p,std::move(f_r));
  insert_at(me.Df(),p,std::move(Df_r));

}


template<class dependent_type,class... Ds,class...Is>
void insert_at( Derivative<dependent_type,vector_space<Ds...>>& me,
               const Position<Is...>& p, row_type_w_unit_t<Derivative<dependent_type,vector_space<Ds...>>>&& r)
{
  using derivative_type=typename Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  auto [f_r,Df_r]=distribute(transfer_t<row_type_w_unit_t<dependent_type>,Cs<>>{},
                                transfer_t<row_type_w_unit_t<derivative_type>,Cs<>>{},
                                std::move(r));
  insert_at(me.f(),p,std::move(f_r));
  insert_at(me.Df(),p,std::move(Df_r));

}



template<class TYPE,class myunit,class I>
auto& at(v<TYPE,myunit>& me,const I& , value_k){ return me.value();}

template<class TYPE,class myunit,class I>
auto& at(const v<TYPE,myunit>& me,const I& , value_k){ return me.value();}

template<class TYPE,class myunit,class I>
auto at(const v<TYPE,myunit>& me,const I& , unit_k) { return typename v<TYPE,myunit>::unit{};}



template<class T,class... Units,class I>
auto& at(logv<T,Units...>& me,const I& , value_k){ return me.value();}

template<class T,class... Units,class I>
auto& at(const logv<T,Units...>& me,const I& , value_k){ return me.value();}

template<class T,class... Units,class I>
auto& at(const logv<T,Units...>& me,const I& , unit_k) { return me.units();}

template<class T,class... Units,class I>
auto& at(logv<T,Units...>& me,const I& , unit_k) { return me.units();}







template<class... x_is,class J, class Position, class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
auto at(const vector_space<x_is...>&me,const Position& i, J) {return me[J{}](i);}

template<class... x_is, class...Is, class J,
          class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>()
                                       &&!std::is_same_v<J,index_k>,float>>
auto at(const vector_space<x_is...>&me,const Position<Is...>& i, J j)-> decltype(at(me,i[Index<typename x_is::ei...>{}](),j))
{
  return at(me,i[Index<typename x_is::ei...>{}](),j);
}

template<class... x_is, class...Is, class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>(),float>>
auto& at(const vector_space<x_is...>&,const Position<Is...>& i, index_k ) {
  return i[Index<typename x_is::ei...>{}]();
}

template<class... x_is,class J, class j_in, class Position,class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
auto at(const vector_space<x_is...>&me,const Position& i,recursive<J,j_in> ) ->decltype (at(me[J{}](i),i,j_in{}))
{return at(me[J{}](i),i,j_in{});}

template<class... x_is,class J>
auto  at(const vector_space<x_is...>&me,const std::variant<typename x_is::ei...>& ind,J )
{
  return std::visit([&me](auto& id)
          ->transfer_t<pack_unique_t<Cs<std::decay_t<decltype (me[typename x_is::ei{}]()[J{}])>...>>,std::variant<>>
      {
        return me[id]()[J{}];
      },ind);
}


template<class dependent_type,class... Ds,class Position, class ind
          , typename=std::enable_if_t<is_in_pack(ind{},cols_w_unit_t<dependent_type>{}),int>
          >
auto at(Derivative<dependent_type,vector_space<Ds...>>const&me,const Position& p,recursive<primitive_k,ind>)->decltype (at(me.f(),p,ind{}))
{return at(me.f(),p,ind{});}

template<class dependent_type,class... Ds,class Position, class ind
          //   ,typename=std::enable_if_t<is_in_pack(ind{},cols_w_unit_t<derivative_type>{}),bool>
          >
auto at(const Derivative<dependent_type,vector_space<Ds...>>&me,const Position& p,recursive<derivative_k,ind> ) ->decltype (at(me.Df(),p,ind{}))
{
  // using test1=typename ind::ind_has;
  //  using test2=typename cols_w_unit_t<derivative_type>::col_has;

  return at(me.Df(),p,ind{});
}


template <>
struct element_type_impl<std::mt19937_64>
{
  using type=std::mt19937_64;
};


template<class...x_is>
std::istream& from_DataTable(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>>cols_t<x_is>{}),...);
  auto p=d_begin(v);
  auto r=myrow_type_t<std::decay_t<decltype(v)>>{};
  if (ss)
  {
    std::getline(is, s);
    auto scopy=s;
    ss.clear();
    ss.str(s);
    ss>>r;
    insert_at(v,p,std::move(r));
  }
  while (ss)
  {
    std::getline(is, s);
    ss.clear();
    ss.str(s);
    ss>>r;
    p=insert(v,p,std::move(r));
  }

  return is;
}


template<class...x_is, class... col_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v, Cs<col_is...>)
{

  ((os<<col_is{}<<"\t"),...)<<"\n";
  auto p=d_begin(v);
  os<<std::setprecision(std::numeric_limits<double>::digits10+2);
  do{
    ((os<<at(v,p,col_is{})<<"\t"),...)<<"\n";
  }while( next(v,p));

  return os;
}


#endif // QM_DATA_FRAME_H
