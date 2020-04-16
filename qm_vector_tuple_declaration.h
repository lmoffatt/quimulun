#ifndef QM_VECTOR_TUPLE_DECLARATION_H
#define QM_VECTOR_TUPLE_DECLARATION_H

#include "qm_vector_space_declaration.h"
#include "qm_vector_field_declaration.h"









template<class...v_is> class vector_tuple
{
private:
  std::tuple<v_is...> v_;

public:
  using  self_type=vector_tuple;

  //  typedef Cs<typename v_is::ei...> my_eis;
  using myIds= pack_concatenation_t<myIds_t<v_is>...>;
  //  typedef Cs<v_is...> myx_is;
  //  using myIndex=Index<typename v_is::ei...>;

  auto& getTuple(){return v_;}
  auto & getTuple()const {return v_;}



  template<class Position>
  auto const & operator()(const Position&) const {return *this;}


  template<class andId>
  auto operator[](andId)const
  {
    return std::apply([](auto const&... t){return (get_at(t, andId{})||...||Nothing{});}, v_);
  }


  friend auto operator + (const vector_tuple& one, const vector_tuple& two)
  {
    return vector_tuple(std::get<v_is>(one.v_)+std::get<v_is>(two.v_)...);
  }

  template<class value>
  friend auto operator *(const vector_tuple& one, const value& a)
  {
    return vector_tuple<std::decay_t<decltype (std::declval<v_is>()*a)>...>(std::get<v_is>(one.v_)*a...);
  }

  friend auto operator - (const vector_tuple& one, const vector_tuple& two)
  {
     return vector_tuple<std::decay_t<decltype (std::declval<v_is>()-std::declval<v_is>())>...>(std::get<v_is>(one.v_)-std::get<v_is>(two.v_)...);
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


