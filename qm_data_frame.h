#ifndef QM_DATA_FRAME_H
#define QM_DATA_FRAME_H
#include "qm_tensor_model_evaluation.h"
#include <fstream>
template <class> struct row_type;
template <class V> using row_type_t=typename row_type<V>::type;


template<class> struct cols;


template <class C> using cols_t=typename cols<C>::type;

template<class> struct index_set;


template <class C> using index_set_t=typename index_set<C>::type;




template <class, class> struct index_of_col;

template <class C,class col> using index_of_col_t=typename index_of_col<C,col>::type;





template<class TYPE,class myunit,class...Is>
void insert_at(v<TYPE,myunit>& me,const Position<Is...>& , row_type_t<v<TYPE,myunit>>&& r)
{
  me.value()=std::move(r);
}

template<> struct cols<int>{
  using type=Cs<value_k>;
};

template<> struct index_set<int>{
  using type=Cs<Cs<>>;
};


template<> struct row_type<int>{
  using type=int;
};



template<class TYPE,class myunit> struct cols<v<TYPE,myunit>>{
  using type=Cs<value_k>;
};

template<class TYPE,class myunit> struct index_set<v<TYPE,myunit>>{
  using type=Cs<Cs<>>;
};


template<class TYPE,class myunit> struct index_of_col<v<TYPE,myunit>,value_k>{
  using type=Cs<>;
};



template<class TYPE,class myunit> struct row_type<v<TYPE,myunit>>{
  using type=TYPE;
};







template<class T,class... Units> struct cols<logv<T,Units...>>{
  using type=Cs<value_k,unit_k>;
};

template<class T,class... Units> struct index_set<logv<T,Units...>>{
  using type=Cs<Cs<>>;
};



template<class T,class... Units, class mycol>
struct index_of_col<logv<T,Units...>,mycol>{
  using type=Cs<>;
};



template<class T,class... Units> struct row_type<logv<T,Units...>>{
  using type=std::tuple<T,logv_units<Units...>>;
};

