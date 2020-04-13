#ifndef QM_VECTOR_FIELD_EVALUATION_H
#define QM_VECTOR_FIELD_EVALUATION_H
#include <qm_vector_field_declaration.h>



template<class F, class... values_t>
auto apply(F&& f,
            values_t&&... ti)
{
  typedef decltype ((...<<get_Field_Indexes_t<values_t>{})) myvec;
  if constexpr (myvec::index_size==0)
  {
//    auto p=Position<>{};
//    return std::invoke(std::forward<F>(f), ti(p)...);
return std::invoke(std::forward<F>(f), std::forward<values_t>(ti)...);
  }
  else
  {
    using res_value_type=std::decay_t<std::invoke_result_t<F,decltype( std::forward<values_t>(ti)(ti.begin()))...>>;
    typedef vector_field<myvec,res_value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type outf;
    auto p=myField::begin();

    fill_vector_field(outf,p,myvec{}, std::forward<values_t>(ti)...);

    myField out(std::move(outf));
    p=out.begin();
    do
    {
      out(p)=std::invoke(std::forward<F>(f),  std::forward<values_t>(ti)(p)...);
    } while(out.next(p));
    return out;
  }
}

template<class F, class random,class... values_t>
auto apply_random(F&& f,random& mt,const values_t&... ti)
{
  typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
  if constexpr (myvec::index_size==0)
  {
       auto p=Position<>{};
    //    return std::invoke(std::forward<F>(f), ti(p)...);
       return std::invoke(std::forward<F>(f), mt(p),ti(p)...);
  }
  else
  {
    using res_value_type=std::decay_t<std::invoke_result_t<F,decltype(mt(mt.begin())),decltype(ti(ti.begin()))...>>;
    typedef vector_field<myvec,res_value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type outf;
    auto p=myField::begin();

    fill_vector_field(outf,p,myvec{},mt,ti...);

    myField out(std::move(outf));
    p=out.begin();
    do
    {
      out(p)=std::invoke(std::forward<F>(f), mt(p),ti(p)...);
    } while(out.next(p));
    return out;
  }
}


template<class Op,class F, class random,class... values_t>
auto apply_op(Op op,F&& f,random& mt,const values_t&... ti)
{
  typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
  if constexpr (myvec::index_size==0)
  {
    //    auto p=Position<>{};
    //    return std::invoke(std::forward<F>(f), ti(p)...);
    return myInvoke(op,std::forward<F>(f), mt,ti...);
  }
  else
  {
    using res_value_type=std::decay_t<decltype(myInvoke(op,std::forward<F>(f),mt(mt.begin()),ti(ti.begin())...))>;
    typedef vector_field<myvec,res_value_type> myField;

    typedef typename myField::value_type myValue_type;

    myValue_type outf;
    auto p=myField::begin();

    fill_vector_field(outf,p,myvec{},mt,ti...);

    myField out(std::move(outf));
    p=out.begin();
    do
    {
      out(p)=myInvoke(op,std::forward<F>(f), mt(p),ti(p)...);
    } while(out.next(p));
    return out;
  }
}



template<class F, class... values_t>
auto apply_parallel_for(F&& f,
                        const values_t&... ti)
{
  using res_value_type=std::decay_t<std::invoke_result_t<F,decltype(ti(ti.begin()))...>>;
  auto out=consolidate<res_value_type>(vec<>{},ti...);
  //  using terms=typename Cs<decltype (ti)...>::types;
  //  using test=typename res_value_type::res_type;
  auto p=out.begin();

  std::vector<std::decay_t<decltype (p)>> allpos;
  do
  {
    allpos.push_back(p);
  } while(out.next(p));


  #pragma omp parallel for
  for (std::size_t i=0; i<allpos.size(); ++i)
  {
    out(allpos[i])=std::invoke(std::forward<F>(f), ti(allpos[i])...);
  }
  return out;

}



//template<class F, class random,class... values_t>
//auto apply_random(F&& f,random& mt,
//                  const values_t&... ti)
//{
//  typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
//    if constexpr (myvec::index_size==0)
//    {
//      auto p=Position<>{};
//      //std::cerr<<"apply_random: &mt="<<&mt<<"\n";
//      return std::invoke(std::forward<F>(f),mt(p),ti(p)...);
//    }
//    else
//    {
//      using res_value_type=std::decay_t<decltype (f(mt(mt.begin()),ti(ti.begin())...))>;
//      typedef vector_field<myvec,res_value_type> myField;

//      typedef typename myField::value_type myValue_type;

//      myValue_type outf;
//      auto p=myField::begin();

//      fill_vector_field(outf,p,myvec{},mt,ti...);

