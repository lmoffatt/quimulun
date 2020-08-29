#ifndef QM_VECTOR_INDEX_H
#define QM_VECTOR_INDEX_H


#include "qm_vector_device.h"
#include "qm_vector_basis.h"
#include "qm_vector_position.h"

template<class... Id>
class offset
{
private:
  std::size_t n_;

public:
  __host__ __device__
      constexpr offset(std::size_t n):n_{n}{}
  __host__ __device__
      constexpr auto operator()()const {return n_;}
  template<class... Id1>
  __host__ __device__
      friend constexpr offset<Id...,Id1...> operator*(offset one,offset<Id1...> two) {return one()*two();}
};





class ind_size
{
private:
  std::size_t n_;
public:

  inline static constexpr auto ind_name=my_static_string("ind_size");

  inline static constexpr auto name=ind_name;

  __host__ __device__   constexpr explicit ind_size(std::size_t n):n_{n}{}

  ind_size()=default;

  __host__ __device__  constexpr auto& size()const  {return n_;}

  __host__ __device__  constexpr auto& size()  {return n_;}

};


template <class , class > class ind;

template<class Id>
class ind<Id,ind_size> : public ind_size
{
public:

  inline static constexpr auto ind_name=my_static_string("ind(");

  inline static constexpr auto name=ind_name+Id::name+my_static_string(")");

  __host__ __device__   constexpr explicit ind(Id, std::size_t n):ind_size{n}{}

  __host__ __device__  constexpr auto& operator[](Id)const  {return *this;}

  __host__ __device__  constexpr auto& operator[](Id) {return *this;}


  constexpr ind()=default;
  using ind_size::size;

  template<class index_table, class Pos>
  __host__ __device__ constexpr auto operator()(const std::pair<const index_table&, const Pos&>& p)const {
    return std::pair<offset<Id>, const std::pair<const index_table&, const Pos&>&>(p[Id{}](),p);
  }

  template<class index_table,class... Id0,class Pos>
  __host__ __device__   constexpr auto operator()(std::pair<offset<Id0...>, const std::pair<const index_table&, const Pos&>&> p)const
  {
    return std::pair<offset<Id0...,Id>, const std::pair<const index_table&, const Pos&>&>(
        p.first()*size()+ p.second.second[Id{}](),
        p.second);
  }
  friend std::ostream& operator<<(std::ostream& os, const ind& me){os<<me.size(); return os;}
  friend std::istream& operator>>(std::istream& is, ind& me){is>>me.size(); return is;}


};




template<class Idx,class... Idxs>
class index_prod
{


public:


  __host__ __device__    constexpr index_prod(Idx, Idxs...){}

  __host__ __device__    constexpr index_prod()=default;

  template< class index_table>
  __host__ __device__    constexpr auto size(const index_table& itable)const {
    if constexpr ((!std::is_same_v<decltype (itable[Idx{}]),Nothing>&&...&& !std::is_same_v<decltype (itable[Idxs{}]),Nothing>))
         return (itable[Idx{}].size()*...*itable[Idxs{}].size());
      else
        return Nothing{};
  }

  template<class index_table,class Pos>
  __host__ __device__    constexpr auto operator()(const std::pair<index_table&,Pos&>& p)const {
    return compose(p,p.first[Idx{}],p.first[Idxs{}]...).first;
  }


};



template<class... indexes>
struct index_vector:public indexes...
{

  template<class index,class... indexs>
   static std::ostream& put(std::ostream& os,const index_vector<index,indexs...>& x)
  {
    os<<static_cast<index const&>(x);
    ((os<<sep<<static_cast<indexs const&>(x)),...);
    return os;

  }
  template<class ind0,class... inds>
  static std::ostream& put_titles(std::ostream& os,ind0,inds... )
  {
    os<<ind0::name;
    ((os<<sep<<inds::name),...);
    return os;

  }

  template<class... indexs, std::enable_if_t<sizeof... (indexs)==0>>
   static std::ostream& put(std::ostream& os,const index_vector<indexs...>& x)
  {
    return os;

  }
   template<class... indexs, std::enable_if_t<sizeof... (indexs)==0>>
    static std::ostream& put(std::ostream& os, typename index_vector<indexs...>::Titles x)
   {
     return os;

   }


  template<class index,class... indexs>
  static std::istream& get(std::istream& is, index_vector<index,indexs...>& x)
  {
    is>>static_cast<index &>(x);
    ((is>>sep>>static_cast<indexs &>(x)),...);
    return is;

  }

  template<class index,class... indexs>
  static std::istream& get_titles(std::istream& is, index,indexs...  )
  {
    is>>index::name;
    ((is>>sep>>indexs::name),...);
    return is;

  }


public:
  using indexes::operator[]...;

  static inline constexpr auto const sep=my_static_string(" ");

  struct Titles{
    friend std::ostream& operator<<(std::ostream& os,  Titles ){ return index_vector::put_titles(os,indexes{}...);}
      friend std::istream& operator>>(std::istream& is,  Titles ) { return get_titles(is,indexes{}...);}
  };

  friend class index_vector::Titles;

  static constexpr auto titles(){return Titles{};}


  friend std::ostream& operator<<(std::ostream& os, const index_vector& x)
  {
    return put(os,x);
  }

  friend std::istream& operator>>(std::istream& is,  index_vector& x)
  {
    return get(is,x);
  }


};

template <class,class> class index_field;

template<class I,class... Idxs>
class index_field<index_prod<Idxs...>,I>
{

private:
  vector_device<I> offset_;

