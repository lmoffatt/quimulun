#ifndef QM_BASICS_H
#define QM_BASICS_H

#include "qm_pack_operations.h"
#include "static_string.h"
#include <variant>

struct Nothing{
  friend Nothing operator+(Nothing, Nothing ){return Nothing{};}


  friend Nothing operator||(Nothing, Nothing ){return Nothing{};}

  template<class Something>
  friend auto operator-( Something&& s, Nothing)->decltype (s)
  {return std::forward<Something>(s);}

  template<class Something>
  friend auto operator+( Something&& s, Nothing)->decltype (s)
  {return std::forward<Something>(s);}

  template<class Something>
  friend auto operator||(Nothing, Something&& s)->decltype (s)
  {return std::forward<Something>(s);}
  template<class Something>
  friend auto operator||( Something&& s, Nothing)->decltype (s)
  {return std::forward<Something>(s);}

  constexpr auto size(){return 0;}
  using myIds=Cs<>;

};


template<class... Ids>
struct Not_found{
  template<class... Id2>
  friend auto operator+(Not_found, Not_found<Id2...> ){return Not_found<Ids...,Id2...>{};}


  template<class... Id2>
  friend auto operator||(Not_found, Not_found<Id2...> ){return Not_found<Ids...,Id2...>{};}

  template<class Something>
  friend auto operator-( Something&& s, Not_found)->decltype (s)
  {return std::forward<Something>(s);}

  template<class Something>
  friend auto operator+( Something&& s, Not_found)->decltype (s)
  {return std::forward<Something>(s);}

  template<class Something>
  friend auto operator||(Not_found, Something&& s)->decltype (s)
  {return std::forward<Something>(s);}
  template<class Something>
  friend auto operator||( Something&& s, Not_found)->decltype (s)
  {return std::forward<Something>(s);}


};




struct Something{  constexpr bool operator==(Something)const {return true;}
};






struct logP_zero{
  constexpr static auto  className=my_static_string("logP_is_zero");
  template<class T>
  friend auto operator+(T&& x,logP_zero){return std::forward<T>(x);}
  template<class T>
  friend auto operator+( logP_zero,T&& x){return std::forward<T>(x);}
  friend inline auto operator+(logP_zero ,logP_zero ){return logP_zero {};}

  template<class T>
  friend auto operator*( logP_zero,const T& ){return logP_zero{};}
  template<class T>
  friend auto operator/( logP_zero,const T& ){return logP_zero{};}
  template<class T>
  friend auto operator*( const T& ,logP_zero){return logP_zero{};}
  friend  auto operator*(logP_zero,logP_zero){return logP_zero{};}
};





template <class...> struct CycleIndex;

template<class Id0, class Id>struct CycleIndex<Id0,Id>
{
  inline constexpr auto next(Id){return Id0{};}

};



template <class Id0, class Id,class Idnext,class... Ids> struct CycleIndex<Id0,Id,Idnext,Ids...>:CycleIndex<Id0,Idnext,Ids...>
{
  using CycleIndex<Id0,Idnext,Ids...>::next;
  inline constexpr auto next(Id){return Idnext{};}

  template<class anId>
  auto operator()(anId)->std::enable_if_t<is_in_pack<anId,Id,Idnext,Ids...>(),std::variant<Id,Idnext,Ids...>>
  {
    return std::variant<Id,Idnext,Ids...>{next(anId{})};
  }

};




template <class... Ids> struct Index{};


template<class Id, class... Ids>
bool next_variant ( std::variant<Id, Ids...>& v)
{
  v=std::visit([](auto e){return std::variant<Id,Ids...>{CycleIndex<Id,Id,Ids...>{}.next(e)};},v );
  return v.index()!=0;
}





template <class...> struct Position;

template< >struct Position<>{};

template <class Id> struct Position<Id>{
private :
  std::size_t i;
public:
  static constexpr auto className=my_static_string("pos_")+Id::className;
  Position(std::size_t i_v):i{i_v}{}
  using innerId=Id;
  auto& operator[](Id)const {return *this;}
  auto& operator[](Id) {return *this;}
  auto& operator()()const {return  i;}
  auto& operator()() {return  i;}
  Position()=default;
  auto& operator++(){ ++i; return *this;}
  std::size_t inc(Id)
  {
    ++i;
    return i;
  }
  friend std::ostream& operator<<(std::ostream& os, const Position& me) {
    os<<className.c_str()<<"="<<me();
    return os;}
  friend std::istream& operator>>(std::istream& is, Position& me){ is>>className>>my_static_string("=")>>me(); return is;}

};



