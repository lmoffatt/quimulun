#ifndef QM_DISTRIBUTION_ENGINE_H
#define QM_DISTRIBUTION_ENGINE_H

#include "qm_distribution_functions.h"
#include "qm_distribution.h"

#include "qm_math.h"
#include <random>
#include "qm_base.h"
#include "qm_engine.h"
#include "random"
#include "/usr/local/cuda-11.0/targets/x86_64-linux/include/curand_kernel.h"



template<>
struct Op<Normal_Distribution_f>{

   __host__ __device__  constexpr auto operator[](Normal_Distribution_f) const{ return *this;}

    template<class Mean, class Stddev>
   __host__ __device__ auto operator()(Sample, std::mt19937_64& mt,const Mean & m, const Stddev& std)const {

     return  std::normal_distribution<double>{}(mt)*std+m;
    }

   template<class sample,class Mean, class Stddev>
   __host__ __device__ auto operator()(logProbability, const sample& x,const Mean & mean, const Stddev& stddev){

      return -0.5 * std::log(2 * PI) -  std::log(stddev) -
             0.5 * sqr((x - mean) / stddev);
   }


};

template<>
struct Op<random_generator>
{
    std::mt19937_64 mt_;
    __host__ __device__   auto& operator[](random_generator) { return *this;}
    __host__ auto& operator()() { return mt_;}


};



using  my_distributions=Ops_p<Op,Cs<Normal_Distribution_f,random_generator>>;

struct distribution_engine: public my_distributions
{
   using  my_distributions::operator[];

};










#endif // QM_DISTRIBUTION_ENGINE_H
