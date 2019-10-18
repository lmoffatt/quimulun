#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H

//#include "qm_data.h"

#include "qm_vector_field.h"





struct Iu_{};
struct Ju_{};
struct Id_{};
struct Jd_{};




template<class ...Is> struct dn;

template<class ...Is> struct up;

template <class, class> struct te;



struct Scalar{};

template<class ...Is> struct dn{

  template<class... as>
  Nothing operator()(const up<as...>& ){return Nothing{};}

  Scalar operator()(const up<Is...>& ){return Scalar{};};
};

template<class ...Is> struct up{
  template<class... as>
  Nothing operator()(const dn<as...>& ){return Nothing{};}

  Scalar operator()(const dn<Is...>& ){return Scalar{};};

};


template<class ...>struct ein;

template<class te,class Up, class Down>
struct ein<te,Up,Down>{

};


template <class ...Ups, class ...Downs>
struct te<up<Ups...>,dn<Downs...>>{
  //  template<class... as, class... bs>
  //  Nothing operator()(const ve<as...>&, const co<bs...>& ){return Nothing{};}
  //  Scalar operator()(ve<Ups...>,co<Downs...>){return Scalar{};};


constexpr static auto myClassNameDown()
  {

    if constexpr (sizeof... (Downs)>0)
      return (my_static_string("d/d")+...+(Downs::className+my_static_string("_")));
    else return my_static_string("");
  }

  constexpr static auto myClassNameUp()
  {

    if constexpr (sizeof... (Ups)>0)
      return ((Ups::className+my_static_string("_"))+...);
    else return my_static_string("");
  }
  constexpr static auto myClassName()
  {
    return myClassNameDown()+myClassNameUp();
  }
  constexpr static auto  className=myClassName();

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...>, dn<iDowns...>){return ein<te,up<iUps...>,dn<iDowns...>>{};}

  template <class ...Ups2, class ...Downs2>
  friend
      auto operator*(te,te<up<Ups2...>,dn<Downs2...>>){
    return te<up<Ups...,Ups2...>,dn<Downs...,Downs2...>>{};
  }

  friend constexpr bool operator==(const te& me, const te& ti){return true;}

};



template <class ...Ups0, class ...iUps0,class a, class ia,
          class b>

constexpr auto operator * (ein<te<up<Ups0...>,dn<a>>,up<iUps0...>,dn<ia>>,
                         ein<te<up<b>,dn<>>,up<ia>,dn<>>)
{

  /**
        -**         -A
       |          |
       |      *   |
       |          |
        -A

  **/
      if constexpr (std::is_same_v<a,b >)
          return ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>{};
  else
      return Nothing{};

}

template <class ...Ups0, class ...iUps0,
          class Up1, class iUp1>
constexpr auto operator * (ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>,
                         ein<te<up<Up1>,dn<>>,up<iUp1>,dn<>>)
{

  /**
        -**         -A            -**A
       |          |              |
       |      *   |        -->   |
       |          |              |


  **/
      return ein<te<up<Ups0...,Up1>,dn<>>,up<iUps0...,iUp1>,dn<>>{};

}



template <class ...Ups0, class ...iUps0,class A, class iA,
          class B, class iB>

constexpr auto operator * (ein<te<up<Ups0...>,dn<A>>,up<iUps0...>,dn<iA>>,
                         ein<te<up<B>,dn<>>,up<iB>,dn<>>)
{

  /**
        -**         -B            -**B
       |          |              |
       |      *   |        -->   |
       |          |              |
        -A                        -A

  **/
      return ein<te<up<Ups0...,B>,dn<A>>,up<iUps0...,iB>,dn<iA>>{};

}

template <class ...Downs0, class ...iDowns0>
constexpr Nothing join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,Nothing)
{
  return Nothing{};
}



template <class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,
                    ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return ein<te<up<Ups1...>,dn<Downs0...,Downs1...>>,up<iUps1...>,dn<iDowns0...,iDowns1...>>{};
}



template <class Up0, class iUp0,class Up00, class iUp00,class ...Ups0, class ...iUps0,class Down1, class iDown1,class ...Downs0, class ...iDowns0>

