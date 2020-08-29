#ifndef QM_BASE_H
#define QM_BASE_H

#include <functional>

struct Nothing{


};


constexpr bool is_not_Nothing(...){return true;}

constexpr bool is_not_Nothing(Nothing){return false;}




template<class... T> struct Cs{};

template<class T> struct Type{};

template<class T>
auto compose(T&& x)
{ return x;}

template<class T,class F, class... G>
auto compose(T&& x, F&& f, G&&...g)
{

  return compose(std::invoke(std::forward<F>(f), std::forward<T>(x)), std::forward<G>(g)...);
}



#endif // QM_BASE_H
