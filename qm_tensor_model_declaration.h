#ifndef QM_TENSOR_MODEL_H
#define QM_TENSOR_MODEL_H
#include "qm_tensor_distribution_declaration.h"



#include <functional>
#include <type_traits>

struct Calculator_new{};
struct Glue_new{};
struct logProbability_new{};
struct logPrior_new{};
struct logLikelihood_new{};

template<class T> struct Result{
  using type=Result<T>;
  using result_type=T;
};

template<> struct Result<Nothing>{
  using type=Nothing;
};

template<class T> using Result_t=typename Result<T>::type;



//namespace qm {
template<class Id> struct all{
  //  constexpr static auto className=Id::className+my_static_string("_all");
};

template<class Id> struct all_view_const{
  //  constexpr static auto className=Id::className+my_static_string("_all");
};
template<class Id> struct all_view_non_const{
  //  constexpr static auto className=Id::className+my_static_string("_all");

};


//namespace qm {
template<class Id> struct pos{
  //  constexpr static auto className=Id::className+my_static_string("_all");
};


template<class T >struct all_view_const_impl{using type=all_view_const<T>;};

template<class T >struct all_view_const_impl<all_view_const<T>>{using type=all_view_const<T>;};

template< class T>
using all_view_const_t=typename all_view_const_impl<T>::type;




template<class...Xs, class valuetype>
struct all_view_const<vector_field<vec<Xs...>,valuetype>>
{
  vector_field<vec<Xs...>,valuetype>const * me;

  all_view_const(vector_field<vec<Xs...>,valuetype>const & v):me{&v}{}
  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto const& operator()() const {return *me;}

  template<class Position>
  auto const& operator()(const Position& ){ return *me;}


  template<class Position>
  auto const& operator()(const Position& )const { return *me;}

  all_view_const(const all_view_const& other):me{other.me}{}
  all_view_const( all_view_const&& other):me{std::move(other.me)}{}
  all_view_const& operator=(const all_view_const& other){me=other.me; return *this;}
  all_view_const& operator=( all_view_const&& other){me=std::move(other.me); return *this;}
  ~all_view_const(){}




};

template<class...Xs, class valuetype>
struct all_view_non_const<vector_field<vec<Xs...>,valuetype>>
{
  vector_field<vec<Xs...>,valuetype> * me;

  all_view_non_const(vector_field<vec<Xs...>,valuetype> & v):me{&v}{}
  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto const& operator()() const {return *me;}

  template<class Position>
  auto& operator()(const Position& ){ return *me;}


  template<class Position>
  auto const& operator()(const Position& )const { return *me;}

  all_view_non_const(const all_view_non_const& other):me{other.me}{}
  all_view_non_const( all_view_non_const&& other):me{other.me}{}
  all_view_non_const& operator=(const all_view_non_const& other){me=other.me; return *this;}
  all_view_non_const& operator=( all_view_non_const&& other){me=other.me; return *this;}
  ~all_view_non_const(){}



};



template<class Id, class...Xs, class valuetype>
class x_i_view_const<all<Id>,all_view_const<vector_field<vec<Xs...>,valuetype>>>
{
public:
  typedef Id ei;

  typedef Id myId;
  auto& operator[](ei)const  {return *this;}

  all_view_const<vector_field<vec<Xs...>,valuetype>> me;

  x_i_view_const(x_i<Id,vector_field<vec<Xs...>,valuetype>>const & v):me{v()}{}
  x_i_view_const(x_i_view_const<Id,vector_field<vec<Xs...>,valuetype>>const & v):me{v()}{}


  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto operator()() const {return me;}

  template<class Position>
  auto operator()(const Position& ){ return make_x_i_view(Id{},me());}


  template<class Position>
  auto operator()(const Position& )const { return make_x_i_view(Id{},me());}

};

template<class Id, class...Xs, class valuetype>
class x_i_view_non_const<all<Id>,all_view_non_const<vector_field<vec<Xs...>,valuetype>>>
{
public:
  typedef Id ei;

