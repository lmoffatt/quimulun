#ifndef QM_TENSOR_BASE_H
#define QM_TENSOR_BASE_H
#include "mytypetraits.h"
#include "qm_vector_field_declaration.h"





struct Iu_{};
struct Ju_{};
struct Id_{};
struct Jd_{};




template<class ...Is> struct dn;

template<class ...Is> struct up;

template <class, class> struct te;



struct Scalar{};
struct Ide{

  using T=Ide;
  using unit=p<>;
  constexpr static auto  className=my_static_string("_");

  friend std::ostream& operator<<(std::ostream& os, Ide){return os<<"_";}
  template<class T>
  friend auto operator*(Ide,T&& other){ return std::forward<T>(other);}
  template<class T>
  friend auto operator*(T&& other,Ide){ return std::forward<T>(other);}

  template<class T>
  friend auto operator/(T&& other,Ide){ return std::forward<T>(other);}



};



template<class ...Is> struct dn{

  template<class... as>
  Nothing operator()(const up<as...>& ){return Nothing{};}

  Scalar operator()(const up<Is...>& ){return Scalar{};};
};

template<class ...Is> struct up{
  template<class... as>
  Nothing operator()(const dn<as...>& ){return Nothing{};}

  Scalar operator()(const dn<Is...>& ){return Scalar{};};

};


template<class ...>struct ein;

template<class te,class Up, class Down>
struct ein<te,Up,Down>{

};


template <class ...Ups, class ...Downs>
struct te<up<Ups...>,dn<Downs...>>{
  //  template<class... as, class... bs>
  //  Nothing operator()(const ve<as...>&, const co<bs...>& ){return Nothing{};}
  //  Scalar operator()(ve<Ups...>,co<Downs...>){return Scalar{};};

  using T=decltype (sizeof... (Ups)>1 ? (typename Ups::T{}*...*1)/(typename Downs::T{}*...*1):(typename Downs::T{}*...*1));
 // using unit=decltype ((typename Ups::unit{}*...*p<>{})/(typename Downs::unit{}*...*Ide{}));


  constexpr static auto myClassNameDown()
  {

    if constexpr (sizeof... (Downs)>0)
      return (my_static_string("d/d")+...+(Downs::className+my_static_string("_")));
    else return my_static_string("");
  }

  constexpr static auto myClassNameUp()
  {

    if constexpr (sizeof... (Ups)>0)
      return ((Ups::className+my_static_string("_"))+...);
    else return my_static_string("");
  }
  constexpr static auto myClassName()
  {
    return myClassNameDown()+myClassNameUp();
  }
  constexpr static auto  className=myClassName();

  template<class... iUps, class... iDowns>
  auto operator()(up<iUps...>, dn<iDowns...>){return ein<te,up<iUps...>,dn<iDowns...>>{};}

  template <class ...Ups2, class ...Downs2>
  friend
      auto operator*(te,te<up<Ups2...>,dn<Downs2...>>){
    return te<up<Ups...,Ups2...>,dn<Downs...,Downs2...>>{};
  }

  friend constexpr bool operator==(const te& , const te& ){return true;}

};



template <class ...Ups0, class ...iUps0,class a, class ia,
          class b>

constexpr auto operator * (ein<te<up<Ups0...>,dn<a>>,up<iUps0...>,dn<ia>>,
                         ein<te<up<b>,dn<>>,up<ia>,dn<>>)
{

  /**
        -**         -A
       |          |
       |      *   |
       |          |
        -A

  **/
      if constexpr (std::is_same_v<a,b >)
          return ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>{};
  else
      return Nothing{};

}

template <class ...Ups0, class ...iUps0,
          class Up1, class iUp1>
constexpr auto operator * (ein<te<up<Ups0...>,dn<>>,up<iUps0...>,dn<>>,
                         ein<te<up<Up1>,dn<>>,up<iUp1>,dn<>>)
{

  /**
        -**         -A            -**A
       |          |              |
       |      *   |        -->   |
       |          |              |


  **/
      return ein<te<up<Ups0...,Up1>,dn<>>,up<iUps0...,iUp1>,dn<>>{};

}



