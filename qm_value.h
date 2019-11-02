#ifndef QM_VALUE_H
#define QM_VALUE_H

#include "qm_unit.h"
#include <cassert>
template<class, class> struct v;
template<class, class> class logv;

struct value_k{ constexpr static auto  className=my_static_string("value");};

struct unit_k{ constexpr static auto  className=my_static_string("unit");};


template<class TYPE,class myunit> struct v
{
private:
  TYPE value_;
public:
  typedef TYPE T;
  typedef myunit unit;
  typedef v element_type;


  using cols=Cs<value_k>;
  using cols_w_unit=Cs<value_k,unit_k>;

  using row_type=std::tuple<TYPE>;
  using row_type_w_unit=std::tuple<TYPE,myunit>;

  explicit constexpr v(TYPE&& x,myunit): value_{std::move(x)}{}
  explicit v(TYPE&& x): value_{std::move(x)}{}
  explicit v(row_type&& x):value_{std::get<TYPE>(std::move(x))}{}
  explicit v(row_type_w_unit&& x):value_{std::get<TYPE>(std::move(x))}{}
  v()=default;
  constexpr const TYPE& value()const &{return value_;}
  constexpr TYPE& value() &{return value_;}
  constexpr TYPE value()&& {return value_;}

  auto& operator[](value_k){return value();}
  auto& operator[](value_k)const {return value();}
  auto operator[](unit_k)const {return unit{};}

  template<class...Is>
  void insert_at(const Position<Is...>& , row_type&& r)
  {
    value()=std::get<TYPE>(std::move(r));
  }
  template<class...Is>
  void insert_at(const Position<Is...>& , row_type_w_unit&& r)
  {
    value()=std::get<TYPE>(std::move(r));
  }
  template<class...Is, class ...us, typename=std::enable_if_t<is_in_pack<myunit,us...>(),int>>
  void insert_at(const Position<Is...>& , std::tuple<TYPE,std::variant<us...>>&& r)
  {
    assert(std::get<myunit>(std::get<1>(r))==myunit{});
    value()=std::get<TYPE>(std::move(r));
  }


  static constexpr auto begin() {return Position<>{};}

  friend constexpr auto begin(const v& ){ return Position<>{};}

  bool next(Position<>& )const {return false;}

  friend inline bool operator==(const v& one, const v& two) { return one.value()==two.value();}






  template<class... Ts>
  friend  Position<Ts...>& operator +( Position<Ts...>& p,const v& )
  {
    return p;
  }
  friend  bool operator +(bool p,const v& )
  {
    return p;
  }



  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const { return *this;}


  template<class I>
  auto& operator()(const I& , value_k){ return value();}

  template<class I>
  auto& operator()(const I& , value_k)const{ return value();}

  template<class I>
  unit operator()(const I& , unit_k)const { return unit{};}





  friend std::ostream& operator<<(std::ostream& os, const v& m)
  {
    return os<<std::setprecision(std::numeric_limits<TYPE>::digits10+2)<<m.value()<<" "<<myunit{};
  }
  v& operator+=(const v& other){value_+=other.value(); return *this;}

  friend std::istream& operator>>(std::istream& is,  v& m)
  {
    return is>>m.value()>>myunit{};
  }

};

template<class varName>
using v_t=v<typename varName::T, typename varName::unit>;


template<class u>
struct logv_unit
{
public:
  using unit=u;
  auto operator()()const {return unit{};}
  inline static constexpr auto className=my_static_string("log(")+unit::className+my_static_string(")");

  logv_unit()=default;
  friend bool operator==(const logv_unit& me, const logv_unit& ti)
  {
    return true;
  }

  friend  logv_unit<decltype (dimension_less{}/u{})> operator-(const logv_unit&){return logv_unit<decltype (dimension_less{}/u{})>{};}

  template<class u2>
  friend bool operator==(const logv_unit& me, const logv_unit<u2>& ti){return false;}

};
template<class...>
struct logv_units;

