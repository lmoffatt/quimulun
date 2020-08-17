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

  vector_tuple()=default;

  auto& getTuple(){return v_;}
  auto & getTuple()const {return v_;}


  auto & operator()(const Position<>&)const {return *this;}

  auto & operator()(const Position<>&){return *this;}



  /**   const version
   *  @brief if all the components of the tuple return themselves upon inspected with this Position, the whole tuple will return itself.
   */
  template<class X,class... Xs, typename=std::enable_if_t<(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  decltype (auto) operator()(const Position<X,Xs...>& ) const{
    //using test=typename Cs<Position<X,Xs...>,vector_tuple>::tuple;

    return *this;
  }

  /**
   *  @brief if not all the components of the tuple return themselves upon inspected with this Position, it will create a tuple with the values at the reclaimed position
   *    */

  template<class X,class... Xs, typename=std::enable_if_t<!(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto operator()(const Position<X,Xs...>& p) const{
   // using test=typename Cs<Position<X,Xs...>,decltype(vector_tuple<std::decay_t<decltype (std::declval<v_is>()(p))>...>(std::declval<v_is>()(p)...)),vector_tuple>::tuple;
    return std::apply([&p](auto&... t){return vector_tuple<std::decay_t<decltype (t(p))>...>(t(p)...);},v_);
  }

  /**  non-const version
   *  @brief if all the components of the tuple return themselves upon inspected with this Position, the whole tuple will return itself.
   */
  template<class X,class... Xs, typename=std::enable_if_t<(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto  & operator()(const Position<X,Xs...>& ) {
  //  using test=typename Cs<Position<X,Xs...>,vector_tuple>::tuple;
    return *this;
  }

  /**
   *  @brief if not all the components of the tuple return themselves upon inspected with this Position, it will create a tuple with the values at the reclaimed position
   *    */

  template<class X,class... Xs, typename=std::enable_if_t<!(std::is_same_v<std::decay_t<decltype(std::declval<v_is>()(std::declval<Position<X,Xs...>>()))>,v_is >&&...&&true)>>
  auto operator()(const Position<X,Xs...>& p) {
  //  using test=typename Cs<Position<X,Xs...>,decltype(vector_tuple<decltype (std::declval<v_is>()(p))...>(std::declval<v_is>()(p)...)),vector_tuple>::tuple;
    return std::apply([&p](auto&... t){return vector_tuple<decltype (t(p))...>(t(p)...);},v_);
  }



  template<class andId, typename=std::enable_if_t<!is_this_template_class_v<non_const,andId>>>
  decltype(auto) operator[](andId)const//->decltype ( (get_at(std::declval<v_is const&>(), andId{})||...||Nothing{}))
  {
    return std::apply([](auto const&... t)
                          ->decltype (auto)
                      {
                        return (get_at(t, andId{})||...||Nothing{});
//                        auto&& out=(get_at(t, andId{})||...||Nothing{});
//                        return out;
                      }, v_);
  }

  template<class andId>
  decltype(auto) operator[](non_const<andId>)//->decltype ( (get_at(std::declval<v_is&>(), andId{})||...||Nothing{}))
  {
    return std::apply([](auto&... t)
                          ->decltype (auto)
                      {return (get_at(t, non_const<andId>{})||...||Nothing{});}, v_);
  }


  /**
   * consider removing operators after we found the way of using gpus for parallelizing everything, as now they are needed
   */
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

   // using test=typename Cs<Cs<decltype (std::declval<v_is>())...>,value>::test;

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

  template<class... V_is, typename=std::enable_if_t<(std::is_same_v<V_is,std::decay_t<v_is>> && ...) >>
  explicit vector_tuple(V_is&&...xs):v_{std::forward<V_is>(xs)...}{}

  explicit vector_tuple(v_is const&...xs):v_{xs...}{}


  auto begin(){return Position<>{};}


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




template<class... xis>
struct get_Field_Indexes <vector_tuple<xis...>>
{
  typedef vec<> type;
};




#endif // QM_VECTOR_TUPLE_DECLARATION_H


