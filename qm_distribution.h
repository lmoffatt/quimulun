#ifndef QM_DISTRIBUTION_H
#define QM_DISTRIBUTION_H

#include "static_string.h"
#include "qm_base.h"

struct Sample{};
struct Replicate{};
struct logProbability{};

struct random_generator{};


template<class Id,class Distribution>
class  D
{
public:
  typedef   Id myId;

  inline static constexpr auto name=eol_s+Id::name+my_static_string("~~")+Distribution::name;

  constexpr auto operator[](Id)const {return *this;}

  constexpr D(Id ,Distribution ){}
  constexpr D(){}


};

template<class... Ids> struct distributions{
    template<class ...Id2>
    friend __host__ __device__ auto operator&&(distributions,distributions<Id2...>) {return distributions<Ids...,Id2...>{};}


    template<template<class...>class arguments,class Id>
    friend __host__ __device__ constexpr auto operator- (arguments<Id>,distributions)
    {
         if constexpr ((std::is_same_v<Id,Ids>||...||false))
            return arguments<>{};
         else
            return arguments<Id>{};
    }

    template<template<class...>class arguments,class Id,class... Id2>
    friend __host__ __device__ constexpr auto operator- (arguments<Id,Id2...>,distributions)
    {
        return ((arguments<Id>{}-distributions{})&&...&&(arguments<Id2>{}-distributions{}));
    }

    template<template<class...>class arguments>
    friend __host__ __device__ constexpr auto operator- (arguments<>,distributions) {return arguments<>{};}


 };

template<class Id,class Distribution>
auto get_distribution(D<Id,Distribution>){ return distributions<Id>{};}

template<class F>
auto get_distribution(F){ return distributions<>{};}







#endif // QM_DISTRIBUTION_H
