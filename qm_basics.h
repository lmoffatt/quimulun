#ifndef QM_BASICS_H
#define QM_BASICS_H

#include "qm_pack_operations.h"
#include "qm_static_string.h"
#include <variant>

struct Nothing{};


inline Nothing operator+(Nothing, Nothing ){return Nothing{};}

inline Nothing operator||(Nothing, Nothing ){return Nothing{};}

template<class Something>
auto operator||(Nothing, Something&& s){return std::forward<Something>(s);}

template<class Something>
auto operator||( Something&& s, Nothing){return std::forward<Something>(s);}



struct logP_zero{
  constexpr static auto  className=my_static_string("logP_is_zero");
};

template<class T>
auto operator+(T&& x,logP_zero){return std::forward<T>(x);}
template<class T>
auto operator+( logP_zero,T&& x){return std::forward<T>(x);}
inline auto operator+(logP_zero ,logP_zero ){return logP_zero {};}

template<class T>
auto operator*( logP_zero,const T& ){return logP_zero{};}
template<class T>
auto operator/( logP_zero,const T& ){return logP_zero{};}
template<class T>
auto operator*( const T& ,logP_zero){return logP_zero{};}
inline auto operator*(logP_zero,logP_zero){return logP_zero{};}




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
  using innerId=Id;
  auto& operator[](Id)const {return *this;}
  auto& operator[](Id) {return *this;}
  auto& operator()()const {return  i;}
  auto& operator()() {return  i;}

  auto& operator++(){ ++i; return *this;}
  std::size_t inc(Id)
  {
    ++i;
    return i;
  }
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
  using Position<Id>::operator[];
  using Position<Ids>::operator[]...;
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


template <class anId, class...Datas>
auto get_from(const Datas&...ds)
{
  return (ds[anId{}]||...);
}







#endif // QM_BASICS_H