  using myId=Id;
  all_view_non_const<vector_field<vec<Xs...>,valuetype>> me;

  x_i_view_non_const(x_i<Id,vector_field<vec<Xs...>,valuetype>> & v):me{v()}{}
  x_i_view_non_const(x_i_view_non_const<Id,vector_field<vec<Xs...>,valuetype>>& v):me{v()}{}


  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  auto& operator()() const {return me;}

  template<class Position>
  auto operator()(const Position& ){ return make_x_i_view(Id{},me());}


  template<class Position>
  auto operator()(const Position& )const { return make_x_i_view(Id{},me());}

  //x_i& operator[](e_i) & {return *this;}
  auto& operator[](non_const<ei>)  {return *this;}


};

auto make_all(Nothing){return Nothing{};}


template<class Id, class...Xs, class valuetype>
auto make_all(x_i<Id,vector_field<vec<Xs...>,valuetype>>const & v)
{
  return x_i_view_const<all<Id>,all_view_const<vector_field<vec<Xs...>,valuetype>>>(v);
}

template<class Id, class valuetype>
auto make_all(x_i<Id,valuetype>const & v)->std::conditional_t<self_referred_v<valuetype>,
                                                                 decltype (v),
                                                                 x_i_view_const<all<Id>,all_view_const_t<valuetype>>>
{
  if constexpr (self_referred_v<valuetype>)
    return v;
  else
    return x_i_view_const<all<Id>,all_view_const_t<valuetype>>(v);
}


template<class Id, class valuetype/*, std::enable_if_t<!is_this_template_class_v<all_view_const,valuetype>>*/>
auto make_all(x_i_view_const<Id,valuetype>const & v)->std::conditional_t<self_referred_v<valuetype>,
                                                                            decltype (v),
                                                                            x_i_view_const<all<Id>,all_view_const_t<valuetype>>>
{
  if constexpr (self_referred_v<valuetype>)
    return v;
  else
    return x_i_view_const<all<Id>,all_view_const_t<valuetype>>(v);
}




template<class Id, class valuetype>
auto make_all(x_i<Id,valuetype> & v)->std::conditional_t<self_referred_v<valuetype>,
                                                           decltype (v),
                                                           x_i_view_non_const<all<Id>,all_view_non_const<valuetype>>>
{
  if constexpr (self_referred_v<valuetype>)
    return v;
  else
    return x_i_view_non_const<all<Id>,all_view_non_const<valuetype>>(v);
}

template<class Id, class valuetype>
auto make_all(x_i_view_non_const<Id,valuetype> & v)->std::conditional_t<self_referred_v<valuetype>,
                                                                          decltype (v),
                                                                          x_i_view_non_const<all<Id>,all_view_non_const<valuetype>>>
{
  if constexpr (self_referred_v<valuetype>)
    return v;
  else
    return x_i_view_non_const<all<Id>,all_view_non_const<valuetype>>(v);
}




template<class Id, class IdOld>
class View{
public:
  View(Id,IdOld){}
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

};



template<class Id,class G, class... Xs>
class  F
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  auto& f()const{ return g_;}
  auto& f(){ return g_;}

  /*
  template<class... Param>
  auto operator()(const Param&... par)const
  {
//    using test=typename Id::myID;
//    using test2=typename Cs<Xs...>::myXs;

  auto  out=apply(g_,get_from(Xs{},par...)()...);

  return x_i(Id{},std::move(out));
}
    */
    F(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
};



template<class ...>class F_new;


template<class Id,class G, class... Xs>
class  F_new<Id,G,Arguments<Xs...>>
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  auto& f()const{ return g_;}
  auto& f(){ return g_;}

  F_new(Id ,G&& g, Arguments<Xs...>&&):g_{std::move(g)}{}
};


template<class Id,class G, class... Xs>
F_new(Id&&,G&&,Arguments<Xs...>&&)->F_new<Id,G,Arguments<Xs...>>;





