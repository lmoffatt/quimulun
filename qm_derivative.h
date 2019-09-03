#ifndef QM_DERIVATIVE_H
#define QM_DERIVATIVE_H
#include "qm_data.h"

#include<algorithm>
//namespace qm {


template<class...> struct der;
template<class...> struct der_tmp;

struct Id{
  constexpr static auto  className=my_static_string("_");

};

template<class...>struct product;

template<class F> struct product<Id,F>{typedef F type;};
template<class F> struct product<F,Id>{typedef F type;};
template<class F, class X> struct product<F,der<Id,X>>{typedef der<F,X> type;};


template<class F,class T, class unit> struct product<F,v<T,unit>>{typedef v<T,unit> type;};

template<class F,class T, class unit> struct product<v<T,unit>,F>{typedef v<T,unit> type;};




template <class X, class Y> using product_t=typename product<X,Y>::type;

template<class F,class I1, class Value_type,class X,class... Xs> struct product<F, Datum<I1,Value_type,vec<X,Xs...>>>{
  typedef Datum<product_t<F,I1>,product_t<F,Value_type>,vec<X,Xs...>> type;
};
template<class F,class I1, class Value_type> struct product<F, Datum<I1,Value_type,vec<>>>{
  typedef Datum<product_t<F,I1>,product_t<F,Value_type>,vec<>> type;
};


//template <class F, class X> using der_tmp_t=typename der_tmp<F,X>::type;

template<class F, class X> struct der<F,X>
{
  //typedef typename F::T T;
  //typedef decltype (typename F::unit{}/typename X::unit{}) unit;
  constexpr static auto  className=my_static_string("d_")+F::className+my_static_string("_d_")+X::className;
  typedef der<F,X> type;
};

template<class F, class X,class Y> struct der<F,X,Y>{
  constexpr static auto  className=my_static_string("d_")+F::className+my_static_string("_d_")+X::className+F::className+my_static_string("_d_")+Y::className;
  typedef der<F,X,Y> type;

};

template<class F, class X,class Y> struct der<der<F,X>,Y>{
  typedef der<F,X,Y> type;
};

template<class F, class X> struct der<F,der<F,X>>{
  typedef X type;
};


template<class F, class X> struct der_{typedef typename der<F,X>::type type;};

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

/*
template< class value_type,class T,class unit2,class I1, class... Xs>
struct der<value_type,Datum<I1,v<T,unit2>,vec<Xs...>>>
{
  typedef Datum<der_t<Id,I1>,der_t<value_type,v<T,unit2>>,vec<Xs...>> type;
};
*/
template<class value_type,class value_type2,class I1, class... Xs>
struct der<value_type,Datum<I1,value_type2,vec<Xs...>>>
{
  typedef Datum<der_t<Id,I1>,der_t<value_type,value_type2>,vec<Xs...>> type;
};


/*
template<class I1,class value_type1, class... Xs1,class I2,class value_type2, class... Xs2>
struct der<Datum<I1,value_type1,vec<Xs1...>>,Datum<I2,value_type2,vec<Xs2...>>>
{
  typedef Datum<der_t<I1,I2>,product_t<I1,der_t<value_type1,Datum<I2,value_type2,vec<Xs2...>>>>,vec<Xs1...>> type;
};
*/

template<class I1,class value_type1, class... Xs1,class I2,class value_type2, class... Xs2>
struct der<Datum<I1,value_type1,vec<Xs1...>>,Datum<I2,value_type2,vec<Xs2...>>>
{
  typedef Datum<der_t<I1,I2>,der_t<value_type1,value_type2>,decltype(vec<Xs2...>{}<<vec<Xs1...>{})> type;
};




template<class value_type,class I1,class... Ds>
struct der<value_type,Data<I1,Ds...>>
{
  typedef Data<der_t<Id,I1>, der_t<value_type,Ds>...> type;

};


template<class Id1,class value_type,class ...Xs,class Id2, class... Ds>
    struct der<Datum<Id1,value_type,vec<Xs...>>,Data<Id2,Ds...>>
{
  typedef Data<der_t<Id1,Id2>,der_t<Datum<Id1,value_type,vec<Xs...>>,Ds>...> type;

};

template<class F,class Id1,class... Ds> struct product<F,Data<Id1,Ds...>>{
  typedef Data<product_t<F,Id1>,product_t<F,Ds>...> type;
};


template<class Id1,class... Ds,class Id2, class... Ds2>
struct der<Data<Id1,Ds...>,Data<Id2,Ds2...>>
{
  typedef Data<der_t<Id1,Id2>,der_t<Ds,Data<Id2,Ds2...>>...> type;

};





