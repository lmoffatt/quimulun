#ifndef QM_TENSOR_DISTRIBUTION_H
#define QM_TENSOR_DISTRIBUTION_H


#include <iomanip>

#include "qm_tensor_derivative.h"
#include "qm_tensor_coordinate.h"
#include "qm_probability_base.h"
#include <random>
#include <my_math.h>



template<class Id>
struct mean{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_mean");
};

template<class Id>
struct stddev{
  typedef typename Id::T T;
  typedef typename Id::unit unit;
  constexpr static auto  className=Id::className+my_static_string("_stddev");
};

template<class Id>
struct variance{
  //typedef typename Id::T T;
  // typedef typename Id::unit unit;
  constexpr static auto  className=Id::className+my_static_string("_variance");
};



template <class ...Ups, class ...Downs
          , class value_type0, class value_type2>
//, typename=std::enable_if_t<(sizeof... (Ups)==sizeof... (Downs))&&(sizeof... (Ups)>0),int>>
auto operator - (const x_i<logpr<up<Ups...>,dn<Downs...>>, value_type0>& one,
               const x_i<logpr<up<Ups...>,dn<Downs...>>, value_type2>& two)
{
  return one()-two();
}


struct Sample{};
struct SampleParameters{};

struct logProbability{};
struct FisherInformationMatrix{};

template<class Op>struct Parallel{};



class Bernoulli_Distribution
{
private:

  std::bernoulli_distribution d_;

public:
  auto& d(){return d_;}
};



template< typename Int>
class Uniform_Integer_Distribution
{
public:
  static_assert(std::is_integral_v<Int>);
  using distribution_type=std::uniform_int_distribution<Int>;
private:

 std::uniform_int_distribution<Int> d_;

public:
 auto& d(){return d_;}

  
};

template< typename Int>
class Uniform_Index_Distribution
{
public:
  static_assert(std::is_integral_v<Int>);
  using distribution_type=std::uniform_int_distribution<Int>;
private:

  std::uniform_int_distribution<Int> d_;

public:
  auto& d(){return d_;}


};



template <class Real>
class Normal_Distribution
{
private:
  static_assert (std::is_floating_point_v<Real> );
  std::normal_distribution<Real> d_;

public:
  auto& d(){return d_;}


};




template <class Real>
class Exponential_Distribution
{
private:
  static_assert (std::is_floating_point_v<Real> );
  std::exponential_distribution<Real> d_;

public:
  auto& d(){return d_;}


};



template<class Real>
struct stretch_move_Distribution  {
  static_assert (std::is_floating_point_v<Real> );
};



template<class...>struct D;


template<class Id,class Distribution, class... Xs>
struct  D<Id,Distribution,Arguments<Xs...>>
{
public:

  typedef   Id myId;
  auto& operator[](Id)const {return *this;}
  auto& operator[](Id) {return *this;}
  D(Id&& ,Distribution&&, Arguments<Xs...>&&){}
};

template<class Id,class Distribution, class... Xs, class...XXs>
struct  D<Id,Distribution,Arguments<Xs...>, Index_struct<XXs...>>
{
public:

  typedef   Id myId;
  auto& operator[](Id)const {return *this;}
  auto& operator[](Id) {return *this;}
  D(Id&& ,Distribution&&, Arguments<Xs...>&&,Index_struct<XXs...>&&){}
};


template<class Id,class Distribution, class... Xs>
D(Id&& ,Distribution&&, Arguments<Xs...>&&)->D<Id,Distribution,Arguments<Xs...>>;

template<class Id,class Distribution, class... Xs, class...XXs>
D(Id&& ,Distribution&&, Arguments<Xs...>&&, Index_struct<XXs...>&&)->D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>>;

template <class Id, class F, class Distribution, class G, class ...Xs >
class Sampler
{
  F f_;
  Distribution d_;
  G g_;
public:
  Sampler(Id, F&& f, Distribution&& d, G&& g, Xs...): f_(std::move(f)), d_{std::move (d)}, g_{std::move(g)}{}



};









template <class> struct extract_distribution_Id{using type=Cs<>;};

template< class Id,class Distribution, class... Xs, class... XXs>
struct extract_distribution_Id<D<Id,Distribution,Arguments<Xs...>,Index_struct<XXs...>>>{using type=Cs<Id>;};

template< class Id,class Distribution, class... Xs>
struct extract_distribution_Id<D<Id,Distribution,Arguments<Xs...>>>{using type=Cs<Id>;};


template<class C> using extract_distribution_Id_t=typename extract_distribution_Id<C>::type;

template< class... Ids>
struct extract_distribution_Id<Cs<Ids...>>{using type=pack_concatenation_t<extract_distribution_Id_t<Ids>...>;};


template<class... Ids,class Distribution, class... Xs>
struct  D<Cs<Ids...>,Distribution,Arguments<Xs...>>: public D<Ids,Distribution,Arguments<Xs...>>...
{

public:
  using D<Ids,Distribution,Xs...>::operator[]...;
  D(Cs<Ids...> ,Distribution&& g, Arguments<Xs...>&&):
                                              D<Ids,Distribution,Xs...>{Ids{},Distribution{g},Xs{}...}...{}
};






#endif // QM_TENSOR_DISTRIBUTION_H
