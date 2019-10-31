#ifndef QM_TENSOR_DERIVATIVE_H
#define QM_TENSOR_DERIVATIVE_H


#include "qm_vector_space.h"

#include<algorithm>
//namespace qm {




template<class...> struct der;
template<class...> struct der_tmp;




template<class F, class X> struct der<F,X>
{
  constexpr static auto  className=my_static_string("d_")+F::className+my_static_string("_d_")+X::className;
  typedef te<up<F>,dn<X>> type;
};

template<class F, class... X,class Y>
struct der<te<up<F>,dn<X...>>,Y>{
  typedef te<up<F>,dn<X...,Y>> type;

};

template<class F, class X>
struct der<F,te<up<F>,dn<X>>>{
  typedef X type;
};





template<class... Fx, class... X,class... Fy,class... Y>
struct der<te<up<Fx...>,dn<X...>>,te<up<Fy...>,dn<Y...>>>{
  typedef te<up<Fx...,Y...>,dn<X...,Fy...>> type;

};


template<class F, class X>
using der_t=typename der<F,X>::type;


template<class...> struct Der;

template<class F, class X>
using Der_t=typename Der<F,X>::type;


template<class T,class unit1, class unit2>
struct der<v<T,unit1>,v<T,unit2>>
{
  typedef decltype (unit1{}/unit2{}) unit;
  typedef v<T,unit> type;
};

template<class T,class... unit1, class unit2>
struct der<logv<T,unit1...>,v<T,unit2>>
{
  typedef decltype (dimension_less{}/unit2{}) unit;
  typedef v<T,unit> type;
};


template<class value_type,class value_type2,class I1>
struct der<value_type,x_i<I1,value_type2>>
{
  typedef x_i<der_t<Ide,I1>,der_t<value_type,value_type2>> type;
};






template<class I1,class value_type1, class I2,class value_type2>
struct der<x_i<I1,value_type1>,x_i<I2,value_type2>>
{
  typedef x_i<der_t<I1,I2>,der_t<value_type1,value_type2>> type;
};




template<class value_type,class... Ds>
struct der<value_type,vector_space<Ds...>>
{
  typedef vector_space<der_t<value_type,Ds>...> type;

};


template<class Id1,class value_type,class... Ds>
struct der<x_i<Id1,value_type>,vector_space<Ds...>>
{
  typedef vector_space<der_t<x_i<Id1,value_type>,Ds>...> type;

};



template<class... Ds, class... Ds2>
struct der<vector_space<Ds...>,vector_space<Ds2...>>
{
  typedef vector_space<der_t<Ds,vector_space<Ds2...>>...> type;

};


template<class ,class...>
class Derivative;

template<class T, class unit,class... Ds,template<class, class>class v>
struct Der<v<T,unit>,vector_space<Ds...>>
{
  typedef Derivative<v<T,unit>,vector_space<Ds...>> type;

};


template<class value_type,class Id,class... Ds>
struct Der<x_i<Id,value_type>,vector_space<Ds...>>
{
  typedef x_i<Id,Derivative<value_type,vector_space<Ds...>>> type;

};


template<class... Ds, class... Ds2>
struct Der<vector_space<Ds...>,vector_space<Ds2...>>
{
  typedef vector_space<Der_t<Ds,vector_space<Ds2...>>...> type;

};


struct primitive_k{  constexpr static auto  className=my_static_string("f");};
struct derivative_k{   constexpr static auto  className=my_static_string("der");};

template<class dependent_type,class... Ds>
class Derivative<dependent_type,vector_space<Ds...>>
{
public:
  // typedef v<T,unit> dependent_type;
  typedef Derivative element_type;

  typedef vector_space<Ds...> independent_type;
  typedef der_t<dependent_type,independent_type> derivative_type;


  using cols=pack_concatenation_t<
               recursive_t<primitive_k,typename dependent_type::cols>,
      recursive_t<derivative_k,typename derivative_type::cols>>;