template<class ,class...>
class Derivative;

template<class T, class unit,class Id,class... Ds,template<class, class>class v>
struct Der<v<T,unit>,Data<Id,Ds...>>
{
  typedef Derivative<v<T,unit>,Data<Id,Ds...>> type;

};


template<class value_type,class Id,class Id2,class... Ds>
struct Der<Datum<Id,value_type,vec<>>,Data<Id2,Ds...>>
{
  typedef Datum<Id,Derivative<value_type,Data<Id2,Ds...>>,vec<>> type;

};


template<class Id,class... Ds,class Id2, class... Ds2>
struct Der<Data<Id,Ds...>,Data<Id2,Ds2...>>
{
  typedef Data<Id,Der_t<Ds,Data<Id2,Ds2...>>...> type;

};



template<class dependent_type,class Id,class... Ds>
class Derivative<dependent_type,Data<Id,Ds...>>
{
public:
 // typedef v<T,unit> dependent_type;
  typedef Data<Id,Ds...> independent_type;
  typedef der_t<dependent_type,independent_type> derivative_type;
private:
  dependent_type f_;
  derivative_type Df_;
public:
  auto& f()const {return f_;}
  auto& f() {return f_;}

  auto& Df()const {return Df_;}
  auto& Df() {return Df_;}


//  Derivative(v<T,unit>&& f,Data<Id,der_t<v<T,unit>,Ds>...>&& Df):f_{std::move(f)},Df_{std::move(Df)}{}
  Derivative(dependent_type&& f,derivative_type&& Df):f_{std::move(f)},Df_{std::move(Df)}{}


  friend std::ostream& operator<<(std::ostream& os, const Derivative& me)
  {
    return os<<"f="<<me.f()<<" df="<<me.Df();
  }
};


template<class dependent_type,class Id,class... Ds>
auto& Df(const Derivative<dependent_type,Data<Id,Ds...>>& f){return f.Df();}

template<class T>
auto Df(const T& f){return logP_zero{};}



template<class T, class unit,class I,class... Ds>
Derivative(v<T,unit>&& f,Data<I,Ds...>&& Df)
    ->Derivative<v<T,unit>,der_t<v<T,unit>,Data<I,Ds...>>>;

template<class T, class... units,class I,class... Ds>
Derivative(logv<T,units...>&& f,Data<I,Ds...>&& Df)
    ->Derivative<logv<T,units...>,der_t<v<T,dimension_less>,Data<I,Ds...>>>;


template<class Id1,class Id2,class value_type1,class value_type2>
auto operator*(const Datum<der<Id,Id1>,value_type1,vec<>>& one, const Datum<der<Id,Id2>,value_type2,vec<>>& two)
{
  return Datum(der<Id,Id1,Id2>{},one.value()*two.value());
}



template<class val_type,class Id,class Id2,class... Ds>
struct Datum<Id,Derivative<val_type,Data<Id2,Ds...>>,vec<>>

{
public:
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  typedef Id myId;

  typedef Derivative<val_type,Data<Id2,Ds...>> element_type;
  typedef  Derivative<val_type,Data<Id2,Ds...>> value_type;

  typedef vec<> myIndexes;

  constexpr static  auto numIndex=0;


protected:
  Datum const& get(Id)const & {return *this;}
  Datum& get(Id) & {return *this;}
  Datum get(Id)&& {return *this;}

  template<class I2>
  auto & get(der<Id,I2>)const &
  {
    return  value().Df()[I2{}];
  }

  template<class I2>
  auto & get(der<Id,I2>) &
  {
    return  value().Df()[I2{}];
  }
  template<class I2>
  auto get(der<Id,I2>) &&
  {
    return  value().Df()[I2{}];
  }


private:
  value_type value_;
public:
  Datum(value_type&& x):value_{std::move(x)}{}
  Datum(Id,value_type&& x):value_{std::move(x)}{}
  Datum()=default;
  Datum const& operator[](Id)const & {return *this;}
  Datum& operator[](Id) & {return *this;}
  Datum operator[](Id)&& {return *this;}
  static auto begin() {return Position<>{};}



  bool next(Position<>& p)const
  {
    return false;
  }
  template<class Position>
  auto& operator()(const Position& p){ return value_;}

  template<class Position>
  auto& operator()(const Position& p)const{ return value_;}

  auto& operator()()const{ return value_;}
  auto& operator()(){ return value_;}