//      myField out(std::move(outf));
//      p=out.begin();
//      do
//      {
//        out(p)=std::invoke(std::forward<F>(f),mt(p),ti(p)...);
//      } while(out.next(p));
//      return out;
//    }
//  }


  template<class Calc, class Fi, class x_random,class... values_t>
  auto apply_random_calculate(Calc&& f,const Fi& fi,x_random&& mt,
                    const values_t&... ti)
  {
    typedef decltype ((get_Field_Indexes_t<std::decay_t<x_random>>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
    if constexpr (myvec::index_size==0)
    {
      auto p=Position<>{};
      return std::invoke(std::forward<Calc>(f),fi,std::forward<x_random>(mt)(p),ti(p)...);
    }
    else
    {
      using res_value_type=std::decay_t<decltype (std::invoke(std::forward<Calc>(f),fi,
                                                               std::forward<x_random>(mt)(std::forward<x_random>(mt).begin()),ti(ti.begin())...))>;
      typedef vector_field<myvec,res_value_type> myField;

      typedef typename myField::value_type myValue_type;

      myValue_type outf;
      auto p=myField::begin();

      fill_vector_field(outf,p,myvec{},std::forward<x_random>(mt),ti...);

      myField out(std::move(outf));
      p=out.begin();
      do
      {
        out(p)=std::invoke(std::forward<Calc>(f),fi,std::forward<x_random>(mt)(p),ti(p)...);
      } while(out.next(p));
      return out;
    }
  }


  template<class Calc, class Fi, class x_random,class... values_t>
  auto apply_random_calculate_parallel_for(Calc&& f,const Fi& fi,x_random&& mt,
                              const values_t&... ti)
  {
    typedef decltype ((get_Field_Indexes_t<std::decay_t<x_random>>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
    if constexpr (myvec::index_size==0)
    {
      auto p=Position<>{};
      return std::invoke(std::forward<Calc>(f),fi,std::forward<x_random>(mt)(p),ti(p)...);
    }
    else
    {
      using res_value_type=std::decay_t<decltype (std::invoke(std::forward<Calc>(f),fi,
                                                               std::forward<x_random>(mt)(std::forward<x_random>(mt).begin()),ti(ti.begin())...))>;
      typedef vector_field<myvec,res_value_type> myField;

      typedef typename myField::value_type myValue_type;

      myValue_type outf;
      auto p=myField::begin();

      fill_vector_field(outf,p,myvec{},std::forward<x_random>(mt),ti...);

      myField out(std::move(outf));
      p=out.begin();
      std::vector<std::decay_t<decltype (p)>> allpos;
      do
      {
        allpos.push_back(p);
      } while(out.next(p));



#pragma omp parallel for
      for (std::size_t i=0; i<allpos.size(); ++i)
      {
        out(allpos[i])=std::invoke(std::forward<Calc>(f),fi,std::forward<x_random>(mt)(allpos[i]),ti(allpos[i])...);
      }
      return out;
    }
  }



  template<class Calc, class Fi, class Posi,class random,class... values_t>
  auto apply_random_calculate_pos(Calc&& f,const Fi& fi,random&& mt,Posi&&,
                              const values_t&... ti)
  {
    typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
    if constexpr (myvec::index_size==0)
    {
      auto p=x_i(Posi{},v(Position<>{},dimension_less{}));

      return std::invoke(std::forward<Calc>(f),fi,std::forward<random>(mt)(p().value()),p,ti(p().value())...);
    }
    else
    {
      using pos_type=transfer_t<myvec,Position<>>;
      using res_value_type=std::decay_t<decltype (std::invoke(std::forward<Calc>(f),fi,
                                                               std::forward<random>(mt)(mt.begin()),
                                                               x_i(Posi{},v(pos_type{},dimension_less{})),
                                                               ti(ti.begin())...))>;
      typedef vector_field<myvec,res_value_type> myField;

      typedef typename myField::value_type myValue_type;

      myValue_type outf;
      auto p=myField::begin();

      fill_vector_field(outf,p,myvec{},std::forward<random>(mt),ti...);

      myField out(std::move(outf));
      p=out.begin();
      do
      {
        auto xp=x_i(Posi{},v(p,dimension_less{}));
        out(p)=std::invoke(std::forward<Calc>(f),fi,std::forward<random>(mt)(p),xp,ti(p)...);
      } while(out.next(p));
      return out;
    }
  }


  template<class Calc, class Fi, class Posi,class random,class... values_t>
  auto apply_random_calculate_pos_parallel_for(Calc&& f,const Fi& fi,random&& mt,Posi&&,
                                  const values_t&... ti)
  {
    typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
    if constexpr (myvec::index_size==0)
    {
      auto p=x_i(Posi{},v(Position<>{},dimension_less{}));

      return std::invoke(std::forward<Calc>(f),fi,std::forward<random>(mt)(p().value()),p,ti(p().value())...);
    }
    else
    {
      using pos_type=transfer_t<myvec,Position<>>;
      using res_value_type=std::decay_t<decltype (std::invoke(std::forward<Calc>(f),fi,
                                                               std::forward<random>(mt)(mt.begin()),
                                                               x_i(Posi{},v(pos_type{},dimension_less{})),
                                                               ti(ti.begin())...))>;
      typedef vector_field<myvec,res_value_type> myField;

      typedef typename myField::value_type myValue_type;

      myValue_type outf;
      auto p=myField::begin();

      fill_vector_field(outf,p,myvec{},std::forward<random>(mt),ti...);

      myField out(std::move(outf));
      p=out.begin();
      std::vector<std::decay_t<decltype (p)>> allpos;
      do
      {
        allpos.push_back(p);
      } while(out.next(p));


#pragma omp parallel for
      for (std::size_t i=0; i<allpos.size(); ++i)
      {
        auto xp=x_i(Posi{},v(allpos[i],dimension_less{}));
        out(allpos[i])=std::invoke(std::forward<Calc>(f),fi,std::forward<random>(mt)(allpos[i]),xp,ti(allpos[i])...);
      }
      return out;
    }
  }



template<class F, class random,class... values_t>
auto apply_random_pos(F&& f,random& mt,
                      const values_t&... ti)
{
    typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
    if constexpr (myvec::index_size==0)
    {
      auto p=Position<>{};
      return std::invoke(std::forward<F>(f),mt(p),p,ti(p)...);
    }
    else
    {
      auto p=transfer_t<myvec,Position<>>{};
      using res_value_type=std::decay_t<decltype (f(mt(mt.begin()),p,ti(ti.begin())...))>;
      typedef vector_field<myvec,res_value_type> myField;

      typedef typename myField::value_type myValue_type;

      myValue_type outf;

      fill_vector_field(outf,p,myvec{},mt,ti...);

      myField out(std::move(outf));
      p=out.begin();
      do
      {
        out(p)=std::invoke(std::forward<F>(f),mt(p),p,ti(p)...);
      } while(out.next(p));
      return out;
    }
 }

 template<class F, class random,class... values_t>
 auto apply_random_pos_parallel_for(F&& f,random& mt,
                       const values_t&... ti)
 {
   typedef decltype ((get_Field_Indexes_t<random>{}<<...<<get_Field_Indexes_t<values_t>{})) myvec;
   if constexpr (myvec::index_size==0)
   {
     auto p=Position<>{};
     return std::invoke(std::forward<F>(f),mt(p),p,ti(p)...);
   }
   else
   {
     auto p=transfer_t<myvec,Position<>>{};
     using res_value_type=std::decay_t<decltype (f(mt(mt.begin()),p,ti(ti.begin())...))>;
     typedef vector_field<myvec,res_value_type> myField;

     typedef typename myField::value_type myValue_type;

     myValue_type outf;

     fill_vector_field(outf,p,myvec{},mt,ti...);

     myField out(std::move(outf));
     p=out.begin();
     std::vector<std::decay_t<decltype (p)>> allpos;
     do
     {
       allpos.push_back(p);
     } while(out.next(p));


 #pragma omp parallel for
     for (std::size_t i=0; i<allpos.size(); ++i)
     {
       out(allpos[i])=std::invoke(std::forward<F>(f),mt(allpos[i]),allpos[i],ti(allpos[i])...);
     }
     return out;
   }
 }



template<class F, class random,class... values_t>
auto apply_random_parallel_for(F&& f,random& mt,
                               const values_t&... ti)
{
  using res_value_type=std::decay_t<decltype (f(mt(mt.begin()),ti(ti.begin())...))>;
  auto out=consolidate<res_value_type>(vec<>{},mt,ti...);
  //using test=typename res_value_type::res_type;
  // using test2=typename decltype(out)::out_type;
  auto p=out.begin();
  std::vector<std::decay_t<decltype (p)>> allpos;
  do
  {
    allpos.push_back(p);
  } while(out.next(p));


  #pragma omp parallel for
  for (std::size_t i=0; i<allpos.size(); ++i)
  {
    //std::cerr<<"&mt(allpos[i])="<<&mt(allpos[i])<<"\n";
    out(allpos[i])=std::invoke(std::forward<F>(f),mt(allpos[i]),ti(allpos[i])...);
  }
  return out;

}





template<class F, class random,class... values_t>
auto apply_sample_parallel_for(F&& f,random& mt,
                  const values_t&... ti)
{
  using res_value_type=std::decay_t<decltype(f.sample(ti(ti.begin())...,mt(mt.begin())))>;
  auto out=consolidate<res_value_type>(vec<>{},mt,ti...);
  //using test=typename res_value_type::res_type;
  // using test2=typename decltype(out)::out_type;
  auto p=out.begin();
  std::vector<std::decay_t<decltype (p)>> allpos;
  do
  {
    allpos.push_back(p);
  } while(out.next(p));


#pragma omp parallel for
  for (std::size_t i=0; i<allpos.size(); ++i)
  {
    out(allpos[i])=f.sample(ti(allpos[i])...,mt(allpos[i]));
  }
  return out;

}



template<class F, class... values_t>
auto apply_parallel(F&& f,
                    values_t&&... ti)
{
  using res_value_type=std::decay_t<std::invoke_result_t<F,typename std::decay_t<values_t>::element_type...>>;
  auto out=consolidate<res_value_type>(vec<>{},ti...);
  auto p=out.begin();
  do
  {
    out(p)=std::invoke(std::forward<F>(f), std::forward<values_t>(ti)(p)...);
  } while(out.next(p));
  return out;

}


#endif // QM_VECTOR_FIELD_EVALUATION_H
