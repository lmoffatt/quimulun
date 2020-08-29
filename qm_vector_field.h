#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H

#include "qm_vector_device.h"
#include "qm_vector_index.h"
#include "qm_vector_position.h"


template<class , class> class v_f;

template<class Value_type,class...  Idxs>
class v_f<index_prod<Idxs...>,Value_type>
{
public:
  typedef Value_type element_type;

  typedef vector_device<element_type> value_type;


   static inline constexpr const auto name_ext=(Idxs::name+...);


  template<class Id>
  struct at_Id
  {
    v_f* v_;

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)
    { return v_->value()[index()(x,p)()][Id{}];}

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[index()(x,p)()][Id{}];}

    constexpr at_Id(Id,v_f& v):v_(&v){}

  };

  template<class Id>
  struct at_Id_const
  {
    v_f const* v_;


    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[index()(x,p)()][Id{}];}

    at_Id_const(Id,v_f const& v):v_(&v){}

  };



private:
  value_type value_;

public:
  __host__ __device__  constexpr  auto& value(){return value_;}


  __host__ __device__  constexpr auto& value()const {return value_;}


  static  __host__ __device__  constexpr auto index() {return index_prod<Idxs...>{};}

  template<class index_table>
  __host__ __device__  constexpr explicit v_f(const index_table& x):value_{index_prod<Idxs...>{}.size(x)}{}


  template<class Id, typename=decltype (std::declval<element_type>()[Id{}])>
  constexpr auto operator[](Id)
  {
    return at_Id<Id>(*this);
  }

  template<class Id, typename=decltype (std::declval<element_type>()[Id{}])>
constexpr   auto operator[](Id) const
  {
    return at_Id_const<Id>(*this);
  }






  //__host__ __device__
  constexpr v_f()=default;

  __host__ __device__ constexpr  auto size()const{return value().size();}

  constexpr auto byte_size()const {return value().byte_size() ;}

  template<class index_table,class...Xs>
  __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
  { return value()[index()(x,p)()];}

  template<class index_table,class...Xs>
  __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)
  { return value()[index()(x,p)()];}


  template<class index_table,class...Xs, class Id, typename= decltype (std::declval<element_type>()[Id{}])>
  __host__ __device__ constexpr    decltype(auto) at_position_Id(const index_table& x,const Position<Xs...>& p, Id)const
  { return value()[index()(x,p)()][Id{}];}

  template<class index_table,class...Xs, class Id, typename= decltype (std::declval<element_type>()[Id{}])>
  __host__ __device__ constexpr    decltype(auto) at_position_Id(const index_table& x,const Position<Xs...>& p, Id)
  { return value()[index()(x,p)()][Id{}];}




  friend
      __host__ __device__    void my_print(const v_f& me)
  {
    printf("{");
    for (auto& e:me.value()) { my_print(e);printf(", ");}
    printf("}");
  }




};





#endif // QM_VECTOR_FIELD_H