  using row_type=decltype(std::tuple_cat(
      typename dependent_type::row_type{},typename derivative_type::row_type{}));

  using row_type_w_unit=decltype(std::tuple_cat(
      typename dependent_type::row_type_w_unit{},typename derivative_type::row_type_w_unit{}));



  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {
    auto [f_r,Df_r]=distribute(transfer_t<typename dependent_type::row_type,Cs<>>{},
                                  transfer_t<typename derivative_type::row_type,Cs<>>{},
                                  std::move(r));
    f().insert_at(p,std::move(f_r));
    Df().insert_at(p,std::move(Df_r));

  }

  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type_w_unit&& r)
  {
    auto [f_r,Df_r]=distribute(transfer_t<typename dependent_type::row_type_w_unit,Cs<>>{},
                                  transfer_t<typename derivative_type::row_type_w_unit,Cs<>>{},
                                  std::move(r));
    f().insert_at(p,std::move(f_r));
    Df().insert_at(p,std::move(Df_r));

  }


private:
  dependent_type f_;
  derivative_type Df_;
public:

  friend   constexpr auto begin(const Derivative& me) {
    return
        //Position<Index<primitive_k,derivative_k>>{}&&
        me.f().begin()&& ::begin(me.Df());
  }




  auto& f()const {return f_;}
  auto& f() {return f_;}

  auto& Df()const {return Df_;}
  auto& Df() {return Df_;}

  template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename dependent_type::cols{}),bool>>
  auto operator()(const Position& p,recursive<primitive_k,ind>)const->decltype (f()(p,ind{}))
  {return f()(p,ind{});}

  template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename derivative_type::cols{}),bool>>
  auto operator()(const Position& p,recursive<derivative_k,ind> )const ->decltype (Df()(p,ind{}))
  {return Df()(p,ind{});}



  Derivative(dependent_type&& f,derivative_type&& Df):f_{std::move(f)},Df_{std::move(Df)}{}
  Derivative()=default;


  static auto begin() {return Position<>{};}
  bool next(Position<>& )const   {     return false;}

  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const{ return *this;}



  friend std::ostream& operator<<(std::ostream& os, const Derivative& me)
  {
    return os<<"f="<<me.f()<<" df="<<me.Df();
  }

  friend bool operator==(const Derivative& me, const Derivative& ti)
  {
    return (me.f()==ti.f())&&(me.Df()==ti.Df());
  }


};


template<class dependent_type,class... Ds>
auto& Df(const Derivative<dependent_type,vector_space<Ds...>>& f){return f.Df();}

template<class dependent_type,class... Ds>
auto& center(const Derivative<dependent_type,vector_space<Ds...>>& f){return f.f();}


template<class T>
auto Df(const T& ){return logP_zero{};}



template<class T, class unit,class... Ds>
Derivative(v<T,unit>&& f,vector_space<Ds...>&& Df)
    ->Derivative<v<T,unit>,der_t<v<T,unit>,vector_space<Ds...>>>;

template<class T, class... units,class... Ds>
Derivative(logv<T,units...>&& f,vector_space<Ds...>&& Df)
    ->Derivative<logv<T,units...>,der_t<v<T,dimension_less>,vector_space<Ds...>>>;









template<class Id_datum, class T, class unit>
 auto  self_derivative(const x_i<Id_datum,v<T,unit>>& )
{
  return der_t<v<T,unit>,x_i<Id_datum,v<T,unit>>>(v<T,dimension_less>(1.0));
}




template<class Id_datum, class T, class unit>
auto  Self_Derivative(const x_i<Id_datum,v<T,unit>>& x)
{
  auto f=x();
  auto df=self_derivative(x);
  return x_i(Id_datum{},
             Derivative(std::move(f),vector_space(std::move(df))));
}


template<class... Ds>
auto  Self_Derivative(const  vector_space<Ds...>& x)
{
  return vector_space(Self_Derivative(x[typename Ds::myId{}])...);
}







//-------------------    ADDition-------------//