template<class T,class... Units,class...Is>
void insert_at(logv<T,Units...>& me,const Position<Is...>& , row_type_t<logv<T,Units...>>&& r)
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
    x=std::forward<ValueType>(e);
  }
  template <class ValueType, class Positionx>
  static void insert_at(ValueType& x,const Positionx& p,row_type_t<ValueType>&& e)
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
      push_back(x,p,std::forward<elementType>(e));
    else
      vec_frame<I...>::insert_at(x.at(p[I0{}]()),p, std::forward<elementType>(e));
  }

  template <class ValueType, class Position>
  static std::vector<ValueType>& push_back(std::vector<ValueType>& x,const Position& i,row_type_t<ValueType>&& e)
  {
    ValueType a;
    ::insert_at(a,i,std::move(e));
    x.push_back(std::move(a));
    return x;
  }
  template <class ValueType, class Position>
  static std::vector<ValueType>& push_back(std::vector<ValueType>& x,const Position& ,ValueType&& e)
  {
    x.push_back(std::forward<ValueType>(e));
    return x;
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
  vec_frame<Xs...>::insert_at(me.value(),p,std::forward<Value_type>(r));
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





template<class Value_type,class... Xs>
struct cols<vector_field<vec<Xs...>,Value_type>>
{
  using type=cols_t<Value_type>;
};

template<class Value_type,class... Xs>
struct index_set<vector_field<vec<Xs...>,Value_type>>
{
 // using test=typename index_set_t<Value_type>::value;
  using type=pack_append_t<Cs<Xs...>,index_set_t<Value_type>>;
};





template<class Value_type,class... Xs, class mycol>
struct index_of_col<vector_field<vec<Xs...>,Value_type>,mycol>{
  using type=pack_concatenation_t<Cs<Xs...>,index_of_col_t<Value_type,mycol>>;
};


template<class Value_type,class... Xs>
struct row_type<vector_field<vec<Xs...>,Value_type>>
{
  using type=row_type_t<Value_type>;
};

template<class e_i, class Value_type>
struct cols<x_i<e_i,Value_type>>{
  using type=recursive_t<e_i,cols_t<Value_type>>;
};





template<class e_i, class Value_type>
struct index_set<x_i<e_i,Value_type>>{
  using type=index_set_t<Value_type>;
};


template<class e_i, class Value_type, class mycol>
struct index_of_col<x_i<e_i,Value_type>,recursive<e_i,mycol>>{
  using type=index_of_col_t<Value_type,mycol>;
};
template<class e_i, class Value_type, class mycol>
struct index_of_col<x_i<e_i,Value_type>,mycol>{
  using type=index_of_col_t<Value_type,mycol>;
};


template<class e_i, class Value_type>
struct row_type<x_i<e_i,Value_type>>{
  using type=row_type_t<Value_type>;
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




template <class> struct cell_type;
template <class> struct cell_type_w_unit;
template <class V> using cell_type_t=typename cell_type<V>::type;
template <class V> using cell_type_w_unit_t=typename cell_type_w_unit<V>::type;


template<class...x_is>
struct cell_type<vector_space<x_is...>>{using type=pack_to_column_t<row_type_t<x_is>...>;};






template<class...x_is>
struct row_type<vector_space<x_is...>>
{
  using type=pack_concatenation_t<std::tuple<>,row_type_t<x_is>...>;
};



template <class...x_is,class... I, class... Is, std::size_t...In>
bool is_same(vector_space<x_is...> const& me,const Position<Is...>& p,const row_type_t<vector_space<x_is...>>& r, Cs<I...>,std::index_sequence<In...>)
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
struct index_set< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=pack_unique_t<pack_concatenation_t<
      index_set_t<dependent_type>,
      index_set_t<derivative_type>>>;
};





template<class dependent_type,class...Ds>
struct row_type< Derivative<dependent_type,vector_space<Ds...>>>{
  using derivative_type=typename  Derivative<dependent_type,vector_space<Ds...>>::derivative_type;
  using type=pack_concatenation_t<std::tuple<>,row_type_t<dependent_type>,row_type_t<derivative_type>>;
};



template <class...x_is,class I, class... Is>
bool is_same(vector_space<x_is...> const& me,I,const Position<Is...>& p,const row_type_t<vector_space<x_is...>>& r)
{
  using xi=std::decay_t<decltype(me[I{}])>;
  using colsi=cols_t<xi>;
  auto index_cols=pack_index_cs(colsi{},cols_t<vector_space<x_is...>>{});
  return is_same(me,p,r,colsi{},index_cols);
}


template <class...x_is,class... Ind, class... Is>
bool is_same(vector_space<x_is...> const& me,Index<Ind...>,const Position<Is...>& p,const row_type_t<vector_space<x_is...>>& r)
{
  auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<row_type_t<vector_space<x_is...>>,Cs<>>{});
  return is_same_index(me,Index<Ind...>{},p,r,index_rows);
}


template <class I, class...x_is,class... Is, std::size_t...In>
void insert_at(Cs<I>,vector_space<x_is...> & me,const Position<Is...>& p, row_type_t<vector_space<x_is...>>&& r,std::index_sequence<In...>)
{
  (insert_at(me[non_const<I>{}],p,std::make_tuple(std::get<In>(std::move(r))...)));

}



template <class...x_is,class Position, std::size_t ...Is>
void insert_at(vector_space<x_is...>& me,const Position& p, row_type_t<vector_space<x_is...>>&& r, std::index_sequence<Is...>)
{

  (insert_at(me[non_const<typename x_is::myId>{}],p,std::get<Is>(std::move(r))),...);
}


template <class...x_is,class Position>
void insert_at(vector_space<x_is...> & me,const Position& p, row_type_t<vector_space<x_is...>>&& r)
{

  return insert_at(me, p, std::move(r),std::index_sequence_for<x_is...>{});
}






