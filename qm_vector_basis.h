#ifndef QM_VECTOR_BASIS_H
#define QM_VECTOR_BASIS_H
#include "qm_tensor_base.h"


template<class Id> struct non_const{};

template<class Id> struct pass_id{};



template<class x_i, class Position=std::decay_t<decltype (std::declval<x_i>().begin() ) > >
constexpr inline bool self_referred_v = std::is_same_v<std::decay_t<decltype(std::declval<x_i>())>,std::decay_t<decltype (std::declval<x_i>()(Position{}))>>;




template<class e_i, class Value_type>
struct x_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

 // using element_type= element_type_t<Value_type> ;


 // using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;



private:
  Value_type value_;
public:

   x_i(const value_type& x):value_{x}{}
   x_i(value_type&& x):value_{std::move(x)}{}
  explicit x_i(e_i,value_type&& x):value_{std::move(x)}{}
  explicit x_i(e_i,const value_type& x):value_{x}{}

  template<class...Index>
//  ,typename=std::enable_if_t<
//                                std::is_same_v<typename Value_type::myIndexes,
//                                               vec<Index...> >,int>>
  x_i(e_i,vec<Index...>):value_{}{}

  x_i()=default;
  x_i const& operator[](e_i)const & {return *this;}
  //x_i& operator[](e_i) & {return *this;}
  x_i& operator[](non_const<e_i>)  {return *this;}

  friend auto begin(const x_i& me){ return begin(me());}

  static constexpr auto begin(){return value_type::begin();}
  static constexpr auto rec_begin() {return value_type::rec_begin();}


  auto&& operator[](e_i)&& {return *this;}

  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}



  inline friend
      bool operator==(const x_i& me, const x_i& ti)
  {
    if  (me()==ti())
    return true;
    else
        return false;
  }


  auto& operator()()const &{
    return value_;
  }
  auto& operator()()&{
    return value_;
  }
  auto operator()()&& {
    return value_;
  }




  template<class Position>
  auto operator()(const Position& p)->std::conditional_t<self_referred_v<Value_type,Position>,x_i&, decltype (make_x_i_view(e_i{},value_(p)))>
  {
    if constexpr (self_referred_v<Value_type,Position>)
      return *this;
    else
      return make_x_i_view(e_i{},value_(p));
  }

  template<class Position>
  auto operator()(const Position& p) const ->std::conditional_t<self_referred_v<Value_type,Position>,x_i const&, decltype (make_x_i_view(e_i{},value_(p)))>
  {
    if constexpr (self_referred_v<Value_type,Position>)
      return *this;
    else
      return make_x_i_view(e_i{},value_(p));
  }

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
  friend  auto operator +( Position<Ts...>& p,const x_i& me)//->decltype (p+me())
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
  friend auto operator*(const x_i& me,const v<T,unit>& a)->x_i<e_i,decltype (std::declval<Value_type>()*v<T,unit>{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},me()*a};
  }

  template<class unit,class T>
  friend auto operator*(const x_i& me,const logv<T,unit>& a)->x_i<e_i,decltype (std::declval<Value_type>()*logv<T,unit>{})>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},me()*a};
  }


  template<class unit,class T>
  friend auto operator*(const v<T,unit>& a,const x_i& me)->x_i<e_i,decltype (v<T,unit>{}*std::declval<Value_type>())>
  {
    return x_i<e_i,std::decay_t<decltype (me()*a)>>{e_i{},a*me()};
  }

  friend x_i operator*(const x_i& me,double a)
  {
    return x_i{e_i{},me()*a};
  }

  friend x_i operator*(double a,const x_i& me)
  {
    return x_i<e_i,std::decay_t<decltype (a*me())>>{e_i{},a*me()};
  }


  template< class e_i1,  class value_1>
  friend auto operator*(const x_i<e_i,Value_type> &one,const x_i<e_i1,value_1> &two )
  {
    if constexpr (std::is_same_v<Nothing,decltype (e_i{}*e_i1{}) >)
      return Nothing{};
    else
      return x_i<decltype (e_i{}*e_i1{}),std::decay_t<decltype(one()*two())>>(one()*two());
  }

  template<class Value_type_2>
  friend auto operator/(const x_i& me,const Value_type_2& a)->x_i<e_i,decltype (std::declval<Value_type>()/std::declval<Value_type_2>())>
  {
    return x_i<e_i,std::decay_t<decltype (me()/a)>>{e_i{},me()/a};
  }




  friend x_i operator+(const x_i& me, const x_i& other)
  {
    x_i out{me};
    out()=out()+other();
    return out;
  }



};