template<class Id,class G, class Xrandom,class... Xs>
class  Fr
{
private:
  G g_;
public:
  typedef   Id myId;
  auto& f()const{ return g_;}
  auto& f(){ return g_;}
  auto &operator[](Id)const {return *this;}

  //  template<class x_rand,class... Param>
  //  auto operator()(x_rand& x,Param&&... par)const
  //  {
  //    auto  out=apply_random(g_,x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);

  //    return x_i(Id{},std::move(out));
  //  }
  Fr(Id ,G&& g,non_const<Xrandom>&&, Xs&&...):g_{std::move(g)}{}
};







template<class Id,class G, class Xrandom,class... Xs>
class  Fr<all<Id>,G,Xrandom,Xs...>
{
private:
  G g_;
public:
  auto& f()const{ return g_;}
  auto& f(){ return g_;}
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  //  template<class x_rand,class... Param>
  //  auto operator()(x_rand& x, Param&&... par)const
  //  {
  //    auto  out=std::invoke(g_,x[non_const<Xrandom>{}](),get_from(Xs{},std::forward<Param>(par)...)...);

  //    return x_i(Id{},std::move(out));
  //  }
  Fr(all<Id> ,G&& g,non_const<Xrandom>&&, Xs&&...):g_{std::move(g)}{}
};


template<class Id,class G, class Xrandom,class... Xs>
class  Fr<pos<Id>,G,Xrandom,Xs...>
{
private:
  G g_;
public:
  auto& f()const{ return g_;}
  auto& f(){ return g_;}
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Fr(pos<Id> ,G&& g,non_const<Xrandom>&&, Xs&&...):g_{std::move(g)}{}
};






template<class Id,class G, class... Xs>
class  F<all<Id>,G,Xs...>
{
private:
  G g_;
public:
  auto& f()const{ return g_;}
  auto& f(){ return g_;}
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  //  template<class... Param>
  //  auto operator()( Param&&... par)const
  //  {
  //    auto  out=std::invoke(g_,get_from(Xs{},std::forward<Param>(par)...)...);

  //    return x_i(Id{},std::move(out));
  //  }
  F(all<Id> ,G&& g, Xs&&...):g_{std::move(g)}{}
};



template<class Id,class Calc,class Fi, class... Xs>
class  Fq
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Fq(Id ,Calc&&,Fi&& , Xs&&...){}
};

template<class...> struct Fq_new;

template<class Id,class Calc,class Fi, class... Xs>
struct  Fq_new<Id,Calc,Fi,Arguments<Xs...>>
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Fq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&){}
};

template<class Id,class Calc,class Fi, class... Xs>
Fq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&)->Fq_new<Id,Calc,Fi,Arguments<Xs...>>;

template<class...> struct Dq_new;

template<class Id,class Calc,class Fi, class... Xs>
struct  Dq_new<Id,Calc,Fi,Arguments<Xs...>>
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Dq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&){}
};

template<class Id,class Calc,class Fi, class... Xs, class... Is>
struct  Dq_new<Id,Calc,Fi,Arguments<Xs...>, Index_struct<Is...>>
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Dq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&, Index_struct<Is...>&&){}
};

template<class Id,class Calc,class Fi, class... Xs>
Dq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&)->Dq_new<Id,Calc,Fi,Arguments<Xs...>>;

template<class Id,class Calc,class Fi, class... Xs, class... Is>
Dq_new(Id&& ,Calc&&,Fi&& , Arguments<Xs...>&&, Index_struct<Is...>&&)->Dq_new<Id,Calc,Fi,Arguments<Xs...>, Index_struct<Is...>>;


template<class Id,class Calc,class Fi, class Xrandom, class... Xs>
class  Frq
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Frq(Id ,Calc&&, Fi&& , non_const<Xrandom>&&,Xs&&...){}
};

template<class Id,class Calc,class Fi, class Xrandom, class Pos,class... Xs>
class  Frq<pos<Id>,Calc,Fi,Xrandom,Pos,Xs...>
{
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  Frq(pos<Id> ,Calc&&, Fi&& , non_const<Xrandom>&&,Pos&&,Xs&&...){}
};











