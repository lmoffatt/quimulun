#ifndef QM_FILTER_H
#define QM_FILTER_H
#include <qm_vector_field.h>
#include<qm_vector_space.h>



template<class X,class... Xs,  class value_type>
auto smooth_by_average(X, const vector_field<vec<Xs...>,value_type>& y,  std::size_t k)
{
  if constexpr(!is_in_pack<X,Xs...>())
    return  y;
    else{
  auto n=outv(y().size(X{},Position<>{})/k);
  std::vector<value_type> outv(n);
  x_i<Y,vector_field<vec<X>,value_type>> out;
  std::size_t i=0;
  std::size_t j=0;
  auto s=value_type{};
  auto f=(1.0/k);
  auto p=y().begin();
  do
  {
    s=s+y(p);
    ++j;
    if (j==k)
    {
      outv[i]=s*f;
      ++i;
       j=0;
       s=value_type{};
    }
  }while(y().next(p));
  return x_i(Y{},vector_field<vec<X>,value_type>(std::move(outv)));
  }
}


#endif // QM_FILTER_H