template<class ei, class value>
auto operator-(x_i<ei,value>&& me){ return x_i{ei{},-me()};};

template<class ei, class value>
auto operator-(const x_i<ei,value>& me){
  auto out{me};

  return -std::move(out);};




template<class e_i, class Value_type1, class Value_type2>
auto operator-(const x_i<e_i,Value_type1>& one, const x_i<e_i,Value_type2>& two)
{
  return x_i{e_i{},one()-two()};
}
template<class e_i, class Value_type1, class Value_type2>
auto operator-( x_i<e_i,Value_type1>&& one,  x_i<e_i,Value_type2>&& two)
{
  return x_i{e_i{},std::move(one)()-std::move(two)()};
}

template<class e_i, class Value_type1, class Value_type2>
auto operator+(const x_i<e_i,Value_type1>& one, const x_i<e_i,Value_type2>& two)
{
  return x_i{e_i{},one()+two()};
}
template<class e_i, class Value_type1, class Value_type2>
auto operator+( x_i<e_i,Value_type1>&& one,  x_i<e_i,Value_type2>&& two)
{
  return x_i{e_i{},std::move(one)()+std::move(two)()};
}



template <class varName, class...Index>
x_i(varName,vec<Index...>)->x_i<varName,vector_field<vec<Index...>,v_t<varName>>>;

template <class varName, class... indexName>
using field_t=x_i<varName,vector_field<vec<indexName...>,v_t<varName>>>;


/*
template<class Id, class Value_type,class Datas, class Rand>
x_i<Id,Value_type> sample(const x_i<Id,Value_type>& d, const Datas& , Rand& ){return d;}

template<class Id, class Value_type,class... Datas>
x_i<Id,Value_type> calculate(const x_i<Id,Value_type>& d, const Datas&... ){return d;}


template<class Id, class Value_type,class... Datas>
auto logP(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}

template<class Id, class Value_type,class... Datas>
auto FIM(const x_i<Id,Value_type>& , const Datas&... ){return logP_zero{};}
*/

template<class e_i,class Value_type,class... Xs> struct get_Field_Indexes <x_i<e_i,vector_field< vec<Xs...>,Value_type>>>
{
  typedef vec<Xs...> type;
};
template<class e_i,class Value_type> struct get_Field_Indexes <x_i<e_i,Value_type>>
{
  typedef vec<> type;
};



template<class e_i, class Value_type>
x_i_view_non_const<e_i,Value_type> make_x_i_view(e_i,Value_type& x);

template<class e_i, class Value_type>
x_i_view_const<e_i,Value_type> make_x_i_view(e_i,Value_type const & x);



template<class e_i, class Value_type>
struct x_i_view_non_const
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  // using element_type= element_type_t<Value_type> ;


  // using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;



private:
  Value_type* value_;
public:

  explicit x_i_view_non_const(value_type& x):value_{&x}{}
  explicit x_i_view_non_const(e_i,value_type& x):value_{&x}{}

  auto operator[](e_i)const  {return make_x_i_view(e_i{},*value_);}
  //x_i& operator[](e_i) & {return *this;}
  auto& operator[](non_const<e_i>)  {return *this;}

  friend auto begin(const x_i_view_non_const& me){ return begin(me());}

  static constexpr auto begin(){return value_type::begin();}
  static constexpr auto rec_begin() {return value_type::rec_begin();}



  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}



  inline friend
      bool operator==(const x_i_view_non_const& me, const x_i_view_non_const& ti)
  {
    if  (me()==ti())
      return true;
    else
      return false;
  }


  auto& operator()()const { return *value_;}
  auto& operator()(){ return *value_;}


  explicit x_i_view_non_const(const x_i_view_non_const& other):
                                                        value_{other.value_}

  {}

  explicit x_i_view_non_const(x_i_view_non_const&& other):
                                                   value_{other.value_}{}

   x_i_view_non_const& operator=( x_i_view_non_const& other)
  {
    value_=other.value_;
    return *this;
  }
  x_i_view_non_const& operator=(x_i_view_non_const&& other)
  {
    value_=std::move(other.value_);
    return *this;
  }

  ~x_i_view_non_const(){}


  template<class Position>
  auto operator()(const Position& p)->std::conditional_t<self_referred_v<Value_type,Position>,x_i_view_non_const&,
                                                           std::decay_t<decltype (make_x_i_view(e_i{},(*value_)(p)))>>
  {
    if constexpr (self_referred_v<Value_type,Position>)
      return *this;
    else
      return make_x_i_view(e_i{},(*value_)(p));
  }

  template<class Position>
  auto operator()(const Position& p)const
  {
    return make_x_i_view(e_i{},(*value_)(p));
  }






  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){return x_i_view_non_const<decltype (ei{}(u,d)),value_type>{ei{}(u,d),(*this)()};}

  inline friend std::ostream& operator<<(std::ostream& os, const x_i_view_non_const& me)
  {
    return os<<me();
  }
  inline friend std::istream& operator>>(std::istream& is,  x_i_view_non_const& me)
  {
    return is>>me();
  }


  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const x_i_view_non_const& me)//->decltype (p+me())
  {
    return p+me();
  }
  friend  bool operator +(bool p,const x_i_view_non_const& )
  {
    return p;
  }


};


