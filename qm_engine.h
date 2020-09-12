#ifndef QM_ENGINE_H
#define QM_ENGINE_H
#include "qm_base.h"


struct cpu{};
struct gpu{};


template< class pu>
class engine
{

};


template<template <class...> class, class>
struct Ops_p;

template<template <class...> class fn, class... functions>
struct Ops_p<fn,Cs<functions...>> : public fn<functions>...
{
    using fn<functions>::operator[]...;
};






#endif // QM_ENGINE_H