template<class...x_is,class...Is>
auto next_pos(vector_space<x_is...>const & me,const Position<Is...>& p,const row_type_t<vector_space<x_is...>>& r)
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
auto insert(vector_space<x_is...>& me,const Position<Is...>& p, row_type_t<vector_space<x_is...>>&& r)
{
  auto p1=next_pos(me,p,r);
  insert_at(me,p1,std::move(r));
  return p1;
}




template<class...x_is> struct cols<vector_space<x_is...>>
{
  using type=pack_concatenation_t<cols_t<x_is>...>;
};


template<class> struct cols_q{using type=Cs<>;};

template<class ei, class value_type> struct cols_q<x_i<ei,value_type>>
{using type=cols_t<x_i<ei,value_type>>;};


template<class...x_is> struct cols<quimulun<x_is...>>
{
  using type=pack_concatenation_t<Cs<>,typename cols_q<x_is>::type...>;

//  using test=typename type::ty;
};


template<class...x_is> struct index_set<vector_space<x_is...>>
{
  using type=pack_unique_t<pack_concatenation_t<index_set_t<x_is>...>>;
};



template<class...x_is, class ei, class mycol> struct index_of_col<vector_space<x_is...>,recursive<ei,mycol>>
{
 // using test=typename ei::ei;
  using type=index_of_col_t<std::decay_t<decltype (std::declval<vector_space<x_is...>>()[ei{}])>,mycol>;
};






template<class...x_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame(os,v,cols_t<vector_space<x_is...>>{});
}

template<class...x_is>
std::ostream& to_DataFrame_title(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame_title(os,v,cols_t<vector_space<x_is...>>{});
}

template<class...x_is>
std::ostream& to_DataFrame_body(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame_body(os,v,cols_t<vector_space<x_is...>>{});
}




template<class...x_is>
std::istream& from_DataFrame(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>> cols_t<x_is>{}),...);
  auto p=begin(v);
  auto r=row_type_t<vector_space<x_is...>>{};
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



template<class TYPE,class myunit,class I>
auto& at(v<TYPE,myunit>& me,const I& , value_k){ return me.value();}

template<class TYPE,class myunit,class I>
auto& at(const v<TYPE,myunit>& me,const I& , value_k){ return me.value();}




template<class T,class... Units,class I>
auto& at(logv<T,Units...>& me,const I& , value_k){ return me.value();}

template<class T,class... Units,class I>
auto& at(const logv<T,Units...>& me,const I& , value_k){ return me.value();}

template<class T,class... Units,class I>
auto& at(const logv<T,Units...>& me,const I& , unit_k) { return me.units();}

template<class T,class... Units,class I>
auto& at(logv<T,Units...>& me,const I& , unit_k) { return me.units();}

template<class Value_type,class... Xs, class Position, class ind>

auto& at(const vector_field<vec<Xs...>,Value_type>& me, const Position& i, ind)
{
  return at(me(i),i,ind{});
}


template<class ei, class value_type, class Position, class ind>
auto& at(const x_i<ei,value_type>& me, const Position& i, ind){return at(me(),i,ind{});}

template<class ei, class value_type, class Position, class ind>
auto& at(const x_i<ei,value_type>& me, const Position& i, recursive<ei,ind>){return at(me(),i,ind{});}

template<class Position, class ind>
Nothing at(Nothing, const Position& , ind){return Nothing{};}




/*
template<class... x_is,class J, class Position, class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
auto at(const vector_space<x_is...>&me,const Position& i, J)->decltype (me[J{}](i))
{return me[J{}](i);}
*/




template<class... x_is,class J>
auto at(const quimulun<x_is...>&me,J) ->decltype (only_xi((me[J{}])))
{
  return only_xi((me[J{}]));
}

template<class... x_is,class J, class j_in, class Position>
auto at(const quimulun<x_is...>&me,const Position& i,recursive<J,j_in> ) ->decltype (at(at(me,J{}),i,j_in{}))
{
  return at(at(me,J{}),i,j_in{});
}