template<class e_i, class Value_type>
struct x_i_view_const
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  // using element_type= element_type_t<Value_type> ;


  // using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;



private:
  Value_type const* value_;
public:

  explicit x_i_view_const(const value_type& x):value_{&x}{}
  explicit x_i_view_const(e_i,const value_type& x):value_{&x}{}


  x_i_view_const const& operator[](e_i)const & {return *this;}
  //x_i& operator[](e_i) & {return *this;}

  friend auto begin(const x_i_view_const& me){ return begin(me());}

  static constexpr auto begin(){return value_type::begin();}
  static constexpr auto rec_begin() {return value_type::rec_begin();}



  auto& get(e_i)const  {return *this;}



  inline friend
      bool operator==(const x_i_view_const& me, const x_i_view_const& ti)
  {
    if  (me()==ti())
      return true;
    else
      return false;
  }


  auto& operator()()const { return *value_;}


//  template<class Position>
//  auto& operator()(const Position& p)const { return value_(p);}



  template<class Position>
  auto operator()(const Position& p)const ->std::conditional_t<self_referred_v<Value_type,Position>,x_i_view_const const&,
                                                           std::decay_t<decltype (make_x_i_view(e_i{},(*value_)(p)))>>
  {
    if constexpr (self_referred_v<Value_type,Position>)
      return *this;
    else
      return make_x_i_view(e_i{},(*value_)(p));
  }

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...> u, dn<iDowns...> d){return x_i_view_const<decltype (ei{}(u,d)),value_type>{ei{}(u,d),(*this)()};}

  inline friend std::ostream& operator<<(std::ostream& os, const x_i_view_const& me)
  {
    return os<<me();
  }
  inline friend std::istream& operator>>(std::istream& is,  x_i_view_const& me)
  {
    return is>>me();
  }


  template<class... Ts>
  friend  auto operator +( Position<Ts...>& p,const x_i_view_const& me)//->decltype (p+me())
  {
    return p+me();
  }
  friend  bool operator +(bool p,const x_i_view_const& )
  {
    return p;
  }

 explicit x_i_view_const(const x_i_view_const& other):value_{other.value_}{}

  explicit x_i_view_const(x_i_view_const&& other):value_{std::move(other.value_)}{}

  x_i_view_const& operator=(const x_i_view_const& other)
  {
    value_=other.value_;
    return *this;
  }
  x_i_view_const& operator=(x_i_view_const&& other)
  {
    value_=other.value_;
    return *this;
  }

  ~x_i_view_const(){}




};



template<class e_i, class Value_type>
x_i_view_non_const<e_i,Value_type> make_x_i_view(e_i,Value_type& x)
{
  return x_i_view_non_const(e_i{},x);
}

template<class e_i, class Value_type>
x_i_view_const<e_i,Value_type> make_x_i_view(e_i,Value_type const & x)
{
  return x_i_view_const(e_i{},x);
}


template<class e_i,class Value_type,class... Xs> struct get_Field_Indexes <x_i_view_const<e_i,vector_field< vec<Xs...>,Value_type>>>
{
  typedef vec<Xs...> type;
};
template<class e_i,class Value_type> struct get_Field_Indexes <x_i_view_const<e_i,Value_type>>
{
  typedef vec<> type;
};





#endif // QM_VECTOR_BASIS_H
