#ifndef QM_METROPOLIS_DISTRIBUTIONS_H
#define QM_METROPOLIS_DISTRIBUTIONS_H


#include <qm_tensor_distribution.h>

    struct Distribution_k{   constexpr static auto  className=my_static_string("Distribution");};


struct res_mcmc_k{  constexpr static auto  className=my_static_string("result_mcmc");};




using res_mcmc_t=vector_space<x_i<accept_k,v<bool,p<>>>,x_i<logAccept_k,v<double,dimension_less>>>;



template < class Parameter_t,class logP_t>
using  mcmc_t= vector_space<x_i<Parameters_ei,Parameter_t>,x_i<logP_k,logP_t>>;

template < class Parameter_t,class logP_t, class res_mcmc_type=res_mcmc_t>
using  rmcmc_t= vector_space<x_i<Parameters_ei,Parameter_t>,x_i<logP_k,logP_t>,x_i<res_mcmc_k,res_mcmc_t>>;



template < class Parameter_t,class logP_t>
using  emcmc_t=vector_space<x_i<Parameters_ei,vector_field<vec<walker_k>,Parameter_t>>,
                             x_i<logP_k,vector_field<vec<walker_k>,logP_t>>>;






template<class logLik_calc,class move_distribution=stretch_move_Distribution>
class Ensemble_Based_Distribution
{
private:
  move_distribution d_;
  logLik_calc logL_;
public:
  template < class Parameter_t,class logP_t,class Random>
 void  Metropolis_hastings_test(double Z,const emcmc_t<Parameter_t,logP_t>& current, rmcmc_t<Parameter_t,logP_t>& candidate, Random& mt)
  {
    double logA = candidate[logP_k{}].value() - current[logP_k{}].value()  +
                  log(Z) * (size(current[Parameters_ei{}].value()) - 1);

    double A = std::exp(logA);
    double r = std::uniform_real_distribution<double>(0, 1)(mt);
    bool accept = r < A;
    res_mcmc_t res;
    res[accept_k{}]().value()=accept;
    res[logAccept_k{}]().value()=logA;
    candidate[res_mcmc_k{}]().value()=res;
  }


  template < class Parameter_t,class logP_t, class Random>
  auto move(const emcmc_t<Parameter_t,logP_t>& current, Random& mt)
{
  auto n=size(current);
  std::uniform_int_distribution<std::size_t> u(1, size(current));

 rmcmc_t<Parameter_t,logP_t> candidate(n);

 for (std::size_t i=0; i<n; ++i)  {
  double z = d_.sample(mt[i]);
Position<walker_k> j = index[u(mt[i]) - 1];
Position<walker_k> pos=i;
candidate(pos)[Parameters_ei{}]=(current(pos)[Parameters_ei{}] - current(j)[Parameters_ei{}]) * z + current(j)[Parameters_ei{}];
candidate(pos)[logP_k{}]=logL_(candidate(pos)[Parameters_ei{}]);
Metropolis_hastings_test(z,current(pos),candidate(pos),mt[i]);
}

return candidate;

  }
  Ensemble_Based_Distribution(const logLik_calc& logL_v, const move_distribution& d_v):logL_{logL_v},d_{d_v}{}

  Ensemble_Based_Distribution()=default;

};



class Ensemble_Parallel{

};



class FIM_Based_Distribution;


#endif // QM_METROPOLIS_DISTRIBUTIONS_H
