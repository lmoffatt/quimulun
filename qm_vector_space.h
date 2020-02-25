#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H
#include "qm_vector_basis.h"
#include <qm_pack_operations.h>
template <class...> struct vector_space;

template<> struct vector_space<>
{
  typedef Cs<> my_eis;
  typedef Cs<> myIds;
  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }

  vector_space()=default;
  static constexpr auto size() {return 0;}

  template<class xi>
  auto operator + (xi&& x)&&
  {
    if constexpr (is_this_template_class_v<x_i,xi>||is_this_template_class_v<x_i_view_const,xi>||is_this_template_class_v<x_i_view_non_const,xi>)
       return vector_space<std::decay_t<xi>>{std::forward<xi>(x)};
    else
      return std::forward<xi>(x);
  }
  auto operator + (Nothing)&&
  {
    return *this;
  }




};






template<class ...Is, class x1, class x2>
auto operator&&(std::pair<Position<Is...>,bool>&& p, const  std::pair<const x1&, const x2&>& x)
    ->std::enable_if_t<is_in_pack<typename x1::ei,Is...>()
                            &&!std::is_same_v<x1,x2>,std::pair<Position<Is...>,bool>>
{
  if (p.second) return p;
  else if (x.first(p.first)==x.second()) return p;
  else
  {
    p.second=true;
    p.first.inc(typename x1::ei{});
    return p;
  }
}

template<class ...Is, class x1, class x2>
auto operator&&(std::pair<Position<Is...>,bool>&& p, const std::pair<const x1&, const x2&>& )
    ->std::enable_if_t<!is_in_pack<typename x1::ei,Is...>()
                            ||std::is_same_v<x1,x2>
                        ,std::pair<Position<Is...>,bool>>
{
  return p;
}

struct index_k{  constexpr static auto  className=my_static_string("index");};



