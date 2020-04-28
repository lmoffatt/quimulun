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


  auto & operator()(const Position<>&)const {return *this;}

  auto & operator()(const Position<>&){return *this;}


  template<class X,class... Xs, typename=std::enable_if_t<(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto  & operator()(const Position<X,Xs...>& ) const{
    return *this;
  }

  template<class X,class... Xs, typename=std::enable_if_t<!(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto operator()(const Position<X,Xs...>& p) const{
    return std::apply([&p](auto&... t){return vector_tuple<std::decay_t<decltype (t(p))>...>(t(p)...);},v_);
  }

  template<class X,class... Xs, typename=std::enable_if_t<(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto  & operator()(const Position<X,Xs...>& ) {
    return *this;
  }

  template<class X,class... Xs, typename=std::enable_if_t<!(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto operator()(const Position<X,Xs...>& p) {
    return std::apply([&p](auto&... t){return vector_tuple<std::decay_t<decltype (t(p))>...>(t(p)...);},v_);
  }



  template<class andId, typename=std::enable_if_t<!is_this_template_class_v<non_const,andId>>>
  auto operator[](andId)const//->decltype (std::apply([](auto const&... t){return (get_at(t, andId{})||...||Nothing{});}, v_))
  {
    return std::apply([](auto const&... t){return (get_at(t, andId{})||...||Nothing{});}, v_);
  }

  template<class andId>
  auto operator[](andId)
  {
    return std::apply([](auto&... t){return (get_at(t, andId{})||...||Nothing{});}, v_);
  }


  friend auto operator + (const vector_tuple& one, const vector_tuple& two)
  {
    return vector_tuple(std::get<v_is>(one.v_)+std::get<v_is>(two.v_)...);
  }
  friend auto operator + (vector_tuple&& one, vector_tuple&& two)
  {
    return vector_tuple(std::get<v_is>(std::move(one.v_))+std::get<v_is>(std::move(two.v_))...);
  }

  template<class value>
  friend auto operator *(const vector_tuple& one, const value& a)
  {
    return vector_tuple<std::decay_t<decltype (std::declval<v_is>()*a)>...>(std::get<v_is>(one.v_)*a...);
  }



  template<class... v_is2>
  friend auto operator +(const vector_tuple& one, const vector_tuple<v_is2...>& two)
  {
    return vector_tuple<std::decay_t<decltype (std::declval<v_is>()+std::declval<v_is2>())>...>(std::get<v_is>(one.v_)+std::get<v_is2>(two.getTuple())...);
  }

  template<class... v_is2>
  friend auto operator - (const vector_tuple& one, const vector_tuple<v_is2...>& two)->vector_tuple<std::decay_t<decltype (std::declval<v_is>()-std::declval<v_is2>())>...>
  {
    return vector_tuple<std::decay_t<decltype (std::declval<v_is>()-std::declval<v_is2>())>...>(std::get<v_is>(one.v_)-std::get<v_is2>(two.v_)...);
  }




  static constexpr auto size() {return sizeof... (v_is);}

  explicit vector_tuple(v_is&&...xs):v_{std::move(xs)...}{}
  explicit vector_tuple(v_is const&...xs):v_{xs...}{}



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