template<class u,class...us>
struct logv_units<u,us...>
{
private:
  std::tuple<std::pair<long,u>,std::pair<long,us>...> units;
public:
  auto& operator()(){return units;}
  auto const& operator()()const {return units;}

  logv_units()=default;
  logv_units(std::pair<long,u> myunit,std::pair<long,us>... myunits):units{std::move(myunit),std::move(myunits)...}{}
  template<class u0>
  long get_number()const
  {
    if constexpr (std::is_same_v<u0,u >||is_in_pack<u0,us...>())
      return std::get<std::pair<long,u0>>(units).first;
    else
      return 0;
  }
  template<class u0>
  long& get(){return std::get<std::pair<long,u0>>(units).first;}
  friend std::ostream& operator<<(std::ostream& os, const logv_units& m)
  {
    std::apply([&os](auto&... p){
      ((os<<"+"<<p.first<<".log("<<p.second<<")"),...);},m.units);
    return os;
  }

  friend std::istream& operator>>(std::istream& is,  logv_units& m)
  {
    std::string s;
    is>>s;
    auto s_copy=s;
    std::stringstream ss(s);
    std::apply([&ss](auto&... p){
      ((ss>>
        my_static_string("+")
        >>p.first>>
        my_static_string(".log(")
        >>
        my_static_string(p.second.className)>>
        my_static_string(")")
        ),...);},m.units);
    return is;
  }

  friend bool operator==(const logv_units& me, const logv_units& ti)
  {
    return     apply_twin([](auto... res){return (true&&...&&res);},
                      [](auto p, auto t){ return (p.first==t.first)&&(p.second==t.second);},
                      me.units,ti.units);

  }

};





template<class ...u1, class... u2>
auto operator +(const logv_units<u1...>& x,const logv_units<u2...>& y )
{
  typedef transfer_t<pack_difference_t<Cs<u2...>,Cs<u1...>>,logv_units<u1...>> res_type;
  res_type out;
  ((out.template get<u1>()+=x.template get_number<u1>()),...);
  ((out.template get<u2>()+=y.template get_number<u2>()),...);
  return out;

}



template<class T,class Unit> class logv//<T,logv_unit<Unit>>
{
private:
  T value_;
public:
  using unit=Unit;
  using logunit=logv_unit<unit>;
  using cols=Cs<value_k>;
  using row_type=std::tuple<T>;
  using cols_w_unit=Cs<value_k,unit_k>;
  using row_type_w_unit=std::tuple<T,logunit>;

  typedef logv element_type;

  logv(T&& x): value_{std::move(x)}{}
  logv(T&& x,unit): value_{std::move(x)}{}
  logv()=default;
  const T& value()const &{return value_;}
  T& value() &{return value_;}
  T value()&& {return value_;}
  static constexpr auto begin() {return Position<>{};}

  friend constexpr auto begin(const logv& ){ return Position<>{};}

  bool next(Position<>& )const {return false;}


  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const { return *this;}



  template<class I>
  auto& operator()(const I& , value_k){ return value();}

  template<class I>
  auto& operator()(const I& , value_k)const{ return value();}


  template<class I>
  auto operator()(const I& , unit_k)const  { return unit{};}


  template<class...Is>
  void insert_at(const Position<Is...>& , row_type&& r)
  {
    value()=std::get<T>(std::move(r));
  }
  template<class...Is>
  void insert_at(const Position<Is...>& , row_type_w_unit&& r)
  {
    value()=std::get<T>(std::move(r));
  }



  friend std::ostream& operator<<(std::ostream& os, const logv& m)
  {
    os<<m.value();
    os<<unit{};
    return os;
  }
  friend std::istream& operator>>(std::istream& is, const logv& m)
  {
    is>>m.value();
    is>>m.units();
    return is;
  }

  friend bool operator==(const logv& me, const logv& ti)
  {
    return (me.value()==ti.value());
  }



  friend logv<T,decltype (dimension_less{}/unit{})> operator-(const logv& me)
  {
    return  logv<T,decltype (dimension_less{}/unit{})>(-me.value());
  }

  friend logv<T,decltype (dimension_less{}/unit{})> operator-(double a, logv me)
  {
    logv<T,decltype (dimension_less{}/unit{})> out(-me.value());
    out.value()+=a;
    return out;
  }
  friend logv operator-(logv me, v<double,dimension_less> a)
  {
    me.value()-=a.value();
    return me;
  }
  friend v<T,dimension_less> operator-(logv me, logv a)
  {
    return v<T,dimension_less>(me.value()-a.value());
  }





};

