#ifndef MY_MATH_H
#define MY_MATH_H

template<typename NUM,typename INT, typename=std::enable_if_t<std::is_arithmetic_v<NUM>&&std::is_integral_v<INT>,int>>
constexpr NUM mypow(NUM x, INT y)
{
  NUM out=1;
  if (y>=0)
    for (INT i=0; i<y; ++i) out*=x;
  else
    for (INT i=0; i<-y; ++i) out/=x;
  return out;
}


template<typename T> auto sqr(const T& x){ return x*x;}

inline constexpr double PI = 3.14159265358979323846;



#endif // MY_MATH_H
