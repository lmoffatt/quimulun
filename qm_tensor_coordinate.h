#ifndef QM_TENSOR_COORDINATE_H
#define QM_TENSOR_COORDINATE_H
#include "qm_vector_field.h"




template<class Id> struct Start{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_start");

};
template<class Id> struct Duration{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_end");

};
template<class Id> struct Step{
  typedef typename Id::T T;
  typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_step");

};



struct EvenCoordinate
{

  template<class T, class unit>
  auto operator()(v<T,unit> start, v<T,unit> duration, v<T,unit> step)const
  {
    auto n=duration/step;
    std::vector<v<T,unit>> out;
    out.reserve(n.value());
    for (auto x=start; x.value()<start.value()+duration.value(); x=x+step)
      out.push_back(x);

    return out;
  }


};



template<class Id> class UnEvenCoordinate
{
public:
  typedef typename Id::T T;
  typedef typename Id::unit unit;
private:private:

  std::map<T, std::size_t> m_;
  std::vector<T> v_;

  std::map<T, std::size_t> vector_to_map(const std::vector<T>& v)
  {
    std::map<T, std::size_t> out;
    for (std::size_t i=0; i<v.size(); ++i)
      out[v[i]]=i;
    return out;
  }
public:
  UnEvenCoordinate(v<std::vector<T>,unit>&& c): m_{vector_to_map(c)},v_{std::move(c).value()}{}
  v<T,unit> start()const {return v_[0];}
  v<T,unit> end()const {return v_.back();}

  auto nsteps()const {return v_.size();}
  v<T,unit> operator()(std::size_t i)const {return v_[i];}

  std::size_t index(const v<T,unit>& x)
  {
    auto it= m_.lower_bound(x);
    return it->second;
  }

};





template<class Id,class G, class... Xs>
class  Coord
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}


  template<class Param>
  auto operator()(const Param& par)const
  {
    typedef typename decltype(g_(std::declval<typename std::decay_t<decltype(par[Xs{}])>::element_type>()...))::value_type value_type;

    //typedef typename value_type::sgser dgewr;

    auto out=consolidate<value_type>(vec<Id>{},par[Xs{}]...);
   // typedef typename decltype (out)::gds ger;
    auto p=out.begin(Id{});

    do {
      out(Id{},p)=g_(par[Xs{}](p)...);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
  }

  template<class... Data>
  auto operator()(const Data&... data)const
  {
    typedef typename decltype(g_(std::declval<typename std::decay_t<decltype(get_from<Xs>(data...))>::element_type>()...))::value_type value_type;

    //typedef typename value_type::sgser dgewr;

    auto out=consolidate<value_type>(vec<Id>{},get_from<Xs>(data...)...);
    auto p=out.begin(Id{});

    do {
      out(Id{},p)=g_(get_from<Xs>(data...)(p)...);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
  }


  Coord(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
};


template<class Id,class G, class... Xs, class... Datas>
auto calculate(const Coord<Id,G,Xs...>& f,const Datas&... d)
{
  if constexpr ((std::is_same_v<decltype (get_from<Xs>(d...)),Nothing>||...))
    return Nothing{};
  else
    return f(d...);
}

template<class Id,class G, class... Xs,class Rnd, class Datas>
auto sample(const Coord<Id,G,Xs...>& f,const Datas& d,Rnd& )
{
  return calculate(f,d);
}



template<class Id,class G, class... Xs,class... Datas>
auto logP(const Coord<Id,G,Xs...>& ,const Datas&...)
{
  return logP_zero{};
}

template<class Id,class G, class... Xs,class... Datas>
auto FIM(const Coord<Id,G,Xs...>& ,const Datas&... )
{
  return logP_zero{};
}


template<class Id, class G, class...Xs> Coord(Id&&,G&&,Xs&&...)->Coord<Id,G,Xs...>;











#endif // QM_TENSOR_COORDINATE_H