template<class... x_is,class J, class j_in, class Position>
auto at(const vector_space<x_is...>&me,const Position& i,recursive<J,j_in> ) ->decltype (at(me[J{}],i,j_in{}))
{
  return at(me[J{}],i,j_in{});
}


template<class... x_is,class... Data,class J, class j_in, class Position>
auto at(const Position& i,recursive<J,j_in> r,const vector_space<x_is...>&me, const Data&... d )->decltype ((at(me,i,r)||...||at(d,i,r)))
{return  (at(me,i,r)||...||at(d,i,r));}





template<class dependent_type,class... Ds,class Position, class ind
          , typename=std::enable_if_t<is_in_pack(ind{},cols_t<dependent_type>{}),int>
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











template<class...x_is, class... col_is>
std::ostream& to_DataFrame_title(std::ostream& os, const vector_space<x_is...>& , Cs<col_is...>)
{

  ((os<<col_is{}<<"\t"),...)<<"\n";

  return os;
}

template<class...x_is, class... col_is>
std::ostream& to_DataFrame_body(std::ostream& os, const vector_space<x_is...>& v, Cs<col_is...>)
{

  auto p=v.rec_begin();
  os<<std::setprecision(std::numeric_limits<double>::digits10+2);
  do{
    ((os<<at(v,p,col_is{})<<"\t"),...)<<"\n";
  }while( next(v,p));

  return os;
}
template <class col, class index>
constexpr bool is_this_col_an_index(col, index)
{
  return false;
}


template <class col, class ind, class index>
constexpr bool is_this_col_an_index(recursive<ind,col>, index)
{
  if constexpr (std::is_same_v<ind,index >) return true;
  else
  return is_this_col_an_index(col{},index{});
}


template <class col, class ind, class... index>
constexpr bool is_this_col_an_index(recursive<ind,col>, Cs<index...>)
{
  return (false||...||is_this_col_an_index(recursive<ind,col>{},index{}));
}


template <class index, class... col_is>
constexpr bool is_this_index_present(index,Cs<col_is...>)
{
  return (false||...||is_this_col_an_index(col_is{},index{}));
}



template <class, class,class> struct only_this_index;

template<class vectorspace, class... col_is, class... Xs>
struct only_this_index<vectorspace, Cs<col_is...>, Cs<Xs...>>
{

  using type= pack_concatenation_t<std::conditional_t<
      is_this_col_an_index(col_is{},Cs<Xs...>{})||
      std::is_same_v<index_of_col_t<vectorspace,col_is>,Cs<Xs...>>,
      Cs<col_is>,
      Cs<>>...>;
};


template< class... col_is, class... Xs>
constexpr bool has_all_the_indexes(Cs<col_is...>, Cs<Xs...>)
{
  if constexpr ((true&&...&&is_this_index_present(Xs{},Cs<col_is...>{})))
    return true;
  else
  {
    static_assert ((true&&...&&is_this_index_present(Xs{},Cs<col_is...>{})), "missing indexes");
    return false;
  }
}




template <class, class,class, class> struct only_this_index_new;




template<class vectorspace, class... col_is, class... col_data_is, class... Xs>
struct only_this_index_new<vectorspace, Cs<col_is...>,Cs<col_data_is...>, Cs<Xs...>>
{

  using type= pack_concatenation_t<
      std::conditional_t<
          is_this_col_an_index(col_data_is{},Cs<Xs...>{}),
          Cs<col_data_is>,
          Cs<>>...,
      std::conditional_t<
        is_this_col_an_index(col_is{},Cs<Xs...>{})||
          std::is_same_v<index_of_col_t<vectorspace,col_is>,Cs<Xs...>>,
      Cs<col_is>,
      Cs<>>...
      >;
  static_assert (has_all_the_indexes(type{},Cs<Xs...>{}),"missing indexes");

};





template <class, class,class> struct separate_by_index;

template<class vectorspace, class... col_is, class... vecXs>
struct separate_by_index<vectorspace, Cs<col_is...>, Cs<vecXs...>>
{
  using type= Cs<typename only_this_index<vectorspace,Cs<col_is...>,vecXs>::type...>;
};