  constexpr auto& value(){return offset_;}

constexpr   auto& value()const {return offset_;}



public:

  static inline constexpr const auto sep=vector_device<I>::sep;

  template<class Id>
  struct at_Id
  {
    index_field* v_;

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)
    { return v_->value()[(*v_)(x,p)()][Id{}];}

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[(*v_)(x,p)()][Id{}];}

    at_Id(Id,index_field& v):v_(&v){}

  };

  template<class Id>
  struct at_Id_const
  {
    index_field const* v_;


    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[(*v_)(x,p)()][Id{}];}

    at_Id_const(Id,index_field const& v):v_(&v){}

  };


  static constexpr auto index(){return index_prod<Idxs...>{};}


  template<class Id, typename=decltype (std::declval<I>()[Id{}])>
  constexpr  auto operator[](Id)
  {
    return at_Id<Id>(*this);
  }

  template<class Id, typename=decltype (std::declval<I>()[Id{}])>
  constexpr auto operator[](Id) const
  {
    return at_Id_const<Id>(*this);
  }




  inline static constexpr auto indname=my_static_string("index_fieldepends_on(");
  inline static constexpr auto name=indname+index_prod<Idxs...>::name+my_static_string(")");

  template<class index_table>
  __host__ __device__ constexpr index_field(const index_table& m, index_prod<Idxs...> i):offset_{i.size(m)}{}


  __host__ __device__ constexpr index_field(std::size_t n):offset_{n}{}


  __host__ __device__  auto size()const{return offset_.size();}

  template<class index_table>
  __host__ __device__  static auto size(const index_table& t){return index().size(t);}


  template<class index_table, class Pos>
  __host__ __device__  auto& operator[](const std::pair<const index_table&,const Pos&>& p)const  {return offset_[index_prod<Idxs...>{}(p)()];}


  friend std::ostream& operator<<(std::ostream& os, const index_field& me)
  {
      os<<typename I::Title{}<<sep;
      os<<me.value();
      return os;
  }

 friend std::istream& operator>>(std::istream& is, index_field& me)
 {
     is>>typename I::Title{}>>sep;
     is>>me.value();
     return is;
 }


};


//template<class Id,class... Idxs>
//class ind<Id,index_field<index_prod<Idxs...>>>
//{

//private:
//  index_field<index_prod<Idxs...>> i_;

//  auto& index(){return i_;}
//  auto& index()const {return i_;}

//  template<class, class>
//  struct index_offset;

//  template<class index_table, class T>
//  using index_offset_t=typename index_offset<index_table,T>::type;

//  template<class index_table,class Id1>
//  struct index_offset<index_table,ind<Id1,ind_size>>
//  {
//    using type=offset<Id1>;
//  };

//  template<class index_table,class Id1, class... Idxts>
//  struct index_offset<index_table,ind<Id1,index_field<index_prod<Idxts...>>>>
//  {
//    using type=decltype((index_offset_t<index_table,std::decay_t<decltype(std::declval<index_table>()[Idxts{}])>>{}*...*offset<Id1>{}));
//  };


//public:

//  inline static constexpr auto indname=my_static_string("index(");
//  inline static constexpr auto name=indname+Id::name+my_static_string(")");

//  template<class index_table>
//  __host__ __device__ constexpr ind(const index_table& m, index_prod<Idxs...> i):i_{m,i}{}

//  __host__ __device__  auto size()const{return index().size();}




//  template< class Pos>
//  __host__ __device__  auto& operator[](const Pos& p) {return i_[p];}
//  template< class Pos>
//  __host__ __device__  auto& operator[](const Pos& p)const  {return i_[p];}

//  __host__ __device__  auto& operator[](Id)const  {return *this;}

//  __host__ __device__  auto& operator[](Id) {return *this;}


//  template<class index_table,class Pos>
//  __host__ __device__  constexpr auto operator()(std::pair<const index_table&,const Pos&>  p)const {
//    return std::pair<index_offset_t<index_table,ind>, std::pair<const index_table&,const Pos&>>(i_[p]+p.second[Id{}](),p);
//  }


//  template<class... Id0,class index_table,class Pos>
//  __host__ __device__   constexpr auto operator()(std::pair<offset<Id0...>, std::pair<const index_table&,const Pos&>> p)const
//  {
//    return std::pair<decltype(offset<Id0...>{}*index_offset_t<index_table,ind>{}), std::pair<const index_table&,const Pos&>>(
//        p.first()*size()+(*this)(p.second),
//        p.second);
//  }

//};

template<class, class...>
class index_table;

template<class... free_indexes,class... dependent_indexes>
class index_table<index_vector<free_indexes...>,dependent_indexes...>
    : public index_vector<free_indexes...>, public dependent_indexes...
{
public:
  using free_ind=index_vector<free_indexes...>;

  using index_vector<free_indexes...>::operator[];
  using dependent_indexes::operator[]...;

    template<class other_Index>
    constexpr auto operator[](other_Index)const {return Nothing{};}
 constexpr  index_table(index_vector<free_indexes...> f_i, dependent_indexes...di):index_vector<free_indexes...>{f_i},dependent_indexes{di}...{}
 constexpr  index_table()=default;

  template<class new_dependen_index>
  friend constexpr  auto operator +(index_table&& me, new_dependen_index&& other)
  {
      return index_table<index_vector<free_indexes...>,dependent_indexes...,new_dependen_index>(
                  static_cast<free_ind&&>(me),static_cast<dependent_indexes&&>(me)...,std::move(other));

  }




};








#endif // QM_VECTOR_INDEX_H
