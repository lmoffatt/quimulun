#ifndef QM_VECTOR_INDEX_H
#define QM_VECTOR_INDEX_H
#include "qm_vector_device.h"


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



template<class Id>
class ind
{
private:
  std::size_t n_;
public:

  inline static constexpr auto ind_name=my_static_string("index(");

  inline static constexpr auto name=ind_name+Id::name+my_static_string(")");

  __host__ __device__
      constexpr explicit ind(Id, std::size_t n):n_{n}{}

   constexpr ind()=default;
  __host__ __device__  constexpr auto size() {return n_;}

  template<class Pos>
  __host__ __device__ constexpr auto operator()(const Pos& p)const {
    return std::pair<offset<Id>, const Pos&>(p[Id{}](),p);
  }

  template<class... Id0,class Pos>
  __host__ __device__   constexpr auto operator()(std::pair<offset<Id0...>, const Pos&> p)const
  {
    return std::pair<offset<Id0...,Id>, const Pos&>(
        p.first()*size()+ p.second[Id{}](),
        p.second);
  }


};


template<class ind_type,class... ind_types>
class ind_prod: public ind_type,ind_types...
{


public:


  __host__ __device__
      constexpr ind_prod(ind_type i0,ind_types... i):ind_type{i0},ind_types{i}...{}

  __host__ __device__
      constexpr auto size()const {
    return (ind_type::size()*...*ind_types::size());
  }

  template<class Pos>
  __host__ __device__
      constexpr auto operator()(const Pos& p)const {
    return compose(p,static_cast<ind_type&>(*this),static_cast<ind_types&>(*this)...).first;
  }


};

template <class, class> class ind_d;

template<class Id, class... ind_types>
class ind_d<ind_prod<ind_types...>,Id>
{

private:
  ind_prod<ind_types...> indexes_;
  vector_device<std::size_t> offset_;


  template<class>
  struct index_offset;

  template<class T>
  using index_offset_t=typename index_offset<T>::type;

  template<class Id1>
  struct index_offset<ind<Id1>>
  {
    using type=offset<Id1>;
  };

  template<class Id1, class... index_types>
  struct index_offset<ind_d<ind<index_types...>,Id1>>
  {
    using type=decltype((index_offset_t<index_types>{}*...*offset<Id1>{}));
  };


public:

  inline static constexpr auto x_iname=my_static_string("index(");
  inline static constexpr auto name=x_iname+Id::name+my_static_string(")");
  using myId=Id;

  __host__ __device__
      constexpr ind_d(Id, ind_prod<ind_types...> i):indexes_{i},offset_{i.size()}{}

  __host__ __device__
      auto size()const{return offset_.size();}


  template< class Pos>
  __host__ __device__
      auto& operator[](const Pos& p) {return offset_[indexes_(p)()];}

  template<class Pos>
  __host__ __device__
      constexpr auto operator()(const Pos& p)const {
    return std::pair<index_offset_t<ind_d>, const Pos&>(offset_[indexes_(p).first()]+p[Id{}](),p);
  }


  template<class... Id0,class Pos>
  __host__ __device__
      constexpr auto operator()(std::pair<offset<Id0...>, const Pos&> p)const
  {
    return std::pair<decltype(offset<Id0...>{}*index_offset_t<ind_d>{}), const Pos&>(
        p.first()*size()+(*this)(p.second),
        p.second);
  }


};


#endif // QM_VECTOR_INDEX_H
