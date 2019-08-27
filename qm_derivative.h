#ifndef QM_DERIVATIVE_H
#define QM_DERIVATIVE_H
#include "qm_data.h"

#include<algorithm>
namespace qm {

template<class...> struct Der;
template<class...> struct Der2;

template<>
struct Der<double,double>{ typedef double type;};



template<class T,class unit1, class unit2>
struct Der<v<T,unit1>,v<T,unit2>>
{
  typedef decltype (unit1{}/unit2{}) unit;
  typedef v<T,unit> type;
};




template<class F, class X>
struct Der<F,Der<F,X>>
{
  typedef X type;
};



template<class F, class X>
using Der_t=typename Der<F,X>::type;

template<class T, class unit,class unit2,class Id>
struct Der<v<T,unit>,Datum_<Id,v<unit2>,vec<>>>
{
  typedef Datum_<Id,Der_t<v<T,unit>,v<T,unit2>>,vec<>> type;
};


template<class T, class unit,class unit2,class Id, class... Xs>
struct Der<v<T,unit>,Datum_<Id,v<T,unit2>,vec<Xs...>>>
{
  typedef Datum_<Id,Der_t<v<T,unit>,v<T,unit2>>,vec<Xs...>> type;
};







template<class ,class...>
class Derivative;
template<class T, class unit,class Id,class... Ds>
struct Der<v<T,unit>,Data_<Id,Ds...>>
{
  typedef Derivative<v<T,unit>,Data_<Id,Ds...>> type;

};

template<class T, class unit,class Id,class... Ds>
class Derivative<v<T,unit>,Data_<Id,Ds...>>
{
public:
  typedef Data_<Id, Der_t<v<T,unit>,Ds>...> derivative_type;
private:
  v<T,unit> f_;
  derivative_type Df_;
public:
  auto& f()const {return f_;}
  auto& f() {return f_;}

  auto& Df()const {return Df_;}
  auto& Df() {return Df_;}


  Derivative(v<T,unit>&& f,Data_<Id,Der_t<v<T,unit>,Ds>...>&& Df):f_{std::move(f)},Df_{std::move(Df)}{}

  friend std::ostream& operator<<(std::ostream& os, const Derivative& me)
  {
    return os<<"f="<<me.f()<<" df="<<me.Df();
  }


};
template<class T, class unit,class Id,class... Ds>
Derivative(v<T,unit>&& f,Data_<Id,Ds...>&& Df)
    ->Derivative<v<T,unit>,Data_<Id,Der_t<v<T,unit>,Ds>...>>;



template<class T, class... units,class Id,class... Ds>
class Derivative<logv<T,units...>,Data_<Id,Ds...>>
{
public:
  typedef Data_<Id, Der_t<v<T,dimension_less>,Ds>...> derivative_type;
private:
  logv<T,units...> f_;
  derivative_type Df_;
public:
  auto& f()const {return f_;}
  auto& f() {return f_;}

  auto& Df()const {return Df_;}
  auto& Df() {return Df_;}


  Derivative(logv<T,units...>&& f,Data_<Id,Der_t<v<T,dimension_less>,Ds>...>&& Df):f_{std::move(f)},Df_{std::move(Df)}{}