template <class, class,class,class> struct separate_by_index_new;

template<class vectorspace, class... col_is, class... col_data_is, class... vecXs>
struct separate_by_index_new<vectorspace, Cs<col_is...>, Cs<col_data_is...>, Cs<vecXs...>>
{
  using type= Cs<typename only_this_index_new<vectorspace,Cs<col_is...>,Cs<col_data_is...>,vecXs>::type...>;
};





template<class...x_is, class... col_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v, Cs<col_is...>)
{

  ((os<<col_is{}<<"\t"),...)<<"\n";
  auto p=v.rec_begin();
  os<<std::setprecision(std::numeric_limits<double>::digits10+2);
  do{
    ((os<<at(v,p,col_is{})<<"\t"),...)<<"\n";
  }while( next(v,p));

  return os;
}


template<class...x_is, class... Xs, class... col_is, class... Header>
void to_DataFrame_body_one_index(const std::string& name, std::size_t nsample,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, Cs<Xs...>  indexes  [[maybe_unused]], Cs<col_is...> columnsets  [[maybe_unused]], const Header&...header)
{
  std::size_t nfactor=decimate_factor[sizeof... (Xs)];
  if (nsample%nfactor==0)
  {
    std::string filename=(name+...+(Xs::className.c_str()+std::string("_")))+".txt";
    auto myfilename=filename;
    std::ofstream f(filename.c_str(),std::ios_base::app);
  auto p=Position<Xs...>{};
  f<<std::setprecision(std::numeric_limits<double>::digits10+2);
  do{
    f<<nsample<<"\t";
    ((f<<header<<"\t"),...);
    ((f<<at(v,p,col_is{})<<"\t"),...);
    f<<"\n";
  }while( next(v,p));
  }
}

template<class...x_is, class... vexXs, class... Cscols, class... Header>
void to_DataFrame_body_all_index(const std::string& filename, std::size_t nsample,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, Cs<vexXs...> , Cs<Cscols...> , const Header&... header)
{
  (to_DataFrame_body_one_index(filename,nsample,decimate_factor,v,vexXs{},Cscols{},header...),...);

}


template<class...x_is, class... Header>
void to_DataFrame_body_index(const std::string& filename,std::size_t nsample,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, const Header& ...header)
{
  using indexes=index_set_t<vector_space<x_is...>>;
// using test=typename indexes::my_indexes;
  using columns_sets=typename separate_by_index<vector_space<x_is...>,cols_t<vector_space<x_is...>>,indexes>::type;
//  using test2=typename columns_sets::columns_sets;
 // using test3=typename cols_t<vector_space<x_is...>>::cols;
  to_DataFrame_body_all_index(filename,nsample,decimate_factor,v,indexes{},columns_sets{},header...);

}




template<class...x_is, class... Xs, class... col_is, class... Header>
void to_DataFrame_title_one_index(const std::string& name,  Cs<Xs...> indexes  [[maybe_unused]], Cs<col_is...> columnsets  [[maybe_unused]], const Header&...header)
{
  std::string filename=(name+...+(Xs::className.c_str()+std::string("_")))+".txt";
  std::ofstream f(filename.c_str());
  f<<"nsample"<<"\t";
  ((f<<header<<"\t"),...);
  ((f<<col_is{}<<"\t"),...);
      f<<"\n";

}

template<class...x_is, class... vexXs, class... Cscols, class... Header>
void to_DataFrame_title_all_index(const std::string& filename,  Cs<vexXs...> , Cs<Cscols...> , const Header&... header)
{
  (to_DataFrame_title_one_index(filename,vexXs{},Cscols{},header...),...);

}