template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator +( Derivative<v<T,unit>,vector_space<Ds...>>&& one,  Derivative<v<T,unit>,vector_space<Ds...>>&& two)
{
  return Derivative<v<T,unit>,vector_space<Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class unit,class... Ds, class ...Ds2>
auto
operator +( Derivative<v<T,unit>,vector_space<Ds...>>&& one,  Derivative<v<T,unit>,vector_space<Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class... unit1,class... unit2,class... Ds, class ...Ds2>
auto
operator +( Derivative<logv<T,unit1...>,vector_space<Ds...>>&& one,  Derivative<logv<T,unit2...>,vector_space<Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class... unit1,class... unit2,class... Ds, class ...Ds2>
auto
operator +(const Derivative<logv<T,unit1...>,vector_space<Ds...>>& one, const Derivative<logv<T,unit2...>,vector_space<Ds2...>>& two)
{
  return Derivative(one.f()+two.f(),one.Df()+two.Df());
}


template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator +( Derivative<v<T,unit>,vector_space<Ds...>>&& one,  v<T,unit>&& two)
{
  one.f()+=std::move(two);
  return one;
}

template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator +(   v<T,unit>&& two,Derivative<v<T,unit>,vector_space<Ds...>>&& one)
{
  std::move(one)+std::move(two);
}




//----logv addition



template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator +( Derivative<logv<T,units...>,vector_space<Ds...>>&& one,  Derivative<logv<T,units...>,vector_space<Ds...>>&& two)
{
  return Derivative<logv<T,units...>,vector_space<Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}



template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator +( Derivative<logv<T,units...>,vector_space<Ds...>> one,  logv<T,units...> two)
{
  return Derivative<logv<T,units...>,vector_space<Ds...>>(std::move(one.f())+std::move(two),std::move(one.Df()));
}

template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator +( logv<T,units...> two,Derivative<logv<T,units...>,vector_space<Ds...>> one )
{
  return std::move(one)+std::move(two);
}

template<class T, class... unit,class... unit2,class... Ds>
auto
operator +( Derivative<logv<T,unit...>,vector_space<Ds...>>&& one,  logv<T,unit2...> two)
{
  return Derivative(std::move(one.f())+std::move(two),std::move(one.Df()));
}

template<class T, class... unit,class... unit2,class... Ds>
auto
operator +( logv<T,unit2...> two, Derivative<logv<T,unit...>,vector_space<Ds...>>&& one )
{
  return std::move(one)+std::move(two);
}






//-------------------    SUBSTRACTION-------------//



template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator -(  Derivative<v<T,unit>,vector_space<Ds...>> one)
{
  one.f()=-one.f();
  one.Df()=-std::move(one.Df());
  return one;
}


template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator -( Derivative<v<T,unit>,vector_space<Ds...>> one,  v<T,unit> two)
{
  one.f()=one.f()-std::move(two);
  return one;
}


template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator -(   v<T,unit> two,Derivative<v<T,unit>,vector_space<Ds...>> one)
{
  return (-std::move(one))+std::move(two);
}





template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator -( Derivative<v<T,unit>,vector_space<Ds...>>&& one,  Derivative<v<T,unit>,vector_space<Ds...>>&& two)
{
  return Derivative<v<T,unit>,vector_space<Ds...>>(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}

template<class T, class unit,class... Ds, class ...Ds2>
auto
operator -( Derivative<v<T,unit>,vector_space<Ds...>>&& one,  Derivative<v<T,unit>,vector_space<Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}


// --------------operations on logv


template<class T, class... unit,class... Ds, class ...Ds2>
auto
operator -(Derivative<logv<T,unit...>,vector_space<Ds...>> one, Derivative<v<T,dimension_less>,vector_space<Ds2...>> two)
{
  return Derivative(one.f()-two.f(),std::move(one.Df())-std::move(two.Df()));
}




template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator -( Derivative<v<T,dimension_less>,vector_space<Ds...>> one,  logv<T,units...> two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}


template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator -(  logv<T,units...> two, Derivative<v<T,dimension_less>,vector_space<Ds...>> one)
{
  auto f=two-one.f();
  return Derivative<logv<T,units...>,vector_space<Ds...>>(std::move(f),-std::move(one.Df()));
}



template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator -( Derivative<logv<T,units...>,vector_space<Ds...>> one,  double two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}

template<class T, class... units,class... Ds>
Derivative<logv<T,units...>,vector_space<Ds...>>
operator -(double one,Derivative<logv<T,units...>,vector_space<Ds...>> two)
{
  auto f=one-two.f();
  return Derivative(std::move(f),std::move(two.Df())*(-1.0));
}




//-------------------   MULTIPLICATION-------------//



template<class T, class unit,class... Ds>
Derivative<v<T,unit>,vector_space<Ds...>>
operator *( const Derivative<v<T,unit>,vector_space<Ds...>>& one,  const Derivative<v<T,unit>,vector_space<Ds...>>& two)
{
  return Derivative<v<T,unit>,vector_space<Ds...>>(
      one.f()*two.f(),
      one.Df()*two.f()+one.f()*two.Df());
}

template<class T, class unit,class... Ds, class ...Ds2>
auto
operator *(const  Derivative<v<T,unit>,vector_space<Ds...>>& one,  const Derivative<v<T,unit>,vector_space<Ds2...>>& two)
{

  return Derivative(one.f()+two.f(),one.Df()*two.f()+one.f()*two.Df());
}



template<class T, class unit,class unit2,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,vector_space<Ds...>>
operator *( const Derivative<v<T,unit>,vector_space<Ds...>>& one, const v<T,unit2>& two)
{
  return Derivative<v<T,decltype(unit{}*unit2{})>,vector_space<Ds...>>(
      one.f()*two,
      one.Df()*two);
}

template<class T, class unit,class unit2,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,vector_space<Ds...>>
operator *(const v<T,unit2>& two, const Derivative<v<T,unit>,vector_space<Ds...>>& one)
{
  return one*two;
}


//--------------------------------DIVISION--------------------------//


template<class T, class unit,class unit2,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>
operator /( const Derivative<v<T,unit>,vector_space<Ds...>>& one,  const Derivative<v<T,unit2>,vector_space<Ds...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>(std::move(f),std::move(Df));
}

template<class T, class unit,class unit2,class... Ds, class ...Ds2>
auto
operator /(const  Derivative<v<T,unit>,vector_space<Ds...>>& one,  const Derivative<v<T,unit2>,vector_space<Ds2...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>
operator /( const Derivative<v<T,unit>,vector_space<Ds...>>& one,  const v<T,unit2>& two)
{
  auto f=one.f()/two;
  auto Df=one.Df()/two;


  return Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>
operator /( const v<T,unit>& one,  const Derivative<v<T,unit2>,vector_space<Ds...>>& two)
{
  auto f=one/two.f();
  auto Df=f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,vector_space<Ds...>>(std::move(f),std::move(Df));
}




//----------------------SQR

template<class T, class unit,class... Ds>
Derivative<v<T,decltype(unit{}*unit{})>,vector_space<Ds...>>
sqr( const Derivative<v<T,unit>,vector_space<Ds...>>& x)
{
  auto f=sqr(x.f());
  auto Df=2.0*x.f()*x.Df();
  return Derivative<v<T,decltype(unit{}*unit{})>,vector_space<Ds...>>(std::move(f),std::move(Df));

}


template<class T, class unit,class... Ds>
Derivative<logv<T,unit>,vector_space<Ds...>>
log( const Derivative<v<T,unit>,vector_space<Ds...>>& x)
{
  using std::log;
  auto f=log(x.f());
  auto Df=x.Df()/x.f();
  return Derivative(std::move(f),std::move(Df));

}






//} // namespace qm


#endif // QM_TENSOR_DERIVATIVE_H
