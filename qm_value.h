#ifndef QM_VALUE_H
#define QM_VALUE_H

#include "qm_unit.h"
#include <cassert>
template<class ...> struct v;
template<class ...> class logv;

struct value_k{ constexpr static auto  className=my_static_string("value");};

struct unit_k{ constexpr static auto  className=my_static_string("unit");};


template<class TYPE,class myunit> struct v<TYPE,myunit>
{
private:
  TYPE value_;
public:
  typedef TYPE T;
  typedef myunit unit;
  typedef v element_type;


  using cols=Cs<value_k,unit_k>;

  using row_type=std::tuple<TYPE,myunit>;

  explicit v(TYPE&& x,myunit): value_{std::move(x)}{}
  explicit v(TYPE&& x): value_{std::move(x)}{}
  explicit v(row_type&& x):value_{std::get<TYPE>(std::move(x))}{}
  v()=default;
  const TYPE& value()const &{return value_;}
  TYPE& value() &{return value_;}
  TYPE value()&& {return value_;}

  auto& operator[](value_k){return value();}
  auto& operator[](value_k)const {return value();}
  auto operator[](unit_k)const {return unit{};}

  template<class...Is>
  void insert_at(const Position<Is...>& , row_type&& r)
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

template<class...us>
struct logv_units
{
private:
  std::tuple<std::pair<long,us>...> units;
public:
  auto& operator()(){return units;}
  auto const& operator()()const {return units;}

  logv_units()=default;
  logv_units(std::pair<long,us>... myunits):units{std::move(myunits)...}{}
  template<class u>
  long get_number()const
  {
    if constexpr (is_in_pack<u,us...>()) return std::get<std::pair<long,u>>(units).first;
    else return 0;
  }
  template<class u>
  long& get(){return std::get<std::pair<long,u>>(units).first;}
  friend std::ostream& operator<<(std::ostream& os, const logv_units& m)
  {
    std::apply([&os](auto&... p){
      ((os<<p.first<<"*log("<<p.second<<")"),...);},m.units);
    return os;
  }

  friend std::istream& operator>>(std::istream& is,  logv_units& m)
  {
    std::string s;
    is>>s;
    auto s_copy=s;
    std::stringstream ss(s);
    std::apply([&ss](auto&... p){
      ((ss>>p.first>>
        my_static_string("*log(")
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





template<class T,class... Units> class logv<T,Units...>
{
private:
  T value_;
  logv_units<Units...> us_;


public:
  using cols=Cs<value_k,unit_k>;
  using row_type=std::tuple<T,logv_units<Units...>>;

  logv(T&& x,std::pair<long,Units>... us): value_{std::move(x)}, us_{std::move(us)...}{}
  logv(T&& x,logv_units<Units...>&& us): value_{std::move(x)}, us_{std::move(us)}{}
  logv()=default;
  const T& value()const &{return value_;}
   T& value() &{return value_;}
  T value()&& {return value_;}
  auto& size()const {return us_();}
  auto& units()const {return us_;}
  auto& units() {return us_;}
  auto& size() {return us_();}
  static constexpr auto begin() {return Position<>{};}

  friend constexpr auto begin(const logv& ){ return Position<>{};}


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
    units()=std::get<logv_units<Units...>>(std::move(r));
  }



  friend std::ostream& operator<<(std::ostream& os, const logv& m)
  {
    os<<m.value();
    os<<m.units();
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
    return (me.value()==ti.value())&&(me.units()==ti.units());
  }



  friend logv operator-(logv&& me)
  {
    me.value_=-me.value_;
    me.size()=std::apply([](auto&&...p)
                           {return std::make_tuple(std::make_pair(-p.first,p.second)...);},
                           std::move(me.size()));
    return std::move(me);
  }
  friend logv operator-(const logv& x)
  {
    logv me=x;
    return -std::move(me);
  }


  friend logv operator-(double a, logv me)
  {
    me=-me;
    me.value_+=a;
    return me;
  }
  friend logv operator-(logv me, v<double,dimension_less> a)
  {
    me.value_-=a.value();
    return me;
  }




};

template<class T,class... units> logv(T,logv_units<units...>)->logv<T,units...>;


template<class T,class... units1, class ...units2>
auto operator+(const logv<T,units1...>& me, const logv<T,units2...>& ti)
{
  return logv(me.value()+ti.value(),me.units()+ti.units());

}



template <class T>
v(T&& x)->v<T,dimension_less>;
template<class T,class unit>
v(T&& x,unit)->v<T,unit>;



template <class T, class U,class unit1, class unit2>
auto operator*( const v<T,unit1>& one, const  v<U,unit2>& other)
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
auto operator/( const v<T,unit1>& one, const  v<U,unit2>& other)
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
auto sqrt(const v<double,unit1>& x) { return v<double,div_exponent_t<unit1,2>>(std::sqrt(x.value())); }

template <class unit1>
auto log(const v<double,unit1>& x) { return logv<double,unit1>(std::log(x.value()),{1,unit1{}}); }








#endif // QM_VALUE_H