  auto& value()const& {return value_;}
  auto& value()& {return value_;}
  auto value()&& {return value_;}

  friend std::ostream& operator<<(std::ostream& os, const Datum me)
  {
    return os<<me.value();
  }

};






/*
template<class T, class... units,class Id,class... Ds>
class Derivative<logv<T,units...>,Data<Id,Ds...>>
{
public:
  typedef Data<Id, der_t<v<T,dimension_less>,Ds>...> derivative_type;
private:
  logv<T,units...> f_;
  derivative_type Df_;
public:
  auto& f()const {return f_;}
  auto& f() {return f_;}

  auto& Df()const {return Df_;}
  auto& Df() {return Df_;}


  Derivative(logv<T,units...>&& f,Data<Id,der_t<v<T,dimension_less>,Ds>...>&& Df):f_{std::move(f)},Df_{std::move(Df)}{}

  friend std::ostream& operator<<(std::ostream& os, const Derivative& me)
  {
    return os<<"f="<<me.f()<<" df="<<me.Df();
  }


};

*/


template<class Id_datum, class T, class unit>
auto  self_derivative( const Datum<Id_datum,v<T,unit>,vec<>>& x)
{
  return der_t<v<T,unit>,Datum<Id_datum,v<T,unit>,vec<>>>(v<T,dimension_less>(1.0));
}




template<class Id_data,class Id_datum, class T, class unit>
auto  Self_Derivative(Id_data,const Datum<Id_datum,v<T,unit>,vec<>>& x)
{
  auto f=x;
  return Datum(Id_datum{},
               Derivative(std::move(f.value()),Data(Id_data{},self_derivative(x))));
}


template<class I,class... Ds>
auto  Self_Derivative(const  Data<I,Ds...>& x)
{
  return Data(I{},Self_Derivative(I{},x[typename Ds::myId{}])...);
}