template<class T,class... Units> class logvs
{
private:
  T value_;
  logv_units<Units...> us_;

public:
  using cols=Cs<value_k>;
  using row_type=std::tuple<T>;
  using cols_w_unit=Cs<value_k,unit_k>;
  using row_type_w_unit=std::tuple<T,logv_units<Units...>>;


  typedef logvs element_type;

  logvs(T&& x,std::pair<long,Units>... us): value_{std::move(x)}, us_{std::move(us)...}{}
  logvs(T&& x,logv_units<Units...>&& us): value_{std::move(x)}, us_{std::move(us)}{}

  template<class unit>
  logvs(logv<T,unit>&& x):value_{std::move(x).value()},us_{{1,unit{}}}{}
  template<class unit, typename =std::enable_if_t<std::is_same_v<unit,dimension_less >,int>>
  logvs(v<T,unit>&& x):value_{std::move(x).value()},us_{{1,unit{}}}{}
  logvs()=default;
  const T& value()const &{return value_;}
  T& value() &{return value_;}
  T value()&& {return value_;}
  auto& size()const {return us_();}
  auto& units()const {return us_;}
  auto& units() {return us_;}
  auto& size() {return us_();}
  static constexpr auto begin() {return Position<>{};}

  friend constexpr auto begin(const logvs& ){ return Position<>{};}

  bool next(Position<>& )const {return false;}


  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const { return *this;}



  template<class I>
  auto& operator()(const I& , value_k){ return value();}

  template<class I>
  auto& operator()(const I& , value_k)const{ return value();}

  template<class I>
  auto& operator()(const I& , unit_k)const { return units();}

  template<class I>
  auto& operator()(const I& , unit_k) { return units();}


  template<class...Is>
  void insert_at(const Position<Is...>& , row_type&& r)
  {
    value()=std::get<T>(std::move(r));
  }
  template<class...Is>
  void insert_at(const Position<Is...>& , row_type_w_unit&& r)
  {
    value()=std::get<T>(std::move(r));
    units()=std::get<logv_units<Units...>>(std::move(r));
  }



  friend std::ostream& operator<<(std::ostream& os, const logvs& m)
  {
    os<<m.value();
    os<<m.units();
    return os;
  }
  friend std::istream& operator>>(std::istream& is, const logvs& m)
  {
    is>>m.value();
    is>>my_static_string("+");
    is>>m.units();
    return is;
  }

  friend bool operator==(const logvs& me, const logvs& ti)
  {
    return (me.value()==ti.value())&&(me.units()==ti.units());
  }



  friend logvs operator-(logvs&& me)
  {
    me.value_=-me.value_;
    me.size()=std::apply([](auto&&...p)
                           {return std::make_tuple(std::make_pair(-p.first,p.second)...);},
                           std::move(me.size()));
    return std::move(me);
  }
  friend logvs operator-(const logvs& x)
  {
    logv me=x;
    return -std::move(me);
  }


  friend logvs operator-(double a, logvs me)
  {
    me=-me;
    me.value_+=a;
    return me;
  }
  friend logvs operator-(logvs me, v<double,dimension_less> a)
  {
    me.value_-=a.value();
    return me;
  }
  friend v<T,dimension_less> operator-(logvs me, logvs a)
  {
    return v<T,dimension_less>(me.value()-a.value());
  }





};



template<class T,class... units> logv(T,p<units...>)->logv<T,p<units...>>;

template<class T,class... Units> logvs(T&& x,logv_units<Units...>&& us)->logvs<T,Units...>;

