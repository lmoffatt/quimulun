#ifndef QM_TENSOR_COORDINATE_H
#define QM_TENSOR_COORDINATE_H
#include "qm_vector_field_declaration.h"


template<class Id> struct Size{
  //typedef typename Id::T T;
  //typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_size");

};


template<class Id> struct Start{
  //typedef typename Id::T T;
 // typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_start");

};

template<class Id> struct End{
 // typedef typename Id::T T;
 // typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_end");

};

template<class Id> struct Duration{
 // typedef typename Id::T T;
 // typedef typename Id::unit unit;

  constexpr static auto  className=Id::className+my_static_string("_end");

};
template<class Id> struct Step{
 // typedef typename Id::T T;
 // typedef typename Id::unit unit;

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


struct IndexCoordinate
{

  template<class T, class unit>
  auto operator()(v<T,unit> start, v<T,unit> end, v<T,unit> step=v<T,unit>(0))const
  {
    auto duration=end-start;
    auto n=duration/step;
    std::vector<v<T,unit>> out;
    out.reserve(n.value());
    for (auto x=start; x.value()<start.value()+duration.value(); x=x+step)
      out.push_back(x);

    return out;
  }
  template<class T, class unit>
  auto operator()(v<T,unit> duration)const
  {
    auto n=duration.value();
    std::vector<v<T,unit>> out;
    out.reserve(n);
    for (T i=0; i<n; ++i)
      out.push_back(v<T,unit>(i));

    return out;
  }


};


/*

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

*/



template<class Id,class G, class... Xs>
class  Coord
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class... Data>
  auto operator()(const Data&... data)const
  {
    using res_value_type=std::decay_t<
        std::invoke_result_t<G,decltype(get_from(Xs{},data...)(get_from(Xs{},data...).begin()))...>>;

//    typedef typename decltype(g_(std::declval<typename std::decay_t<decltype(get_from(Xs{},data...))>::element_type>()...))::value_type value_type;

    //typedef typename value_type::sgser dgewr;

    //using test=typename res_value_type::value_test;

    auto out=consolidate<element_type_t<res_value_type>>(vec<Id>{},get_from(Xs{},data...)...);

    //using test2=typename decltype(out)::out_test;

    auto p=out.begin(Id{});

    do {
      out(Id{},p)=g_(get_from(Xs{},data...)(p)...);

    } while (out.next(p));
    return x_i(Id{},std::move(out));
  }


  Coord(Id ,G&& g, Xs&&...):g_{std::move(g)}{}
};


template<class...> class Coord_new;

template<class Id,class G, class... Xs>
class  Coord_new<Id,G,Arguments<Xs...>>
{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  auto& get_G()const {return  g_;}

  Coord_new(Id&& ,G&& g, Arguments<Xs...>&&):g_{std::move(g)}{}
};

template<class Id,class G, class... Xs> Coord_new(Id&& ,G&& g, Arguments<Xs...>&&)->Coord_new<Id,G,Arguments<Xs...>>;




template <class...> struct extract_function_Id;
template <class C> struct extract_function_Id<C>
{using type=Cs<>;};

template< class Id,class G, class... Xs>
struct extract_function_Id<Coord<Id,G,Xs...>>{using type=Cs<Id>;};



template<class C> using extract_function_Id_t=typename extract_function_Id<C>::type;

template< class... Ids>
struct extract_function_Id<Cs<Ids...>>{using type=pack_concatenation_t<extract_function_Id_t<Ids>...>;};

template< class... Ids>
struct extract_function_Id{using type=pack_concatenation_t<extract_function_Id_t<Ids>...>;};





template<class Id,class G, class... Xs, class... Datas>
auto calculate(const Coord<Id,G,Xs...>& f,const Datas&... d)
{
  auto res=(...&&find_in(Xs{},d...));
  if constexpr (!std::is_same_v<decltype (res),Has_been_found >)
  {
    return Error(Arguments_not_found(Id{},res));

  }else

    return f(d...);
}

/*
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

*/
template<class Id, class G, class...Xs> Coord(Id&&,G&&,Xs&&...)->Coord<Id,G,Xs...>;











#endif // QM_TENSOR_COORDINATE_H