/*

template<class ...Ids,class G, class... Xs>
class  F<Cs<Ids...>,G,Xs...>
{
private:
  G g_;
public:
  typedef   Cs<Ids...> myId;
  auto &operator[](Cs<Ids...>)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    auto  out=apply(g_,get_from(Xs{},par...)...);
    return std::apply([](auto&&...v){return vector_space(x_i(Ids{},std::move(v))...);}, std::move(out));
  }
  F(Cs<Ids...> ,G&& g, Xs&&...):g_{std::move(g)}{}
};

template<class ...Ids,class G, class... Xs>
class  F<all<Cs<Ids...>>,G,Xs...>
{
private:
  G g_;
public:
  typedef   Cs<Ids...> myId;
  auto &operator[](Cs<Ids...>)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    auto  out=std::invoke(g_,get_from(Xs{},par...)...);
    return std::apply([](auto&&...v){return vector_space(x_i(Ids{},std::move(v))...);}, std::move(out));
  }
  F(all<Cs<Ids...>> ,G&& g, Xs&&...):g_{std::move(g)}{}
};

*/

    template< class Id,class G, class... Xs>
    struct extract_function_Id<F<Id,G,Xs...>>{using type=Cs<Id>;};

template< class Id,class IdOld>
struct extract_function_Id<View<Id,IdOld>>{using type=Cs<Id>;};

template< class Id,class G,class Xrandom, class... Xs>
struct extract_function_Id<Fr<Id,G,Xrandom,Xs...>>{using type=Cs<Id>;};

template <class Id, class F, class Distribution, class G, class ...Xs >
struct extract_function_Id<Sampler<Id,F,Distribution,G,Xs...>>{using type=Cs<Id>;};


template< class Id,class G, class... Xs>
struct extract_function_Id<F<all<Id>,G,Xs...>>{using type=Cs<Id>;};
template< class Id,class G,class Xrandom, class... Xs>
struct extract_function_Id<Fr<all<Id>,G,Xrandom,Xs...>>{using type=Cs<Id>;};

template< class Id,class G, class Xrandom, class... Xs>
struct extract_function_Id<Fr<pos<Id>,G,Xrandom,Xs...>>{using type=Cs<Id>;};


template< class Id,class G, class... Xs>
struct extract_function_Id<Fq<Id,G,Xs...>>{using type=Cs<Id>;};
template< class Id,class G, class Xrandom,class... Xs>
struct extract_function_Id<Frq<Id,G,Xrandom,Xs...>>{using type=Cs<Id>;};

template< class Id,class G, class... Xs>
struct extract_function_Id<Fq<all<Id>,G,Xs...>>{using type=Cs<Id>;};
template< class Id,class G,class Xrandom, class... Xs>
struct extract_function_Id<Frq<all<Id>,G,Xrandom,Xs...>>{using type=Cs<Id>;};

template< class Id,class G, class Xrandom,class... Xs>
struct extract_function_Id<Frq<pos<Id>,G,Xrandom,Xs...>>{using type=Cs<Id>;};




template< class Id,class G, class... Xs>
struct extract_function_Id<Coord<all<Id>,G,Xs...>>{using type=Cs<Id>;};





template<class Id, class G, class...Xs> F(Id&&,G&&,Xs&&...)->F<Id,G,Xs...>;

template<class e_i, class Value_type>
struct f_i
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  using element_type= Value_type ;


  using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;


private:
  Value_type value_;