constexpr auto operator * (ein<te<up<Up0,Up00,Ups0...>,dn<Downs0...>>,up<iUp0,iUp00,iUps0...>,dn<iDowns0...>>,
                         ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>)
{

  /**
             -***                                                         -*      -*    -*
            |          |                        |             |          |       |     |
            |      *   |           --> join (   |      ,      |      *   |   *   |  *  |     )
            |          |                        |             |          |       |     |
             -***       -*                       -***          -*

  **/


      return  join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>{},
                  ((ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{}*
                    ein<te<up<Up0>,dn<>>,up<iUp0>,dn<>>{}*
                    ein<te<up<Up00>,dn<>>,up<iUp00>,dn<>>{})*...*
                   ein<te<up<Ups0>,dn<>>,up<iUps0>,dn<>>{})
                  );

}




template <class ...Ups, class ...iUps,class Down0, class iDown0,class Down1, class iDown1,class ...Downs, class ...iDowns>

constexpr auto operator * (ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>,
                         ein<te<up<>,dn<Down0,Down1,Downs...>>,up<>,dn<iDown0,iDown1,iDowns...>>)
{
  /**
             -***                                       -***
            |          |                                |          |       |     |
            |      *   |           -->                  |      *   |   *   |  *  |
            |          |                                |          |       |     |
                        -***                                       -*      -*    -*

  **/

      return ((ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>{}*
               ein<te<up<>,dn<Down0>>,up<>,dn<iDown0>>{}*ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{})
              *...*
              ein<te<up<>,dn<Downs>>,up<>,dn<iDowns>>{});
}


template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0>
constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         Nothing)
{

  return Nothing{};

}

template <          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto operator * (Nothing,ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return Nothing{};

}




template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>

constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{

  /**
        -**         -**                     -****
       |          |                        |          |
       |      *   |            -->         |      *   |
       |          |                        |          |
        -**         -**                                -****

  **/
      return ein<te<up<Ups0...,Ups1...>,dn<>>,up<iUps0...,iUps1...>,dn<>>{}*
      ein<te<up<>,dn<Downs0...,Downs1...>>,up<>,dn<iDowns0...,iDowns1...>>{};

}

//template <class...> struct x_i;


template<class e_i, class Value_type>
struct x_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  using element_type= typename Value_type::element_type ;


  using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;

  using cols=recursive_t<ei,typename value_type::cols>;
  using row_type=typename value_type::row_type;

private:
  Value_type value_;
public:

  x_i(value_type&& x):value_{std::move(x)}{}
  x_i(e_i,value_type&& x):value_{std::move(x)}{}
  x_i(e_i,const value_type& x):value_{x}{}
  x_i()=default;
  x_i const& operator[](e_i)const & {return *this;}
  x_i& operator[](e_i) & {return *this;}

  friend auto begin(const x_i& me){ return begin(me());}

  x_i operator[](e_i)&& {return *this;}

  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}



  inline friend
      bool operator==(const x_i& me, const x_i& ti)
  {
    return me()==ti();
  }


  auto& operator()()const &{ return value_;}
  auto& operator()()&{ return value_;}
  //auto operator()()&& {return value_;}


  template<class...Is>
  void insert_at(const Position<Is...>& p, x_i<e_i,element_type>&& r)
  {
    this->operator()().insert_at(p,std::move(r)());
  }

  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {
    this->operator()().insert_at(p,std::move(r));
  }



  template<class Position>
  auto& operator()(const Position& p){ return value_(p);}

  template<class Position>
  auto& operator()(const Position& p)const { return value_(p);}

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){return x_i<decltype (ei{}(u,d)),value_type>{ei{}(u,d),(*this)()};}

  inline friend std::ostream& operator<<(std::ostream& os, const x_i& me)
  {
    return os<<me();
  }
  inline friend std::istream& operator>>(std::istream& is,  x_i& me)
  {
    return is>>me();
  }


  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const x_i& me)->decltype (p+me())
  {
    return p+me();
  }
  friend  bool operator +(bool p,const x_i& )
  {
    return p;
  }


  x_i& operator+=(const x_i& other){ (*this)()+=other(); return *this;}


  template<class Value_type_2>
  auto operator*=(const Value_type_2& x)->std::enable_if_t<std::is_same_v<Value_type,decltype (value_*x) >,x_i&>
  {
    (*this)()*=x;
    return *this;
  }

  template<class unit,class T>
  friend auto operator*(const x_i& me,const v<T,unit>& a)->x_i<e_i,decltype (Value_type{}*v<T,unit>{})>
  {
    return {e_i{},me()*a};
  }

  template<class unit,class T>
  friend auto operator*(const v<T,unit>& a,const x_i& me)->x_i<e_i,decltype (v<T,unit>{}*Value_type{})>
  {
    return {e_i{},a*me()};
  }

  friend x_i operator*(const x_i& me,double a)
  {
    return x_i{e_i{},me()*a};
  }

  friend x_i operator*(double a,const x_i& me)
  {
    return {e_i{},a*me()};
  }



  template<class Value_type_2>
  friend auto operator/(const x_i& me,const Value_type_2& a)->x_i<e_i,decltype (Value_type{}/Value_type_2{})>
  {
    return {e_i{},me()/a};
  }



  friend x_i operator+(const x_i& me, const x_i& other)
  {
    x_i out{me};
    out()+=other();
    return out;
  }


  friend x_i operator-(const x_i& me, const x_i& other)
  {
    x_i out{me};
    out()-=other();
    return out;
  }

};