  friend std::ostream& operator<<(std::ostream& os, const Derivative& me)
  {
    return os<<"f="<<me.f()<<" df="<<me.Df();
  }


};
template<class T, class... units,class Id,class... Ds>
Derivative(logv<T,units...>&& f,Data_<Id,Ds...>&& Df)
    ->Derivative<logv<T,units...>,Data_<Id,Der_t<v<T,dimension_less>,Ds>...>>;



template<class Id_data,class Id_datum, class T, class unit>
auto  self_Derivative( Datum_<Id_datum,v<T,unit>,vec<>>&& x)
{
  typedef Datum_<Id_datum,v<T,unit>,vec<>> self_type;
  typedef v<T,unit> element_type;
  typedef Data_<Id_data,self_type> self_data;

  typedef Der_t<v<T,unit>, self_type> derivative_datum;
  typedef Der_t<v<T,unit>, self_data> derivative_data;


  return Datum_(Id_datum{},derivative_data(std::move(x).value(),Data_(Id_data{},derivative_datum(v<T,dimension_less>(1.0)))));
}

template<class Id,class... Ds>

auto  self_Derivative( Data_<Id,Ds...>&& x)
{

  return Data_(Id{},self_Derivative<Id>(std::move(x)[typename Ds::myId{}])...);
}



//-------------------    ADDition-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator +( Derivative<v<T,unit>,Data_<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data_<Id,Ds...>>&& two)
{
  return Derivative<v<T,unit>,Data_<Id,Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator +( Derivative<v<T,unit>,Data_<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data_<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator +( Derivative<v<T,unit>,Data_<Id,Ds...>>&& one,  v<T,unit>&& two)
{
  one.f()+=std::move(two);
  return one;
}

template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator +(   v<T,unit>&& two,Derivative<v<T,unit>,Data_<Id,Ds...>>&& one)
{
  std::move(one)+std::move(two);
}

//----logv addition



template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data_<Id,Ds...>>
operator +( Derivative<logv<T,units...>,Data_<Id,Ds...>>&& one,  Derivative<logv<T,units...>,Data_<Id,Ds...>>&& two)
{
  return Derivative<logv<T,units...>,Data_<Id,Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class... unit,class... unit2,class Id,class... Ds, class ...Ds2>
auto
operator +( Derivative<logv<T,unit...>,Data_<Id,Ds...>>&& one,  Derivative<logv<T,unit2...>,Data_<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}







//-------------------    SUBSTRACTION-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator -(  Derivative<v<T,unit>,Data_<Id,Ds...>> one)
{
  one.f()=-one.f();
  one.Df()=-std::move(one.Df());
  return one;
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator -( Derivative<v<T,unit>,Data_<Id,Ds...>> one,  v<T,unit> two)
{
  one.f()-=std::move(two);
  return one;
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator -(   v<T,unit> two,Derivative<v<T,unit>,Data_<Id,Ds...>> one)
{
  return (-std::move(one))+std::move(two);
}





template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator -( Derivative<v<T,unit>,Data_<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data_<Id,Ds...>>&& two)
{
  return Derivative<v<T,unit>,Data_<Id,Ds...>>(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator -( Derivative<v<T,unit>,Data_<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data_<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}


// --------------operations on logv


template<class T, class... unit,class Id,class... Ds, class ...Ds2>
auto
operator -(Derivative<logv<T,unit...>,Data_<Id,Ds...>> one, Derivative<v<T,dimension_less>,Data_<Id,Ds2...>> two)
{
  return Derivative(one.f()-two.f(),std::move(one.Df())-std::move(two.Df()));
}




template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data_<Id,Ds...>>
operator -( Derivative<v<T,dimension_less>,Data_<Id,Ds...>> one,  logv<T,units...> two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}


template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data_<Id,Ds...>>
operator -(  logv<T,units...> two, Derivative<v<T,dimension_less>,Data_<Id,Ds...>> one)
{
  auto f=two-one.f();
  return Derivative<logv<T,units...>,Data_<Id,Ds...>>(std::move(f),-std::move(one.Df()));
}



template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data_<Id,Ds...>>
operator -( Derivative<logv<T,units...>,Data_<Id,Ds...>> one,  double two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}

template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data_<Id,Ds...>>
operator -(double one,Derivative<logv<T,units...>,Data_<Id,Ds...>> two)
{
  auto f=one-two.f();
  return Derivative(std::move(f),-std::move(two.Df()));
}




//-------------------   MULTIPLICATION-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data_<Id,Ds...>>
operator *( const Derivative<v<T,unit>,Data_<Id,Ds...>>& one,  const Derivative<v<T,unit>,Data_<Id,Ds...>>& two)
{
  return Derivative<v<T,unit>,Data_<Id,Ds...>>(
      one.f()*two.f(),
      one.Df()*two.f()+one.f()*two.Df());
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator *(const  Derivative<v<T,unit>,Data_<Id,Ds...>>& one,  const Derivative<v<T,unit>,Data_<Id,Ds2...>>& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}



template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,Data_<Id,Ds...>>
operator *( const Derivative<v<T,unit>,Data_<Id,Ds...>>& one, const v<T,unit2>& two)
{
  return Derivative<v<T,decltype(unit{}*unit2{})>,Data_<Id,Ds...>>(
      one.f()*two,
      one.Df()*two);
}

template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,Data_<Id,Ds...>>
operator *(const v<T,unit2>& two, const Derivative<v<T,unit>,Data_<Id,Ds...>>& one)
{
  return one*two;
}


//--------------------------------DIVISION--------------------------//


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>
operator /( const Derivative<v<T,unit>,Data_<Id,Ds...>>& one,  const Derivative<v<T,unit2>,Data_<Id,Ds...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>(std::move(f),std::move(Df));
}

template<class T, class unit,class unit2,class Id,class... Ds, class ...Ds2>
auto
operator /(const  Derivative<v<T,unit>,Data_<Id,Ds...>>& one,  const Derivative<v<T,unit2>,Data_<Id,Ds2...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>
operator /( const Derivative<v<T,unit>,Data_<Id,Ds...>>& one,  const v<T,unit2>& two)
{
  auto f=one.f()/two;
  auto Df=one.Df()/two;


  return Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>
operator /( const v<T,unit>& one,  const Derivative<v<T,unit2>,Data_<Id,Ds...>>& two)
{
  auto f=one/two.f();
  auto Df=f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,Data_<Id,Ds...>>(std::move(f),std::move(Df));
}




//----------------------SQR

template<class T, class unit,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit{})>,Data_<Id,Ds...>>
sqr( const Derivative<v<T,unit>,Data_<Id,Ds...>>& x)
{
  auto f=sqr(x.f());
  auto Df=2.0*x.f()*x.Df();
  return Derivative<v<T,decltype(unit{}*unit{})>,Data_<Id,Ds...>>(std::move(f),std::move(Df));

}


template<class T, class unit,class Id,class... Ds>
Derivative<logv<T,unit>,Data_<Id,Ds...>>
log( const Derivative<v<T,unit>,Data_<Id,Ds...>>& x)
{
  using std::log;
  auto f=log(x.f());
  auto Df=x.Df()/x.f();
  return Derivative(std::move(f),std::move(Df));

}






} // namespace qm

#endif // QM_DERIVATIVE_H
