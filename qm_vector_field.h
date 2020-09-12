#ifndef QM_VECTOR_FIELD_H
#define QM_VECTOR_FIELD_H

#include "qm_vector_device.h"
#include "qm_vector_index.h"
#include "qm_vector_position.h"



struct Map{

};

template<class , class> class v_f;

template<class Value_type,class...  Idxs>
class v_f<index_prod<Idxs...>,Value_type>
{
public:
  typedef Value_type element_type;

  typedef vector_device<element_type> value_type;


   static inline constexpr const auto name_ext=(Idxs::name+...);


private:
  value_type value_;

public:
  __host__ __device__  constexpr  auto& value(){return value_;}


  __host__ __device__  constexpr auto& value()const {return value_;}


  static  __host__ __device__  constexpr auto index() {return index_prod<Idxs...>{};}

  template<class index_table>
  __host__ __device__  constexpr explicit v_f(const index_table& x):value_(index_prod<Idxs...>{}.size(x)){}


  constexpr v_f()=default;

  template<class Id>
 __host__ __device__  constexpr auto operator[](Id)
  {
      if constexpr (std::is_same_v<Nothing,decltype (std::declval<Value_type&>()[Id{}]) >)
              return Nothing{};
      else  *this;
  }


 template<class Id>
__host__ __device__  constexpr auto operator[](Id) const
 {
     if constexpr (std::is_same_v<Nothing,decltype (std::declval<Value_type const&>()[Id{}]) >)
             return Nothing{};
     else  *this;
 }


  __host__ __device__ constexpr  auto size()const{return value().size();}

  constexpr auto byte_size()const {return value().byte_size() ;}


  friend
      __host__ __device__    void my_print(const v_f& me)
  {
    printf("{");
    for (auto& e:me.value()) { my_print(e);printf(", ");}
    printf("}");
  }




};


template<class Id, class...> class x_i_view;



template<class Id, class vector_field, class ...Indexes>
class x_i_view<Id,index_table<Indexes...>,vector_field>
{
private:
    index_table<Indexes...> const* i_;
    vector_field * vf_;

public:
    x_i_view(Id,const index_table<Indexes...>& i,vector_field & vf): i_{&i},vf_{&vf}{}

    template<class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const Position<Xs...>& p)const
      { return vf_->value()[vf_->index()(*i_,p)()][Id{}];}

    template<class...Xs, typename=std::enable_if_t<!std::is_const_v<vector_field>>>
    __host__ __device__ constexpr    decltype(auto) operator()(const Position<Xs...>& p)
     { return vf_->value()[vf_->index()(*i_,p)()][Id{}];}

};

template<class Id, class Value_type,class...  Idxs, class... Indexes>
x_i_view(Id,const index_table<Indexes...>&, v_f<index_prod<Idxs...>,Value_type>&)-> x_i_view<Id,index_table<Indexes...>,v_f<index_prod<Idxs...>,Value_type>>;

template<class Id, class Value_type,class...  Idxs, class... Indexes>
x_i_view(Id,const index_table<Indexes...>&, v_f<index_prod<Idxs...>,Value_type> const&)-> x_i_view<Id,index_table<Indexes...>,const v_f<index_prod<Idxs...>,Value_type>>;

template<class Id, class Value_type,class...  Idxs, class ...Indexes>
struct element_type<x_i_view<Id, index_table<Indexes...>,v_f<index_prod<Idxs...>,Value_type>>>
{
    using type=std::decay_t<decltype (std::declval<Value_type&>()[Id{}]())>;
};

template<class Id, class Value_type,class...  Idxs, class ...Indexes>
struct element_type<x_i_view<Id, index_table<Indexes...>,const v_f<index_prod<Idxs...>,Value_type>>>
{
    using type=std::decay_t<decltype (std::declval<Value_type&>()[Id{}]())>;
};



template<class Id, class value>
auto  span(const x_i<Id,value>&)
{
    return index_prod<>{};
};


template<class Id, class Value_type,class...  Idxs, class ...Indexes>
auto span(x_i_view<Id, index_table<Indexes...>,v_f<index_prod<Idxs...>,Value_type>>)
{
    return index_prod<Idxs...>{};
};

template<class Id, class Value_type,class...  Idxs, class ...Indexes>
auto span(x_i_view<Id, index_table<Indexes...>,const v_f<index_prod<Idxs...>,Value_type>>)
{
    return index_prod<Idxs...>{};
};


#endif // QM_VECTOR_FIELD_H