public:

  explicit f_i(const value_type& x):value_{x}{}
  explicit f_i(value_type&& x):value_{std::move(x)}{}
  explicit f_i(e_i,value_type&& x):value_{std::move(x)}{}
  explicit f_i(e_i,const value_type& x):value_{x}{}


  f_i()=default;
  f_i const& operator[](e_i)const & {return *this;}
  f_i& operator[](e_i) & {return *this;}


  f_i operator[](e_i)&& {return *this;}

  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}





  auto& operator()()const &{ return value_;}
  auto& operator()()&{ return value_;}
  //auto operator()()&& {return value_;}


  friend auto begin(const f_i& me){ return begin(me());}

  static constexpr auto begin() {return Position<>{};}
  static constexpr auto rec_begin() {return Position<>{};}


  constexpr bool next(Position<>& )const {return false;}

  template<class Position>
  auto& operator()(const Position& ){ return (*this);}

  template<class Position>
  auto& operator()(const Position& )const { return (*this);}

};


template<class e_i, class Value_type>
struct f_i_view_const
{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef e_i ei;



  typedef e_i myId;

  using element_type= Value_type ;


  using element_xi= x_i<e_i,element_type>;

  typedef  Value_type value_type;


private:
  Value_type const& value_;
public:

  explicit f_i_view_const(const value_type& x):value_{x}{}
  explicit f_i_view_const(e_i,const value_type& x):value_{x}{}


  f_i_view_const const& operator[](e_i)const  {return *this;}
  f_i_view_const& operator[](e_i) {return *this;}

  friend auto begin(const f_i_view_const& me){ return begin(me());}


  auto& get(e_i)const  {return *this;}
  auto& get(e_i)  {return *this;}





  auto& operator()()const { return value_;}



  template<class Position>
  auto& operator()(const Position& p){ return value_(p);}

  template<class Position>
  auto& operator()(const Position& p)const { return value_(p);}

};




template<bool Complete,class... Ds, class Id2, class Value_type>
Is_Complete<Complete,vector_space<Ds...,f_i<Id2,Value_type>>>
operator &&
    (Is_Complete<Complete,vector_space<Ds...>>&& d, f_i<Id2,Value_type>&& d2)
{
  //  using test=typename Id2::test_this_Id;
  return Is_Complete(std::bool_constant<Complete>{},
                     vector_space<Ds...,f_i<Id2,Value_type>>(std::move(d.value)[typename Ds::myId{}]...,std::move(d2)));
}

template<class... > struct quimulun;

template<class...Fs > struct  quimulun: public Fs...{
  using Fs::operator[]...;
  using myIds= Cs<typename Fs::myId...>;
  
  quimulun(Fs&&...f):Fs{std::move(f)}...{}
  quimulun(const Fs&...f):Fs{f}...{}

  friend constexpr auto begin(const quimulun& ){ return Position<>{};}

  static constexpr auto begin() {return Position<>{};}
  constexpr bool next(Position<>& )const {return false;}

  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const { return *this;}





  template<class andId>
  Nothing operator[](andId)const
  {
    return Nothing{};
  }

};




template<class...Fs >   quimulun(Fs&&...) -> quimulun<Fs...>;
template< class...Fs >   quimulun(const Fs&...) -> quimulun<Fs...>;

template<class... Ids> struct myselect{ static constexpr auto size=sizeof... (Ids);};
template<class... Ids> struct myremove{};

template<class... Ids> struct myselect_for_data{};




template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myselect<Cs<Ids...>>)
{
  return q|myselect<Ids...>{};
}
template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myselect<Ids...>)
{
  return quimulun(q[Ids{}]...);
}
template<class...Fs, class ...Ids >
auto operator | (const quimulun<Fs...>& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<Ids...>>,myselect<>>;
  return q|sel{};
}

template<class...Fs, class ...Ids >
auto operator | ( quimulun<Fs...>&& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<Ids...>>,myselect<>>;
  return std::move(q)|sel{};
}




template<class...Fs, class ...Ids >
auto operator | ( quimulun<Fs...>&& q, myselect<Ids...>)
{
  return quimulun(std::move(q)[Ids{}]...);
}



template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...>&& d, myselect<Ids...>)
{
  return vector_space{std::move(d)[Ids{}]...};
}

template<class... Ds, class...Ids>
auto operator | (const vector_space<Ds...>& d, myselect<Ids...>)
{
  // using test=typename Cs<decltype (d[Ids{}])...>::test;
  return vector_space{d[Ids{}]...};
}