template<class...x_is, class... Header>
void to_DataFrame_title_index(const std::string& filename, const vector_space<x_is...>& , const Header& ...header)
{
  using indexes=index_set_t<vector_space<x_is...>>;
  using columns_sets=typename separate_by_index<vector_space<x_is...>,cols_t<vector_space<x_is...>>,indexes>::type;
  to_DataFrame_title_all_index(filename,indexes{},columns_sets{},header...);

}
///--------------------------------------------------------
///
///
///


template<class...x_is, class... Xs, class... col_is, class... Data>
void to_DataFrame_body_one_index_new(const std::string& name, std::size_t nsample,double time,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, Cs<Xs...>  indexes  [[maybe_unused]], Cs<col_is...> columnsets  [[maybe_unused]], const Data&...data)
{
  std::size_t nfactor=decimate_factor[sizeof... (Xs)];
  if (nsample%nfactor==0)
  {
    std::string filename=(name+...+(Xs::className.c_str()+std::string("_")))+".txt";
    auto myfilename=filename;
    std::ofstream f(filename.c_str(),std::ios_base::app);
    auto p=Position<Xs...>{};
    f<<std::setprecision(std::numeric_limits<double>::digits10+2);
    do{
      f<<nsample<<"\t"<<time<<"\t";
      ((f<<at(p,col_is{},v,data...)<<"\t"),...);
      f<<"\n";
    }while( next(p,v,data...));
  }
}

template<class...x_is, class... vexXs, class... Cscols, class... Data>
void to_DataFrame_body_all_index_new(const std::string& filename, std::size_t nsample,double time,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, Cs<vexXs...> , Cs<Cscols...> , const Data&... data)
{
  (to_DataFrame_body_one_index_new(filename,nsample,time,decimate_factor,v,vexXs{},Cscols{},data...),...);

}


template<class...x_is, class... Data>
void to_DataFrame_body_index_new(const std::string& filename,std::size_t nsample,double time,const std::vector<std::size_t>& decimate_factor, const vector_space<x_is...>& v, const Data& ...data)
{
  using indexes=index_set_t<vector_space<x_is...>>;
   //using test=typename indexes::my_indexes;
  using cols_data_is=std::conditional_t<(sizeof... (Data)>0),pack_concatenation_t<cols_t<Data>...>,Cs<>>;
  using columns_sets=typename separate_by_index_new<vector_space<x_is...>,cols_t<vector_space<x_is...>>,cols_data_is,indexes>::type;
   // using test2=typename columns_sets::columns_sets;
  // using test3=typename cols_t<vector_space<x_is...>>::cols;
  to_DataFrame_body_all_index_new(filename,nsample,time,decimate_factor,v,indexes{},columns_sets{},data...);

}


template<class...x_is, class... Xs, class... col_is>
void to_DataFrame_title_one_index_new(const std::string& name,  Cs<Xs...> indexes  [[maybe_unused]], Cs<col_is...> columnsets  [[maybe_unused]])
{
  std::string filename=(name+...+(Xs::className.c_str()+std::string("_")))+".txt";
  std::ofstream f(filename.c_str());
  f<<"nsample"<<"\t"<<"time"<<"\t";
  ((f<<col_is{}<<"\t"),...);
  f<<"\n";

}


template<class...x_is, class... vexXs, class... Cscols>
void to_DataFrame_title_all_index_new(const std::string& filename,  Cs<vexXs...> , Cs<Cscols...> )
{
  (to_DataFrame_title_one_index_new(filename,vexXs{},Cscols{}),...);

}




template<class...x_is, class... Data>
void to_DataFrame_title_index_new(const std::string& filename, const vector_space<x_is...>& , const Data&...)
{
  using indexes=index_set_t<vector_space<x_is...>>;
  using cols_data_is=std::conditional_t<(sizeof... (Data)>0),pack_concatenation_t<Cs<>,cols_t<Data>...>,Cs<>>;


  using columns_sets=typename separate_by_index_new<vector_space<x_is...>,cols_t<vector_space<x_is...>>,cols_data_is,indexes>::type;
  to_DataFrame_title_all_index_new(filename,indexes{},columns_sets{});

}




#endif // QM_DATA_FRAME_H