template <class... Ids> struct Position<Index<Ids...>>{
private :
  std::variant<Ids...> i;
public:
  auto& operator[](Index<Ids...>)const {return *this;}
  auto& operator[](Index<Ids...>) {return *this;}
  auto& operator()()const {return  i;}
  auto& operator()() {return  i;}
  auto& operator++(){ next_variant(i); return *this;}


};



template <class id0, class id_in> struct recursive{
  constexpr static auto  className=id0::className+my_static_string("_")+id_in::className;
  using type=recursive;
};

template <class id0, class... id_in> struct recursive<id0,Cs<id_in...>>{
  using type=Cs<recursive<id0,id_in>...>;
};


template <class id0, class id_in>
using recursive_t=typename recursive<id0,id_in>::type;



template <class Id, class... Ids> struct Position<Id,Ids...>:Position<Id>,Position<Ids>...
{
  static constexpr auto className=(Position<Id>::className+...+Position<Ids>::className);

  using Position<Id>::operator[];
  using Position<Ids>::operator[]...;
  friend std::ostream& operator<<(std::ostream& os, const Position& me) {
    os<<me[Id{}];
    if constexpr (sizeof... (Ids)>0)
      ((os<<"_"<<me[Ids{}]),...);
    return os;
  }

};


template<class ...Ids,class IdOther>
auto operator && (Position<Ids...>&& me, Position<IdOther>&&  )
    ->std::enable_if_t<is_in_pack<IdOther,Ids...>(),Position<Ids...>>
{
  return std::move(me);
}

template<class ...Ids>
auto operator && (Position<Ids...>&& me, Position<>  ){return std::move(me);}


template<class ...Ids,class IdOther>
auto operator && (Position<Ids...> , Position<IdOther>  )
    ->std::enable_if_t<!is_in_pack<IdOther,Ids...>(),Position<Ids...,IdOther>>
{
  return Position<Ids...,IdOther>{};
}

template<class ...Ids,class... Is>
auto operator && (Position<Ids...>&& me, Position<Is...>&&  )
{
  return (std::move(me)&&...&&Position<Is>{});
}



template<bool complete,class Data>
struct Is_Complete{
  Data value;
  typedef Data type;
  static constexpr bool is_complete=complete;
  Is_Complete(std::bool_constant<complete>,Data&& d):value{std::move(d)}{}
  Is_Complete()=default;

};

template<bool Complete, class Data> Is_Complete(std::bool_constant<Complete>,Data&&)->Is_Complete<Complete,Data>;





template<typename T>
auto& center(const T& x){ return x;}

template<class e_i, class Value_type>
struct x_i;

template<class e_i, class Value_type>
struct x_i_view_const;

template<class e_i, class Value_type>
struct x_i_view_non_const;


template<class e_i, class Value_type>
struct f_i;

template<class e_i, class Value_type>
struct f_i_view_const;



template<class C>
    auto only_xi_or_fi(C&& x)->std::conditional_t<
    (is_this_template_class<x_i,std::decay_t<C>>::value||
     is_this_template_class<x_i_view_const,std::decay_t<C>>::value||
     is_this_template_class<x_i_view_non_const,std::decay_t<C>>::value||
     is_this_template_class<f_i_view_const,std::decay_t<C>>::value||
     is_this_template_class<f_i,std::decay_t<C>>::value),
    decltype(x),Nothing>
{
  if constexpr (is_this_template_class<x_i,std::decay_t<C>>::value||
                is_this_template_class<x_i_view_const,std::decay_t<C>>::value||
                is_this_template_class<x_i_view_non_const,std::decay_t<C>>::value||
                is_this_template_class<f_i_view_const,std::decay_t<C>>::value||
                is_this_template_class<f_i,std::decay_t<C>>::value)
    return std::forward<C>(x);
  else
    return Nothing{};

}


template<class C>
auto only_xi(C&& x)->std::conditional_t<
    (is_this_template_class<x_i,std::decay_t<C>>::value||
     is_this_template_class<x_i_view_const,std::decay_t<C>>::value||
     is_this_template_class<x_i_view_non_const,std::decay_t<C>>::value),
    decltype(x),Nothing>
{
  if constexpr (is_this_template_class<x_i,std::decay_t<C>>::value||
                is_this_template_class<x_i_view_const,std::decay_t<C>>::value||
                is_this_template_class<x_i_view_non_const,std::decay_t<C>>::value)
    return std::forward<C>(x);
  else
    return Nothing{};

}



template<class Id> struct all;

template <class anId, class...Datas
          , typename =std::enable_if_t< ( !is_this_template_class<all,anId>::value)&&
                                          ( !is_this_template_class<std::tuple,anId>::value) ,int>>
auto get_from(anId, Datas&&...ds)->decltype ((only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...))
{
  return (only_xi_or_fi(std::forward<Datas>(ds)[anId{}])||...);
}







#endif // QM_BASICS_H