template<class... Ds, class...Ids>
auto operator | (vector_space<Ds...>&& d, myselect<Cs<Ids...>>)
{
  return std::move(d)|myselect<Ids...>{};
}




template<class... Ds, class...Ids>
auto operator | (const vector_space<Ds...>& d, myselect<Cs<Ids...>>)
{
  return d|myselect<Ids...>{};
}






template<class...Ds, class ...Ids >
auto operator | (const vector_space<Ds...>& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Ds::myId...>,Cs<Ids...>>,myselect<>>;
  return q|sel{};
}

template<class...Ds, class ...Ids >
auto operator | ( vector_space<Ds...>&& q, myremove<Ids...>)
{
  using sel=transfer_t<pack_difference_t<Cs<typename Ds::myId...>,Cs<Ids...>>,myselect<>>;
  return std::move(q)|sel{};
}





template<class...Fs, class...Fs2>
auto operator + (const quimulun<Fs...>& q1,const quimulun<Fs2...>& q2)
    ->std::enable_if_t<!(is_in_pack(typename Fs2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Fs2...>>
{
  return quimulun(q1[typename Fs::myId{}]...,q2[typename Fs2::myId{}]...);
}

template<class...Fs, class...Ids2>
auto operator + (const quimulun<Fs...>& q1,const vector_space<Ids2...>& q2)
    ->std::enable_if_t<!(is_in_pack(typename Ids2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Ids2...>>
{
  return quimulun(q1[typename Fs::myId{}]...,q2[typename Ids2::myId{}]...);
}
template<class...Fs, class...Ids2>
auto operator + (const vector_space<Ids2...>& q2,const quimulun<Fs...>& q1)
    ->std::enable_if_t<!(is_in_pack(typename Ids2::myId{},Cs<typename Fs::myId...>{})&&...),quimulun<Fs...,Ids2...>>
{
  return quimulun(q2[typename Ids2::myId{}]...,q1[typename Fs::myId{}]...);
}


template< class...Fs,  class...Fs2>
auto operator << (const quimulun<Fs...>& q1,const quimulun<Fs2...>& q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,myselect<>> non_common_Ids ;
  return (q1|non_common_Ids{})+q2;
}

template<class...Fs, class...Fs2>
auto operator << (vector_space<Fs...> q1,vector_space<Fs2...> q2)
{
  typedef transfer_t<pack_difference_t<Cs<typename Fs::myId...>,Cs<typename Fs2::myId...>>,myselect<>> non_common_Ids ;
  return (std::move(q1)|non_common_Ids{})+std::move(q2);
}


template <class...Ids,class...Ids2>
constexpr  bool test(Cs<Ids...>,Cs<Ids2...>)
{
  return ((is_in_pack<Ids,Ids2...>()&&...));
}



template <class ...Ups, class ...Downs, class value_one,
          class ...Ups2, class ...Downs2, class value_two>
auto operator + (const x_i<logpr<up<Ups...>,dn<Downs...>>,value_one>& one,
               const x_i<logpr<up<Ups2...>,dn<Downs2...>>,value_two>& two)

{
  return x_i(logpr<up<Ups...>,dn<Downs...>>{}+logpr<up<Ups2...>,dn<Downs2...>>{},one()+two());
};





template <class... Ids_Not_Reachead>
constexpr bool check_if_reacheable(Cs<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
  return false;
}

template <class... Ids_Not_Reachead>
constexpr bool check_if_reacheable(Error<Ids_Not_Reachead...>)
{
  static_assert(sizeof... (Ids_Not_Reachead)==0,"some Ids are Not Reacheable");
  return false;
}




template<class Id, class Start, class Step, class End>
class Recursive
{
private:
  Start start_;
  Step step_;
  End end_;

public:
  Recursive(Id&&, Start start, Step step, End end):start_{std::move(start)},step_{std::move(step)}, end_{std::move(end)}{}

};














//} // namespace qm




#endif // QM_TENSOR_MODEL_H