template<class u1>
auto operator+(const logv<double,u1>& one, const logv<double,u1>& two)
{
  return logvs<double,u1>(one.value()+two.value(),{2,u1{}});
}




template<class T,class... units1, class ...units2>
auto operator+(const logvs<T,units1...>& me, const logvs<T,units2...>& ti)
{
  return logvs(me.value()+ti.value(),me.units()+ti.units());

}

template<class T,class... units1, class unit2>
auto operator+(const logvs<T,units1...>& me, const logv<T,unit2>& ti)
{
  return logvs(me.value()+ti.value(),me.units()+logv_units<unit2>(std::pair<std::size_t,unit2>(1,unit2{})));

}



template <class T, class unit>
auto pow(double base,const logv<T,unit>& me)
{
  return v(std::pow(base,me.value()),unit{});
}



template <class T>
v(T&& x)->v<T,dimension_less>;
template<class T,class unit>
v(T&& x,unit)->v<T,unit>;

template<class T,class unit>
logvs(logv<T,unit>&&)->logvs<T,unit>;
template<class T>
logvs(v<T,dimension_less>&&)->logvs<T,dimension_less>;


template <class T, class U,class unit1, class unit2>
constexpr auto operator*( const v<T,unit1>& one, const  v<U,unit2>& other)
{
  return v(one.value()*other.value(),unit1{}*unit2{});
}


template <class T, class unit1>
auto operator*( const v<T,unit1>& one, double other)
{
  return v(one.value()*other,unit1{});
}
template <class T, class unit1>
auto operator*(  double other,const v<T,unit1>& one)
{
  return one*other;
}


template <class T, class U,class unit1, class unit2>
constexpr auto operator/( const v<T,unit1>& one, const  v<U,unit2>& other)
{
  return v(one.value()/other.value(),unit1{}/unit2{});
}

template <class T, class U,class unit1>
auto operator+( const v<T,unit1>& one, const  v<U,unit1>& other)
{
  return v(one.value()+other.value(),unit1{});
}




template <class T, class U,class unit1>
auto operator-( const v<T,unit1>& one, const  v<U,unit1>& other)
{
  return v(one.value()-other.value(),unit1{});
}


template <class T, class unit1>
auto operator-( const v<T,unit1>& one)
{
  return v(-one.value(),unit1{});
}

template <class T,class unit1>
auto sqr(const v<T,unit1>& x) { return x * x; }



inline v<double,dimension_less> exp(const v<double,dimension_less>& x)
{
  return v<double,dimension_less>(std::exp(x.value()));
}

template <class unit1>
auto exp(const v<double,logv_unit<unit1>>& x) { return v<double,unit1>(std::exp(x.value())); }


inline v<double,dimension_less> cos(const v<double,dimension_less>& x)
{
  return v<double,dimension_less>(std::cos(x.value()));
}


inline v<double,dimension_less> operator-(double x, const v<double,dimension_less>& y){return v(x-y.value());}
inline v<double,dimension_less> operator-(const v<double,dimension_less>& x,double y){return v(x.value()-y);}


template <class unit1>
auto sqrt(const v<double,unit1>& x) { return v(std::sqrt(x.value()), pow_inv<2>(unit1{})); }

template <class unit1>
auto log(const v<double,unit1>& x) { return logv(std::log(x.value()),unit1{}); }

inline auto log(const v<double,dimension_less>& x) { return v(std::log(x.value()),dimension_less{}); }


template <class unit1>
auto pow(double base,const v<double,logv_unit<unit1>>& x) {
  using t=typename decltype (x)::tty;
  return v<double,unit1>(pow(base,x.value())); }

inline auto pow(double base,const v<double,dimension_less>& x) { return v<double,dimension_less>(pow(base,x.value())); }


template<class ei>
struct Log10_t{
  using T=typename ei::T;
  using unit=typename ei::unit;
  constexpr static auto className=my_static_string("log10_")+ei::className;
};

struct Log10_rev{
  template<typename T>
  auto operator()(const T& x)const {
    //using d=typename T::T;
    using std::pow;
    return pow(10.0,x);}
};




#endif // QM_VALUE_H