template <class ...Ups0, class ...iUps0,class A, class iA,
          class B, class iB>

constexpr auto operator * (ein<te<up<Ups0...>,dn<A>>,up<iUps0...>,dn<iA>>,
                         ein<te<up<B>,dn<>>,up<iB>,dn<>>)
{

  /**
        -**         -B            -**B
       |          |              |
       |      *   |        -->   |
       |          |              |
        -A                        -A

  **/
      return ein<te<up<Ups0...,B>,dn<A>>,up<iUps0...,iB>,dn<iA>>{};

}

template <class ...Downs0, class ...iDowns0>
constexpr Nothing join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,Nothing)
{
  return Nothing{};
}



template <class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>,
                    ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return ein<te<up<Ups1...>,dn<Downs0...,Downs1...>>,up<iUps1...>,dn<iDowns0...,iDowns1...>>{};
}



template <class Up0, class iUp0,class Up00, class iUp00,class ...Ups0, class ...iUps0,class Down1, class iDown1,class ...Downs0, class ...iDowns0>

constexpr auto operator * (ein<te<up<Up0,Up00,Ups0...>,dn<Downs0...>>,up<iUp0,iUp00,iUps0...>,dn<iDowns0...>>,
                         ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>)
{

  /**
             -***                                                         -*      -*    -*
            |          |                        |             |          |       |     |
            |      *   |           --> join (   |      ,      |      *   |   *   |  *  |     )
            |          |                        |             |          |       |     |
             -***       -*                       -***          -*

  **/


      return  join(ein<te<up<>,dn<Downs0...>>,up<>,dn<iDowns0...>>{},
                  ((ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{}*
                    ein<te<up<Up0>,dn<>>,up<iUp0>,dn<>>{}*
                    ein<te<up<Up00>,dn<>>,up<iUp00>,dn<>>{})*...*
                   ein<te<up<Ups0>,dn<>>,up<iUps0>,dn<>>{})
                  );

}




template <class ...Ups, class ...iUps,class Down0, class iDown0,class Down1, class iDown1,class ...Downs, class ...iDowns>

constexpr auto operator * (ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>,
                         ein<te<up<>,dn<Down0,Down1,Downs...>>,up<>,dn<iDown0,iDown1,iDowns...>>)
{
  /**
             -***                                       -***
            |          |                                |          |       |     |
            |      *   |           -->                  |      *   |   *   |  *  |
            |          |                                |          |       |     |
                        -***                                       -*      -*    -*

  **/

      return ((ein<te<up<Ups...>,dn<>>,up<iUps...>,dn<>>{}*
               ein<te<up<>,dn<Down0>>,up<>,dn<iDown0>>{}*ein<te<up<>,dn<Down1>>,up<>,dn<iDown1>>{})
              *...*
              ein<te<up<>,dn<Downs>>,up<>,dn<iDowns>>{});
}


template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0>
constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         Nothing)
{

  return Nothing{};

}

template <          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>
constexpr auto operator * (Nothing,ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{
  return Nothing{};

}




template <class ...Ups0, class ...iUps0,class ...Downs0, class ...iDowns0,
          class ...Ups1, class ...iUps1,class ...Downs1, class ...iDowns1>

constexpr auto operator * (ein<te<up<Ups0...>,dn<Downs0...>>,up<iUps0...>,dn<iDowns0...>>,
                         ein<te<up<Ups1...>,dn<Downs1...>>,up<iUps1...>,dn<iDowns1...>>)
{

  /**
        -**         -**                     -****
       |          |                        |          |
       |      *   |            -->         |      *   |
       |          |                        |          |
        -**         -**                                -****

  **/
      return ein<te<up<Ups0...,Ups1...>,dn<>>,up<iUps0...,iUps1...>,dn<>>{}*
      ein<te<up<>,dn<Downs0...,Downs1...>>,up<>,dn<iDowns0...,iDowns1...>>{};

}


#endif // QM_TENSOR_BASE_H
