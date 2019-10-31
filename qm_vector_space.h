#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H
#include "qm_vector_basis.h"

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

  template<class x_i>
  auto operator + (x_i&& x)&&
  {
    return vector_space<std::decay_t<x_i>>{std::forward<x_i>(x)};
  }
  template<class ...x_is2>
  auto operator +(const vector_space<x_is2...>& other)const &
  {
    return other;
  }
  template<class ...x_is2>
  auto operator +( vector_space<x_is2...>&& other)&&
  {
    return std::move(other);
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
  using x_is::operator[]...;
  typedef Cs<typename x_is::ei...> my_eis;
  typedef Cs<typename x_is::myId...> myIds;

  using myIndex=Index<typename x_is::ei...>;


  using myCols=pack_concatenation_t<typename x_is::cols...>;
  using myrow_type=decltype (std::tuple_cat(typename x_is::row_type{}...));

  using myCols_w_unit=pack_concatenation_t<typename x_is::cols_w_unit...>;
  using myrow_type_w_unit=decltype (std::tuple_cat(typename x_is::row_type_w_unit{}...));


  using cols=pack_unique_t<pack_concatenation_t<Cs<index_k>,pack_concatenation_t<typename x_is::value_type::cols...>>>;

  using cols_w_unit=pack_unique_t<pack_concatenation_t<Cs<index_k>,pack_concatenation_t<typename x_is::value_type::cols_w_unit...>>>;


 // using print_cols=typename cols::print;

  using cell_type=pack_to_column_t<typename x_is::value_type::row_type...>;
  using cell_type_w_unit=pack_to_column_t<typename x_is::value_type::row_type_w_unit...>;


  using row_type=pack_concatenation_t<
std::tuple<std::variant<typename x_is::ei...>>,cell_type>;

  using row_type_w_unit=pack_concatenation_t<
      std::tuple<std::variant<typename x_is::ei...>>,cell_type_w_unit>;

  //using print_rowtype=typename row_type::print;


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


  //using gsdg=typename cols::ger;

  template<class J, class Position, class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
  auto operator()(const Position& i, J)const {return (*this)[J{}](i);}

  template< class...Is, class J,
            class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>()
                                     &&!std::is_same_v<J,index_k>,float>>
  auto operator()(const Position<Is...>& i, J j)const-> decltype((*this)(i[Index<typename x_is::ei...>{}](),j))
  {
    return (*this)(i[Index<typename x_is::ei...>{}](),j);
  }

  template< class...Is, class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>(),float>>
  auto& operator()(const Position<Is...>& i, index_k )const {
    return i[Index<typename x_is::ei...>{}]();
  }



  template<class J, class j_in, class Position,class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
  auto operator()(const Position& i,recursive<J,j_in> )const ->decltype ((*this)[J{}](i)(i,j_in{}))
  {return (*this)[J{}](i)(i,j_in{});}

  
  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){
    return vector_space<std::decay_t<decltype ((*this)[typename x_is::ei{}](u,d))>...>
        {(*this)[typename x_is::ei{}](u,d)...};

  }


  template<class J>
  auto  operator()(const std::variant<typename x_is::ei...>& ind,J )const
  {
    return std::visit([this](auto& id)->
        transfer_t<pack_unique_t<Cs<std::decay_t<decltype ((*this)[typename x_is::ei{}]()[J{}])>...>>,std::variant<>>
                      {
      return (*this)[id]()[J{}];
    },ind);
  }






  template<class Is>
  using get_xi_t=std::decay_t<decltype (std::declval<vector_space&>()[Is{}])>;

  template<class Position,class Is>
  using get_element_xi_t=std::decay_t<decltype (std::declval<vector_space&>()(std::declval<Position>(),Is{}))>;

  friend constexpr myrow_type row_vector(const vector_space&) {return  myrow_type{};}

  friend constexpr myrow_type_w_unit row_vector_w_unit(const vector_space&) {return  myrow_type_w_unit{};}

  template <class... I, class... Is, std::size_t...In>
  bool is_same(const Position<Is...>& p,const myrow_type& r, Cs<I...>,std::index_sequence<In...>)const
  {
    return ((std::get<In>(r)==(*this)(p,I{}))&&...);

  }

  template <class... I, class... Is, std::size_t...In>
  bool is_same(const Position<Is...>& p,const myrow_type_w_unit& r, Cs<I...>,std::index_sequence<In...>)const
  {
    return ((std::get<In>(r)==(*this)(p,I{}))&&...);

  }

  template <class... Ind, class... Is, std::size_t...In>
  static bool is_same_index(Index<Ind...>,const Position<Is...>& p,const myrow_type& r, std::index_sequence<In...>)
  {
    return ((std::get<In>(r)==p[Index<Ind...>{}]())&&...);

  }
  template <class... Ind, class... Is, std::size_t...In>
  static bool is_same_index(Index<Ind...>,const Position<Is...>& p,const myrow_type_w_unit& r, std::index_sequence<In...>)
  {
    return ((std::get<In>(r)==p[Index<Ind...>{}]())&&...);

  }


  template <class I, class... Is>
  bool is_same(I,const Position<Is...>& p,const myrow_type& r)const
  {
    using xi=std::decay_t<decltype(std::declval<vector_space>()[I{}])>;
    using colsi=typename xi::cols;
    auto index_cols=pack_index_cs(colsi{},myCols{});
    return is_same(p,r,colsi{},index_cols);
  }

  template <class I, class... Is>
  bool is_same(I,const Position<Is...>& p,const myrow_type_w_unit& r)const
  {
    using xi=std::decay_t<decltype(std::declval<vector_space>()[I{}])>;
    using colsi_w_unit=typename xi::cols_w_unit;
    auto index_cols_w_unit=pack_index_cs(colsi_w_unit{},myCols_w_unit{});
    return is_same(p,r,colsi_w_unit{},index_cols_w_unit);
  }

  template <class... Ind, class... Is>
  bool is_same(Index<Ind...>,const Position<Is...>& p,const myrow_type& r)const
  {
    auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<myrow_type,Cs<>>{});
    return is_same_index(Index<Ind...>{},p,r,index_rows);
  }

  template <class... Ind, class... Is>
  bool is_same(Index<Ind...>,const Position<Is...>& p,const myrow_type_w_unit& r)const
  {
    auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<myrow_type_w_unit,Cs<>>{});
    return is_same_index(Index<Ind...>{},p,r,index_rows);
  }


  template <class I, class... Is, std::size_t...In>
  void insert_at(const Position<Is...>& p, myrow_type&& r,std::index_sequence<In...>)
  {
    ((*this)[I{}].insert_at(p,std::make_tuple(std::get<In>(std::move(r))...)));

  }
  template <class I, class... Is, std::size_t...In>
  void insert_at(const Position<Is...>& p, myrow_type_w_unit&& r,std::index_sequence<In...>)
  {
    ((*this)[I{}].insert_at(p,std::make_tuple(std::get<In>(std::move(r))...)));

  }


  template <class xi, class... Is>
  void insert_at(const Position<Is...>& p, myrow_type&& r)
  {
    using colsi=typename xi::cols;
    auto index_cols=pack_index_cs(colsi{},myCols{});
    insert_at<typename xi::ei>(p,std::move(r),index_cols);
  }

  template <class xi, class... Is>
  void insert_at(const Position<Is...>& p, myrow_type_w_unit&& r)
  {
    using colsi=typename xi::cols_w_unit;
    auto index_cols=pack_index_cs(colsi{},myCols_w_unit{});
    insert_at<typename xi::ei>(p,std::move(r),index_cols);
  }

  template <class... Is>
  void insert_at(const Position<Is...>& p, myrow_type&& r)
  {

    (insert_at<x_is>(p,std::move(r)),...);
  }

  template <class... Is>
  void insert_at(const Position<Is...>& p, myrow_type_w_unit&& r)
  {

    (insert_at<x_is>(p,std::move(r)),...);
  }

  template <class... Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {
    auto v=p[Index<typename x_is::ei...>{}]();
    auto [r_ei, r_value]=distribute(Cs<std::variant<typename x_is::ei...>>{},
                                      transfer_t<cell_type,Cs<>>{},std::move(r));

    auto& rvalue=r_value;
    std::visit([this,&rvalue,&p](auto ei){(*this)[ei].insert_at(p,std::move(rvalue));},v);


  }

  template <class... Is
            >//,std::enable_if_t<!std::is_same_v<row_type_w_unit,row_type >,int>>
  void insert_at(const Position<Is...>& p, row_type_w_unit&& r)
  {
    auto v=p[Index<typename x_is::ei...>{}]();
    auto [r_ei, r_value]=distribute(Cs<std::variant<typename x_is::ei...>>{},
                                      transfer_t<cell_type_w_unit,Cs<>>{},std::move(r));

    auto& rvalue=r_value;
    std::visit([this,&rvalue,&p](auto ei){(*this)[ei].insert_at(p,std::move(rvalue));},v);


  }


  template<class...Is>
  auto next_pos(const Position<Is...>& p,const myrow_type& r)const
  {
     auto pres=(std::pair{p,false}|...|
                 [this,&r](auto p)
                 {
                   if (p.second)
                   {
                     /// lower indexes set to zero after high index increases
                     p.first[Is{}]={};
                     return p;
                   }
                   else
                       if (this->is_same(Is{},p.first,r)) return p;
                   else
                   {
                     p.second=true;
                     ++p.first[Is{}];
                     return p;
                   }
                 });

    return pres.first;
  }


  template<class...Is>
  auto next_pos(const Position<Is...>& p,const myrow_type_w_unit& r)const
  {
    auto pres=(std::pair{p,false}|...|
                 [this,&r](auto p)
                 {
                   if (p.second)
                   {
                     /// lower indexes set to zero after high index increases
                     p.first[Is{}]={};
                     return p;
                   }
                   else if (
                       this->is_same(Is{},p.first,r)
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


  template<class...Is>
  auto insert(const Position<Is...>& p, myrow_type&& r)
  {
    auto p1=next_pos(p,r);
    insert_at(p1,std::move(r));
    return p1;
  }

  template<class...Is>
  auto insert(const Position<Is...>& p, myrow_type_w_unit&& r)
  {
    auto p1=next_pos(p,r);
    insert_at(p1,std::move(r));
    return p1;
  }





  template<class ...Is>
  friend auto next_old(const vector_space& me, Position<Is...>& p)
  {
    return (p+...+me[typename x_is::ei{}]);
  }
  template<class... I, class Is, typename =std::enable_if_t<is_in_pack<Is,I...>(),int>>
  bool next(Is,Position<I...>& p)const
  {
    if (p[Is{}]()+1<(*this)[Is{}]().size(Is{},p)){
      ++p[Is{}]();
      return true;}
    else{
      p[Is{}]()={};
      return false;}

  }

  template<class... I, class... eis, typename =std::enable_if_t<is_in_pack<Index<eis...>,I...>(),int>>
  bool next(Index<eis...>,Position<I...>& p)const
  {
    return next_variant(p[Index<eis...>{}]());
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




  explicit vector_space(x_is&&...xs):x_is{std::move(xs)}...{}


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


  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space(const vector_space<x_is_1...>&x,const vector_space<x_is_2...>& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{x[typename x_is_1::ei{}]}...,x_not_common{y[typename x_not_common::ei{}]}...{
    (((*this)[typename x_is_common::ei{}]+=y[typename x_is_common::ei{}]),...);
  }
  template<class... x_is_1, class... x_is_2, class... x_is_common, class ...x_not_common>
  vector_space( vector_space<x_is_1...>&&x, vector_space<x_is_2...>&& y, Cs<x_is_common...>, Cs<x_not_common...>)
      : x_is_1{std::move(x)[typename x_is_1::ei{}]}...,x_not_common{std::move(y)[typename x_not_common::ei{}]}...{
    (((*this)[typename x_is_common::ei{}]+=y[typename x_is_common::ei{}]),...);
  }

  vector_space()=default;


  auto operator-()&&
  {
    (((*this)[typename x_is::ei{}]=(*this)[typename x_is::ei{}]),...);
    return std::move(*this);
  }
  auto operator-()const&
  {
    auto out=*this;
    ((out[typename x_is::ei{}]=out[typename x_is::ei{}]),...);
    return out;
  }


  template<class ...x_is2>
  auto operator +(const vector_space<x_is2...>& other)const &
  {
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(*this,other,x_common{},x_not_common{});
  }
  template<class ...x_is2>
  auto operator +( vector_space<x_is2...>&& other)&&
  {
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(std::move(*this),std::move(other),x_common{},x_not_common{});
  }

  template<class ...x_is2>
  auto operator -(const vector_space<x_is2...>& other)const &
  {
    auto x=other*(-1.0);
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(*this,x,x_common{},x_not_common{});
  }
  template<class ...x_is2>
  auto operator -( vector_space<x_is2...>&& other)&&
  {
    other=std::move(other)*v(-1.0,dimension_less{});
    typedef pack_difference_t<Cs<x_is2...>,Cs<x_is...>> x_not_common;
    typedef pack_difference_t<Cs<x_is2...>,x_not_common> x_common;
    return transfer_t<x_not_common,vector_space>(std::move(*this),std::move(other),x_common{},x_not_common{});
  }







  friend std::ostream& operator<<(std::ostream& os, const vector_space& me)
  {
    ((os<<typename x_is::myId{}<<"="<<me[typename x_is::myId{}]<<"  ")&&...);
    return os;
  }



};

template<class...x_is>
constexpr auto begin(const vector_space<x_is...>& ) {
  return Position<Index<typename x_is::ei...>>{};
}


template<class...x_is>
constexpr
    auto d_begin(const vector_space<x_is...>& )  {
  return (begin(x_is{})&&...);
}



template<class...x_is> struct get_Field_Indexes <vector_space<x_is...>>
{
  typedef get_Field_Indexes_t<x_is...> type;
};

template<class I, class... Is>
std::ostream& operator<<(std::ostream& os, const std::variant<I,Is...>& v) { std::visit([&os](auto& e){os<<e;},v); return os;}

template<class... Is>
std::istream& operator>>(std::istream& is,  std::variant<Is...>& v) {

  std::string s;
  is>>s;
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



template<class...x_is, class... col_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v, Cs<col_is...>)
{

  ((os<<col_is{}<<"\t"),...)<<"\n";
  auto p=d_begin(v);
  os<<std::setprecision(std::numeric_limits<double>::digits10+2);
  do{
    ((os<<v(p,col_is{})<<"\t"),...)<<"\n";
  }while( next(v,p));

  return os;
}


template<class...x_is>
std::ostream& to_DataFrame(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame(os,v,typename vector_space<x_is...>::myCols_w_unit{});
}

template<class...x_is>
std::ostream& to_DataTable(std::ostream& os, const vector_space<x_is...>& v)
{
  return to_DataFrame(os,v,typename vector_space<x_is...>::myCols{});
}




template<class...x_is>
std::istream& from_DataFrame(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>>typename x_is::cols_w_unit{}),...);
  auto p=d_begin(v);
  auto r=row_vector_w_unit(v);
  if (ss)
  {
    std::getline(is, s);
    auto scopy=s;
    ss.clear();
    ss.str(s);
    ss>>r;
    v.insert_at(p,std::move(r));
  }
  while (ss)
  {
    std::getline(is, s);
    ss.clear();
    ss.str(s);
    ss>>r;
    p=v.insert(p,std::move(r));
  }

  return is;
}


template<class...x_is>
std::istream& from_DataTable(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>>typename x_is::cols{}),...);
  auto p=d_begin(v);
  auto r=row_vector(v);
  if (ss)
  {
    std::getline(is, s);
    auto scopy=s;
    ss.clear();
    ss.str(s);
    ss>>r;
    v.insert_at(p,std::move(r));
  }
  while (ss)
  {
    std::getline(is, s);
    ss.clear();
    ss.str(s);
    ss>>r;
    p=v.insert(p,std::move(r));
  }

  return is;
}





template<class...x_is>
std::istream& from_DataFrame_new(std::istream& is,  vector_space<x_is...>& v)
{
  std::string s;
  std::getline(is, s);
  std::stringstream ss(s);
  ((ss>>typename x_is::ei{}),...);
  auto p=begin(v);
  auto r=row_vector(v);
  if (ss)
  {
    std::getline(is, s);
    ss.clear();
    ss.str(s);
    ss>>r;
    v.insert_at(p,std::move(r));
  }
  while (ss)
  {
    std::getline(is, s);
    ss.clear();
    ss.str(s);
    ss>>r;
    p=v.insert(p,std::move(r));
  }

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




template<bool Complete,class... Ds, class Id2, class Value_type>
Is_Complete<Complete,vector_space<Ds...,x_i<Id2,Value_type>>>
operator &&
    (Is_Complete<Complete,vector_space<Ds...>>&& d, x_i<Id2,Value_type>&& d2)
{
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,x_i<Id2,Value_type>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}


template<bool Complete,class... Ds>
Is_Complete<false,vector_space<Ds...>> operator &&(Is_Complete<Complete,vector_space<Ds...>>&& d, Nothing)
{
  return Is_Complete(std::bool_constant<false>{},std::move(d.value));
}



template <class Vector, class Position, class... Datum>
void fill_vector_field(Vector&,Position& ,vec<>, const Datum&...  )
{}

template <class Vector, class Position, class... X1>
void fill_vector_field(Vector&,Position& ,vec<X1...>)
{}


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




template< class Value_type,class...Datas>
auto consolidate(const Datas...d)
{
  typedef decltype ((vec<>{}<<...<<typename get_Field_Indexes<Datas>::type{})) myvec;

  typedef vector_field<myvec,Value_type> myField;



  typedef typename myField::value_type myValue_type;

  myValue_type out;
  auto p=myField::begin();

  fill_vector(out,p,myvec{},d...);

  return myField(std::move(out));
}

template< class Value_type,class... myIndex,class...Datas>
auto consolidate(vec<myIndex...>,const Datas...d)
{
  typedef decltype ((...<<get_Field_Indexes_t<Datas>{})<<vec<myIndex...>{}) myvec;

  if constexpr (myvec::index_size==0)
  {
    return Value_type{};
  }
  else
  {
    typedef vector_field<myvec,Value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type out;
    auto p=myField::begin();

    fill_vector_field(out,p,myvec{},d...);

    return myField(std::move(out));
  }
}






template<class ...xi>
using Matrix_t=decltype (vector_space<xi...>{}*vector_space<xi...>{});





#endif // QM_VECTOR_SPACE_H