//-------------------    ADDition-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator +( Derivative<v<T,unit>,Data<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data<Id,Ds...>>&& two)
{
  return Derivative<v<T,unit>,Data<Id,Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator +( Derivative<v<T,unit>,Data<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator +( Derivative<v<T,unit>,Data<Id,Ds...>>&& one,  v<T,unit>&& two)
{
  one.f()+=std::move(two);
  return one;
}

template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator +(   v<T,unit>&& two,Derivative<v<T,unit>,Data<Id,Ds...>>&& one)
{
  std::move(one)+std::move(two);
}

//----logv addition



template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data<Id,Ds...>>
operator +( Derivative<logv<T,units...>,Data<Id,Ds...>>&& one,  Derivative<logv<T,units...>,Data<Id,Ds...>>&& two)
{
  return Derivative<logv<T,units...>,Data<Id,Ds...>>(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}

template<class T, class... unit,class... unit2,class Id,class... Ds, class ...Ds2>
auto
operator +( Derivative<logv<T,unit...>,Data<Id,Ds...>>&& one,  Derivative<logv<T,unit2...>,Data<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+std::move(two.f()),std::move(one.Df())+std::move(two.Df()));
}







//-------------------    SUBSTRACTION-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator -(  Derivative<v<T,unit>,Data<Id,Ds...>> one)
{
  one.f()=-one.f();
  one.Df()=-std::move(one.Df());
  return one;
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator -( Derivative<v<T,unit>,Data<Id,Ds...>> one,  v<T,unit> two)
{
  one.f()-=std::move(two);
  return one;
}


template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator -(   v<T,unit> two,Derivative<v<T,unit>,Data<Id,Ds...>> one)
{
  return (-std::move(one))+std::move(two);
}





template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator -( Derivative<v<T,unit>,Data<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data<Id,Ds...>>&& two)
{
  return Derivative<v<T,unit>,Data<Id,Ds...>>(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator -( Derivative<v<T,unit>,Data<Id,Ds...>>&& one,  Derivative<v<T,unit>,Data<Id,Ds2...>>&& two)
{
  return Derivative(std::move(one.f())+(-std::move(two.f())),std::move(one.Df())+(-std::move(two.Df())));
}


// --------------operations on logv


template<class T, class... unit,class Id,class... Ds, class ...Ds2>
auto
operator -(Derivative<logv<T,unit...>,Data<Id,Ds...>> one, Derivative<v<T,dimension_less>,Data<Id,Ds2...>> two)
{
  return Derivative(one.f()-two.f(),std::move(one.Df())-std::move(two.Df()));
}




template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data<Id,Ds...>>
operator -( Derivative<v<T,dimension_less>,Data<Id,Ds...>> one,  logv<T,units...> two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}


template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data<Id,Ds...>>
operator -(  logv<T,units...> two, Derivative<v<T,dimension_less>,Data<Id,Ds...>> one)
{
  auto f=two-one.f();
  return Derivative<logv<T,units...>,Data<Id,Ds...>>(std::move(f),-std::move(one.Df()));
}



template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data<Id,Ds...>>
operator -( Derivative<logv<T,units...>,Data<Id,Ds...>> one,  double two)
{
  auto f=one.f()-two;
  return Derivative(std::move(f),std::move(one.DF()));
}

template<class T, class... units,class Id,class... Ds>
Derivative<logv<T,units...>,Data<Id,Ds...>>
operator -(double one,Derivative<logv<T,units...>,Data<Id,Ds...>> two)
{
  auto f=one-two.f();
  return Derivative(std::move(f),-std::move(two.Df()));
}




//-------------------   MULTIPLICATION-------------//



template<class T, class unit,class Id,class... Ds>
Derivative<v<T,unit>,Data<Id,Ds...>>
operator *( const Derivative<v<T,unit>,Data<Id,Ds...>>& one,  const Derivative<v<T,unit>,Data<Id,Ds...>>& two)
{
  return Derivative<v<T,unit>,Data<Id,Ds...>>(
      one.f()*two.f(),
      one.Df()*two.f()+one.f()*two.Df());
}

template<class T, class unit,class Id,class... Ds, class ...Ds2>
auto
operator *(const  Derivative<v<T,unit>,Data<Id,Ds...>>& one,  const Derivative<v<T,unit>,Data<Id,Ds2...>>& two)
{

  return Derivative(one.f()+two.f(),one.Df()*two.f()+one.f()*two.Df());
}



template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,Data<Id,Ds...>>
operator *( const Derivative<v<T,unit>,Data<Id,Ds...>>& one, const v<T,unit2>& two)
{
  return Derivative<v<T,decltype(unit{}*unit2{})>,Data<Id,Ds...>>(
      one.f()*two,
      one.Df()*two);
}

template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit2{})>,Data<Id,Ds...>>
operator *(const v<T,unit2>& two, const Derivative<v<T,unit>,Data<Id,Ds...>>& one)
{
  return one*two;
}


//--------------------------------DIVISION--------------------------//


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>
operator /( const Derivative<v<T,unit>,Data<Id,Ds...>>& one,  const Derivative<v<T,unit2>,Data<Id,Ds...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>(std::move(f),std::move(Df));
}

template<class T, class unit,class unit2,class Id,class... Ds, class ...Ds2>
auto
operator /(const  Derivative<v<T,unit>,Data<Id,Ds...>>& one,  const Derivative<v<T,unit2>,Data<Id,Ds2...>>& two)
{
  auto f=one.f()/two.f();
  auto Df=one.Df()/two.f()+f/two.f()*two.Df();
  return Derivative(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>
operator /( const Derivative<v<T,unit>,Data<Id,Ds...>>& one,  const v<T,unit2>& two)
{
  auto f=one.f()/two;
  auto Df=one.Df()/two;


  return Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>(std::move(f),std::move(Df));
}


template<class T, class unit,class unit2,class Id,class... Ds>
Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>
operator /( const v<T,unit>& one,  const Derivative<v<T,unit2>,Data<Id,Ds...>>& two)
{
  auto f=one/two.f();
  auto Df=f/two.f()*two.Df();
  return Derivative<v<T,decltype(unit{}/unit2{})>,Data<Id,Ds...>>(std::move(f),std::move(Df));
}




//----------------------SQR

template<class T, class unit,class Id,class... Ds>
Derivative<v<T,decltype(unit{}*unit{})>,Data<Id,Ds...>>
sqr( const Derivative<v<T,unit>,Data<Id,Ds...>>& x)
{
  auto f=sqr(x.f());
  auto Df=2.0*x.f()*x.Df();
  return Derivative<v<T,decltype(unit{}*unit{})>,Data<Id,Ds...>>(std::move(f),std::move(Df));

}


template<class T, class unit,class Id,class... Ds>
Derivative<logv<T,unit>,Data<Id,Ds...>>
log( const Derivative<v<T,unit>,Data<Id,Ds...>>& x)
{
  using std::log;
  auto f=log(x.f());
  auto Df=x.Df()/x.f();
  return Derivative(std::move(f),std::move(Df));

}






//} // namespace qm

#endif // QM_DERIVATIVE_H
