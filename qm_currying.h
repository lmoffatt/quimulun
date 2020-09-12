#ifndef QM_CURRYING_H
#define QM_CURRYING_H


template<class T>
struct wrapper
{
    T x_;
    wrapper(T x):x_{x}{}
};


template<class F, class...Args>
class closure: public wrapper<Args>...
{
  F f_;
public:
  closure(F f, Args...args):wrapper<Args>(args)...,f_{f}{}

};





#endif // QM_CURRYING_H