template<class Id, class Value_type,class Datas, class Rand>
x_i<Id,Value_type> sample(const x_i<Id,Value_type>& d, const Datas& , Rand& ){return d;}

template<class Id, class Value_type,class... Datas>
x_i<Id,Value_type> calculate(const x_i<Id,Value_type>& d, const Datas&... ){return d;}


template<class Id, class Value_type,class... Datas>
auto logP(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}

template<class Id, class Value_type,class... Datas>
auto FIM(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}



template< class e_i0,class e_i1, class value_0, class value_1>
auto operator*(const x_i<e_i0,value_0> &one,const x_i<e_i1,value_1> &two )
{
  if constexpr (std::is_same_v<Nothing,decltype (e_i0{}*e_i1{}) >)
    return Nothing{};
  else
    return x_i(e_i0{}*e_i1{},one()*two());
}

template<class e_i,class Value_type,class... Xs> struct get_Field_Indexes <x_i<e_i,vector_field< vec<Xs...>,Value_type>>>
{
  typedef vec<Xs...> type;
};
template<class e_i,class Value_type> struct get_Field_Indexes <x_i<e_i,Value_type>>
{
  typedef vec<> type;
};


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





template<class...x_is> struct vector_space: public x_is...
{
  using x_is::operator[]...;
  typedef Cs<typename x_is::ei...> my_eis;
  typedef Cs<typename x_is::myId...> myIds;

  using myIndex=Index<typename x_is::ei...>;


  using myCols=pack_concatenation_t<typename x_is::cols...>;
  using myrow_type=decltype (std::tuple_cat(typename x_is::row_type{}...));

  using cols=pack_unique_t<pack_concatenation_t<Cs<index_k>,pack_concatenation_t<typename x_is::value_type::cols...>>>;

  using row_type=decltype(std::tuple_cat(std::tuple<std::variant<typename x_is::ei...>>{},typename x_is::value_type::row_type{}...));

  template<class Is>
  static constexpr auto index_to_myCol_number(){
    using xi=std::decay_t<decltype(std::declval<vector_space>()[Is{}])>;
    using coli=typename xi::cols;
    return pack_index_cs(coli{},myCols{});
  }


  //using gsdg=typename cols::ger;

  template<class J, class Position, class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
  auto operator()(const Position& i, J)const {return (*this)[J{}](i);}

  template< class...Is, class J,class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>(),float>>
  auto operator()(const Position<Is...>& i, J j)const-> decltype((*this)(i[Index<typename x_is::ei...>{}]())()(i,j))
  {
    return (*this)(i[Index<typename x_is::ei...>{}]())()(i,j);
  }

  template< class...Is, class =std::enable_if_t<is_in_pack<Index<typename x_is::ei...>,Is...>(),float>>
  auto& operator()(const Position<Is...>& i, index_k j)const {
    return i[Index<typename x_is::ei...>{}]();
  }



  template<class J, class j_in, class Position,class =std::enable_if_t<is_in_pack<J,typename x_is::ei...>(),int>>
  auto operator()(const Position& i,recursive<J,j_in> r)const ->decltype ((*this)[J{}](i)(i,j_in{}))
  {return (*this)[J{}](i)(i,j_in{});}




  auto & operator()(const std::variant<typename x_is::ei...>& ind)const
  {
    return std::visit([this](auto& id)->decltype ((*this)[id]) {return (*this)[id];},ind);
  }






  template<class Is>
  using get_xi_t=std::decay_t<decltype (std::declval<vector_space&>()[Is{}])>;

  template<class Position,class Is>
  using get_element_xi_t=std::decay_t<decltype (std::declval<vector_space&>()(std::declval<Position>(),Is{}))>;

  friend constexpr myrow_type row_vector(const vector_space&) {return  myrow_type{};}


  template <class... I, class... Is, std::size_t...In>
  bool is_same(const Position<Is...>& p,const myrow_type& r, Cs<I...>,std::index_sequence<In...>)const
  {
    return ((std::get<In>(r)==(*this)(p,I{}))&&...);

  }

  template <class... Ind, class... Is, std::size_t...In>
  static bool is_same_index(Index<Ind...>,const Position<Is...>& p,const myrow_type& r, std::index_sequence<In...>)
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

  template <class... Ind, class... Is>
  bool is_same(Index<Ind...>,const Position<Is...>& p,const myrow_type& r)const
  {
    auto index_rows=pack_multindex<std::variant<Ind...>>(transfer_t<myrow_type,Cs<>>{});
    return is_same_index(Index<Ind...>{},p,r,index_rows);
  }



  template <class I, class... Is, std::size_t...In>
  void insert_at(const Position<Is...>& p, myrow_type&& r,std::index_sequence<In...>)
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

  template <class... Is>
  void insert_at(const Position<Is...>& p, myrow_type&& r)
  {

    (insert_at<x_is>(p,std::move(r)),...);
  }


  template <class... Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {
    auto v=p[Index<typename x_is::ei...>{}]();
    auto [r_ei, r_value]=distribute(Cs<std::variant<typename x_is::ei...>>{},
                                      transfer_t<decltype(std::tuple_cat(typename x_is::value_type::row_type{}...)),Cs<>>{},std::move(r));
    std::visit([this,&r_value,&p](auto ei){(*this)[ei].insert_at(p,std::move(r_value));},v);


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
                       if (is_same(Is{},p.first,r)) return p;
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
    return (
               [&me,&p](auto b){
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

  template<class x_j>
  auto operator * (const x_j&  two)const
  {
    return (vector_space<>{}|...|((*this)[typename x_is::ei{}]*two));
  }


  vector_space(x_is&&...xs):x_is{std::move(xs)}...{}


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

  template<class x_i>
  auto operator + (x_i&& x)&&
  {
    if constexpr (is_in_pack<x_i,x_is...>())
    {
      (*this)[typename x_i::ei{}]=std::forward<x_i>(x);
      return *this;
    }
    else
    {
      return    vector_space<x_is...,std::decay_t<x_i>>{std::move(*this)[typename x_is::ei{}]...,std::forward<x_i>(x)};
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




  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){
    return vector_space<std::decay_t<decltype ((*this)[typename x_is::ei{}](u,d))>...>
        {(*this)[typename x_is::ei{}](u,d)...};

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
  return to_DataFrame(os,v,typename vector_space<x_is...>::myCols{});
}

template<class...x_is>
std::istream& from_DataFrame(std::istream& is,  vector_space<x_is...>& v)
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









#endif // QM_VECTOR_SPACE_H