template<class...x_is> struct vector_space: private x_is...
{
  using  self_type=vector_space;
  using x_is::operator[]...;

  typedef Cs<typename x_is::ei...> my_eis;
  typedef Cs<typename x_is::myId...> myIds;
  typedef Cs<x_is...> myx_is;
  using myIndex=Index<typename x_is::ei...>;




 // using print_cols=typename cols::print;




  //using print_rowtype=typename row_type::print;

/*
  template<class Is>
  static constexpr auto index_to_myCol_number(){
    using xi=std::decay_t<decltype(std::declval<vector_space>()[Is{}])>;
    using coli=typename xi::cols;
    return pack_index_cs(coli{},myCols{});
  }
  template<class Is>
  static constexpr auto index_to_myCol_number_w_unit(){
    using xi=std::decay_t<decltype(std::declval<vector_space>()[Is{}])>;
    using coli_w_unit=typename xi::cols_w_unit;
    return pack_index_cs(coli_w_unit{},myCols_w_unit{});
  }

*/
  //using gsdg=typename cols::ger;












  





  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){
    return vector_space<std::decay_t<decltype ((*this)[typename x_is::ei{}](u,d))>...>
        {(*this)[typename x_is::ei{}](u,d)...};

  }



  template<class Is>
  using get_xi_t=std::decay_t<decltype (std::declval<vector_space&>()[Is{}])>;

  template<class Position,class Is>
  using get_element_xi_t=std::decay_t<decltype (std::declval<vector_space&>()(std::declval<Position>(),Is{}))>;

  //friend constexpr myrow_type row_vector(const vector_space&) {return  myrow_type{};}

  //friend constexpr myrow_type_w_unit row_vector_w_unit(const vector_space&) {return  myrow_type_w_unit{};}








  template<class... I, class Is,class T>
  static bool next_impl(Is,Position<I...>& p,const T& x)
  {
    if (p[Is{}]()+1<x().size(Is{},p)){
      ++p[Is{}]();
      return true;}
    else{
      p[Is{}]()={};
      return false;}

  }

  /*
  template<class... I, class Is>
  static Nothing next_impl(Is,Position<I...>& ,Nothing)
  {
    return Nothing{};

  }

*/

  template<class... I, class Is, typename =std::enable_if_t< is_in_pack<Is,I...>()>>
  auto next(Is,Position<I...>& p)const
  {
    return next_impl(Is{},p,(*this)[Is{}]);
  }

  template<class... I, class Is, class... Data, typename =std::enable_if_t< is_in_pack<Is,I...>()>>
  friend auto nexts(Is,Position<I...>& p, const vector_space& me , const Data&...d)
  {
    auto& x=(me[Is{}]||...||d[Is{}]);
    return next_impl(Is{},p,x);
  }


  /*
  template<class... I, class... eis, typename =std::enable_if_t<is_in_pack<Index<eis...>,I...>(),int>>
  bool next(Index<eis...>,Position<I...>& p)const
  {
    return next_variant(p[Index<eis...>{}]());
  }
*/
  template<class ...Is, class... Data>
  friend auto next(Position<Is...>& p,const vector_space& me, const Data&...d)
  {
    return ([&me,&p,&d...](auto b){
             if (b.first)
               return b;
             else if (nexts(Is{},p,me,d...))
               return std::pair{true,true};
             else
               return std::pair{false,false};
           }
            |...|std::pair(false,false)).second;
  }





  template<class ...Is>
  friend auto next(const vector_space& me, Position<Is...>& p)
  {
    return ([&me,&p](auto b){
                 if (b.first)
                   return b;
                 else if (me.next(Is{},p))
                   return std::pair{true,true};
                 else
                   return std::pair{false,false};
               }
               |...|std::pair(false,false)).second;
  }




  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }

  auto operator + (Nothing)&&
  {
    return std::move(*this);
  }

  template<class... T>
  auto operator + (Error<T...>)&&
  {
    return std::move(*this);
  }



  template<class Value_2>
  auto operator/(const Value_2& a)const
  {
    return vector_space<decltype (x_is{}/a)...>{(*this)[typename x_is::ei{}]/a...};
  }

  template<class x_j,
            typename=std::enable_if_t<!is_this_template_class<vector_space,x_j>::value,int>>
  auto operator * (const x_j&  two)const
  {
    return (vector_space<>{}|...|((*this)[typename x_is::ei{}]*two));
  }

  friend constexpr auto begin(const vector_space& ){ return vector_space::begin();}

  static constexpr auto begin() {return (x_is::begin()&&...);}
  static constexpr auto rec_begin() {return (x_is::rec_begin()&&...);}

  constexpr bool next(Position<>& )const {return false;}

  template<class...Is>
  auto& operator()(const Position<Is...>& ){ return *this;}

  template<class...Is>
  auto& operator()(const Position<Is...>& )const { return *this;}




  static constexpr auto size() {return sizeof... (x_is);}

  explicit vector_space(x_is&&...xs):x_is{std::move(xs)}...{}
  explicit vector_space(x_is const&...xs):x_is{xs}...{}


  template<class... Ids, class...values_t,typename =std::enable_if_t<(sizeof... (Ids) > 0),int>>
  vector_space(Cs<Ids...>,values_t&&...v):x_is{x_i{Ids{},std::forward<values_t>(v)}}...{}

  template<class...values_t>
  vector_space(Cs<>,values_t&&...v):x_is{std::forward<values_t>(v)}...{}


  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space(const vector_space<x_is_1...>&x,const vector_space<x_is_2...>& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{x[typename x_is_1::ei{}]}...,x_not_common{y[typename x_not_common::ei{}]}...{
    (((*this)[non_const<typename x_is_common::ei>{}]+=y[typename x_is_common::ei{}]),...);
  }
  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space( vector_space<x_is_1...>&&x, vector_space<x_is_2...>&& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{std::move(x)[typename x_is_1::ei{}]}...,x_not_common{std::move(y)[typename x_not_common::ei{}]}...{
    (((*this)[non_const<typename x_is_common::ei>{}]+=y[typename x_is_common::ei{}]),...);
  }

  vector_space()=default;




  inline friend
      bool operator==(const vector_space& me, const vector_space& ti)
  {
    return (true&&...&&(me[typename x_is::ei{}]==ti[typename x_is::ei{}]));
  }


  auto operator+=(const vector_space& other)
  {
    (((*this)[typename x_is::ei{}]+=other[typename x_is::ei{}]),...);
    return *this;
  }


  template<class ax_i,
            typename=std::enable_if_t<is_this_template_class<x_i,ax_i>::value,int>>
  auto operator + (ax_i&& x)&&
  {
    if constexpr (is_in_pack<ax_i,x_is...>())
    {
      (*this)[typename ax_i::ei{}]=std::forward<ax_i>(x);
      return *this;
    }
    else
    {
      return    vector_space<x_is...,std::decay_t<ax_i>>{std::move(*this)[typename x_is::ei{}]...,std::forward<ax_i>(x)};
    }
  }




  auto operator-()&&
  {
    (((*this)[non_const<typename x_is::ei>{}]=-(*this)[typename x_is::ei{}]),...);
    return std::move(*this);
  }
  auto operator-()const&
  {
    auto out=*this;
    ((out[typename x_is::ei{}]=-out[typename x_is::ei{}]),...);
    return out;
  }








  friend std::ostream& operator<<(std::ostream& os, const vector_space& me)
  {
    os<<std::setprecision(std::numeric_limits<double>::digits10+3);
    ((os<<typename x_is::myId{}<<" ="<<me[typename x_is::myId{}]<<"  ")&&...);
    return os;
  }
  friend std::istream& operator>>(std::istream& is,  vector_space& me)
  {
    ((is>>typename x_is::myId{}>>my_static_string(" =")>>me[non_const<typename x_is::myId>{}])&&...);
    return is;
  }



};

template<class... I, class Is,class...vectorfield>
auto get_size_from(Is,Position<I...>&p, const vectorfield&...me)
{
  return ([&me,&p](auto b){
           if (b.first)
             return b;
           else if (me.size(Is{},p)>0)
             return std::pair{true,me.size(Is{},p)};
           else
             return std::pair{false,0ul};
         }
          |...|std::pair(false,0ul)).second;
}



template<class... I, class Is,class...value_type, typename =std::enable_if_t<is_in_pack<Is,I...>(),int>>
bool next(Is,Position<I...>& p,const value_type&...me)
{
  if (p[Is{}]()+1<get_size_from(Is{},p,me...)){
    ++p[Is{}]();
    return true;}
  else{
    p[Is{}]()={};
    return false;}

}



/*
template<class ...Is,class...value_types>
 auto next(Position<Is...>& p,const value_types&...me)
{
  return ([&me...,&p](auto b){
           if (b.first)
             return b;
           else if (next(Is{},p,me...))
             return std::pair{true,true};
           else
             return std::pair{false,false};
         }
          |...|std::pair(false,false)).second;
}
*/



template<class... Ids, class...values_t>
vector_space(Cs<Ids...>,values_t&&...v)->vector_space<x_i<Ids,std::decay_t<values_t>>...>;

template<class...xis>
vector_space(xis&&...v)->vector_space<std::decay_t<xis>...>;

template <class... x_is>
vector_space(x_is const&...xs)->vector_space<x_is...>;








template<class I, class... Is>
std::ostream& operator<<(std::ostream& os, const std::variant<I,Is...>& v) { std::visit([&os](auto& e){os<<e;},v); return os;}

template<class... Is>
std::istream& operator>>(std::istream& is,  std::variant<Is...>& v) {

  std::string s;
  read_between_separators(is, s,'\t');
  if(!(is))
    return is;
  auto scopy=s;
  bool res=false;
  (v|...|[s,&res](auto& v )->auto&
   {auto  i=Is{}; std::stringstream ss(s);
if (ss>>i)
    {v=i; res=true;}
return v;});
  if (!res)
    is.setstate(std::ios::failbit);
  return is;

}









template<class x_i>
auto operator | (x_i&& one, Nothing)
{
  return vector_space<x_i>{std::move(one)};
}
template<class x_i>
auto operator | (Nothing,x_i&& one)
{
  return vector_space<x_i>{std::move(one)};
}

auto operator | (Nothing,Nothing)
{
  return Nothing{};
}

template<class... x_is>
auto operator | (vector_space<x_is...>&& one, Nothing)
{
  return std::move(one);
}


template<class e_i, class Value_type_i, class e_j, class Value_type_j>
auto operator | (x_i<e_i,Value_type_i>&& one, x_i<e_j,Value_type_j> && two)
{
  if constexpr (std::is_same_v<e_i,e_j >)
    return vector_space<x_i<e_i,Value_type_i>>(std::move(one)+std::move(two));
  else
    return vector_space<x_i<e_i,Value_type_i>,x_i<e_j,Value_type_j>>{std::move(one),std::move(two)};
}

template<class... x_is, class x_j>
auto operator | (vector_space<x_is...>&& one, x_j && two)
{
  if constexpr (is_in_pack<x_j,x_is... >())
  {
    one[typename x_j::ei{}]+=std::move(two);
    return std::move(one);
  }
  else
    return vector_space<x_is...,x_j>{std::move(one)[typename x_is::ei{}]...,std::move(two)};
}



template<class... x_is, class x_j>
auto operator * (const x_j&  two,const vector_space<x_is...>& one)
{
  return (vector_space<>{}|...|(two*one[typename x_is::ei{}]));
}


template<class... x_is, class... x_js>
auto operator * (const vector_space<x_is...>& one,const vector_space<x_js...>&  two)
{
  return ((one*two[typename x_js::ei{}])+...);
}

template<class...x_is,class ...x_is2, class... x2_not_x1>
auto difference_impl(const vector_space<x_is...>&me, const vector_space<x_is2...>& other, Cs<x2_not_x1...>)
{
  return vector_space((me[typename x_is::ei{}]-other[typename x_is::ei{}])...,-other[x2_not_x1{}]...);
}
template<class...x_is,class ...x_is2, class... x2_not_x1>
auto difference_impl( vector_space<x_is...>&&me,  vector_space<x_is2...>&& other, Cs<x2_not_x1...>)
{
  return vector_space((std::move(me)[typename x_is::ei{}]-std::move(other)[typename x_is::ei{}])...,-std::move(other)[x2_not_x1{}]...);
}



template<class...x_is,class ...x_is2>
auto operator -(const vector_space<x_is...>&me, const vector_space<x_is2...>& other)
{
  typedef pack_difference_t<Cs<typename x_is2::ei...>,Cs<typename x_is::ei...>> x2_not_1;
  return difference_impl(me,other,x2_not_1{});
}

template<class...x_is,class ...x_is2>
auto operator -( vector_space<x_is...>&&me,  vector_space<x_is2...>&& other)
{
  typedef pack_difference_t<Cs<typename x_is2::ei...>,Cs<typename x_is::ei...>> x2_not_1;
  return difference_impl(std::move(me),std::move(other),x2_not_1{});
}

template<class...x_is,class ...x_is2, class... x2_not_x1>
auto addition_impl(const vector_space<x_is...>&me, const vector_space<x_is2...>& other, Cs<x2_not_x1...>)
{
  return vector_space((me[typename x_is::ei{}]+other[typename x_is::ei{}])...,other[x2_not_x1{}]...);
}
template<class...x_is,class ...x_is2, class... x2_not_x1>
auto addition_impl( vector_space<x_is...>&&me,  vector_space<x_is2...>&& other, Cs<x2_not_x1...>)
{
  return vector_space{(std::move(me)[typename x_is::ei{}]+std::move(other)[typename x_is::ei{}])...,std::move(other)[x2_not_x1{}]...};
}



template<class...x_is,class ...x_is2>
auto operator +(const vector_space<x_is...>&me, const vector_space<x_is2...>& other)
{
  typedef pack_difference_t<Cs<typename x_is2::ei...>,Cs<typename x_is::ei...>> x2_not_1;
  return addition_impl(me,other,x2_not_1{});
}

template<class x_i0,class...x_is,class ...x_is2>
auto operator +( vector_space<x_i0,x_is...>&&me,  vector_space<x_is2...>&& other)
{
  typedef pack_difference_t<Cs<typename x_is2::ei...>,Cs<typename x_i0::ei,typename x_is::ei...>> x2_not_1;
  return addition_impl(std::move(me),std::move(other),x2_not_1{});
}







template<bool Complete,class... Ds, class Id2, class Value_type>
Is_Complete<Complete,vector_space<Ds...,x_i<Id2,Value_type>>>
operator &&
    (Is_Complete<Complete,vector_space<Ds...>>&& d, x_i<Id2,Value_type>&& d2)
{
 // using test=typename Id2::x_i_test_Id;
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,x_i<Id2,Value_type>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}



template<bool Complete,class... Ds, class... Ds2>
Is_Complete<Complete,vector_space<Ds...,Ds2...>>
operator &&
    (Is_Complete<Complete,vector_space<Ds...>>&& d, vector_space<Ds2...>&& d2)
{
  //using test=typename Cs<Ds2...>::x_i_test_Id;
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,Ds2...>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)[typename Ds2::myId{}]...));
}



template<bool Complete,class... Ds>
Is_Complete<false,vector_space<Ds...>> operator &&(Is_Complete<Complete,vector_space<Ds...>>&& d, Nothing)
{
  return Is_Complete(std::bool_constant<false>{},std::move(d.value));
}



template <class Vector, class X, class... X1, class Position,class Id, class Value, class... Datum1>
void fill_vector_field(Vector&v,Position& p,vec<X,X1...> ,const x_i<Id,Value>& one,const Datum1&... two)
{
  if constexpr (is_in_pack(X{},get_Field_Indexes_t<Value>{}))
  {
    auto n=one().size(X{},p);
    v.resize(n);
    auto& i=p[X{}]();
    for (i=0; i<n; ++i)
      fill_vector_field(v[i],p,vec<X1...>{},one,two...);
  }
  else fill_vector_field(v,p,vec<X,X1...>{},two...);
}





template<class ...xi>
using Matrix_t=decltype (vector_space<xi...>{}*vector_space<xi...>{});





#endif // QM_VECTOR_SPACE_H
