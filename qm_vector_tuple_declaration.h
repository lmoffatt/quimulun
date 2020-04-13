#ifndef QM_VECTOR_TUPLE_DECLARATION_H
#define QM_VECTOR_TUPLE_DECLARATION_H

#include "qm_vector_space_declaration.h"
#include "qm_vector_field_declaration.h"






template<class Id, class V>
auto get_at(const V& x, Id)->decltype (x[Id{}])
{
  return x[Id{}];
}

template<class Id, class... Xs, class...xis>
auto get_at(const vector_field<vec<Xs...>,vector_space<xis...>>& x,Id)
{
  if constexpr (std::is_same_v<decltype (std::declval<vector_space<xis...>>()[Id{}]),Nothing >)
    return Nothing{};
  else
    return x_i_vector_field_const<vector_field<vec<Xs...>,vector_space<xis...>>,Id>(x,Id{});
}





template<class...v_is> class vector_tuple
{
private:
  std::tuple<v_is...> v_;

  using  self_type=vector_tuple;

//  typedef Cs<typename v_is::ei...> my_eis;
//  typedef Cs<typename v_is::myId...> myIds;
//  typedef Cs<v_is...> myx_is;
//  using myIndex=Index<typename v_is::ei...>;





public:

  auto& getTuple(){return v_;}
  auto & getTuple()const {return v_;}




  template<class andId>
  auto operator[](andId)const
  {
    return std::apply([](auto const&... t){return (get_at(t, andId{})||...||Nothing{});}, v_);
  }


static constexpr auto size() {return sizeof... (v_is);}

explicit vector_tuple(v_is&&...xs):v_is{std::move(xs)}...{}
explicit vector_tuple(v_is const&...xs):v_is{xs}...{}



vector_tuple()=default;

};


template<> class vector_tuple<>
{
private:

  using  self_type=vector_tuple;

  //  typedef Cs<typename v_is::ei...> my_eis;
  //  typedef Cs<typename v_is::myId...> myIds;
  //  typedef Cs<v_is...> myx_is;
  //  using myIndex=Index<typename v_is::ei...>;





public:

//  auto& getTuple(){return v_;}
//  auto & getTuple()const {return v_;}



  template<class andId>
  auto operator[](andId)const
  {
    return Nothing{};
  }


  static constexpr auto size() {return 0;}

  vector_tuple(){}

};







#endif // QM_VECTOR_TUPLE_DECLARATION_H


