#ifndef QM_METROPOLIS_PARALLEL_H
#define QM_METROPOLIS_PARALLEL_H

#include <qm_tensor_distribution.h>
//#include <qm_Metropolis_Distributions.h>
#include <qm_tensor_model.h>
struct data_ei{  constexpr static auto  className=my_static_string("data");};

struct beta_ei{  constexpr static auto  className=my_static_string("beta");};
struct walker_ei{  constexpr static auto  className=my_static_string("walker");};
struct logPrior_ei{   constexpr static auto  className=my_static_string("logPrior");};
struct logLik_ei{   constexpr static auto  className=my_static_string("logLik");};

struct nwalkers{constexpr static auto className=my_static_string("nwalkers");};

struct rand_e{constexpr static auto className=my_static_string("rand_e");};

struct Model_f{ constexpr static auto className=my_static_string("Model_f");};
struct Prior_f{ constexpr static auto className=my_static_string("Prior_f");};
struct simul_v{ constexpr static auto className=my_static_string("sumulation");};
struct Likelihood_f{ constexpr static auto className=my_static_string("Likelihood_f");};

struct move_distribution_f{ constexpr static auto className=my_static_string("Move_distribution_f");};

struct z_v{ constexpr static auto className=my_static_string("zeta_value");};

struct direction_v{constexpr static auto className=my_static_string("direction");};
struct Parameters_ei{  constexpr static auto  className=my_static_string("Parameters");};
struct Variables_ei{  constexpr static auto  className=my_static_string("Variables");};

struct logP_k{   constexpr static auto  className=my_static_string("logP");};
struct accept_k{  constexpr static auto  className=my_static_string("accept");};
struct logAccept_k{  constexpr static auto  className=my_static_string("logAccept");};


template<class Id> struct Current{constexpr static auto className=Id::className+my_static_string("_current");};
template<class Id> struct Candidate{constexpr static auto className=Id::className+my_static_string("_candidate");};
template<class Id> struct Next{constexpr static auto className=Id::className+my_static_string("_next");};
template<class Id> struct Chosen{constexpr static auto className=Id::className+my_static_string("_chosen");};
template<class Id> struct Pos{constexpr static auto className=Id::className+my_static_string("_position");};

template<class Id> struct Transition{constexpr static auto className=Id::className+my_static_string("_transition");};


struct mcmc_q{constexpr static auto className=my_static_string("mcmc");};

struct calculate_mcmc_f{constexpr static auto className=my_static_string("calculate_mcmc_f");};



template<class Model_q, class data_q/* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee(const Model_q& model,  const data_q& data,const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                    const v<std::size_t,dimension_less>& nw, std::mt19937::result_type seed, long int nsamples, std::ostream& f)
{
  std::mt19937_64 mt0(seed);
  auto ntot=betas.value().size()*nw;

  auto calculate_mcmc=quimulun{
      F(Variables_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {return calculate(model_fa,data_a,parameters_a);},Model_f{},data_ei{},Parameters_ei{}),
      F(logPrior_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logPrior(model_fa,data_a,parameters_a,variables_a);},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logLik_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, logPrior_ei{},logLik_ei{},beta_ei{})
  };


  auto start=quimulun{
      x_i(Start<rand_e>{},v(std::move(mt0),dimension_less{})),
      f_i(calculate_mcmc_f{},calculate_mcmc),
      f_i(Model_f{},model),
      x_i(Size<walker_ei>{},nw),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{}),
      F(index_k{},[](auto beta_a, auto walker_a){return std::pair(beta_a,walker_a);},beta_ei{},walker_ei{}),
      F(rand_e{},[](auto& mt_a,auto const &, auto const &){
            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
            return std::mt19937_64(useed(mt_a.value()));},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
          ,
      F(simul_v{},[](auto const& model_fa,auto const& data_a, auto& mt_a){return sample(model_fa,data_a,mt_a);},Model_f{},data_ei{},non_const<rand_e>{}), //,
      F(Current<Parameters_ei>{},[](auto const& model_fa,auto const& data_a, auto const& simul_a)
          {return Parameters(model_fa,data_a,simul_a);},Model_f{},data_ei{},simul_v{}),




      F(Current<mcmc_q>{},[](auto const &calculate_mcmc_fa, auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {return calculate(calculate_mcmc_fa,model_fa,data_a,parameters_a);},
          Model_f{},data_ei{},Current<Parameters_ei>{})
  };

  auto next=quimulun{
      F(all<Chosen<Parameters_ei>>{},
          [](auto parameters_a, auto &mt_a, auto nwalkers_a)
          {  std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
            auto out=parameters_a.create();
            auto p=parameters_a.begin();
            do
            {
              auto p2=p;
              p2[walker_ei{}]()= u(mt_a(p));
              out(p)=parameters_a(p2);
            } while (parameters_a.next(p));
            return out;
          },
          Current<Parameters_ei>{},rand_e{},Size<walker_ei>{}),
      F(z_v{},
          []( auto const& distribution_a, auto& mt_a)
          {
            return  distribution_a.sample(mt_a);
          },
          move_distribution_f{},rand_e{}),
      F(Candidate<Parameters_ei>{},
          [](auto const& parameter_a, auto const& parameter_b, auto const& z_a)
          {
            return (parameter_a - parameter_b) * z_a + parameter_b;
          },
          Current<Parameters_ei>{}, Chosen<Parameters_ei>{},z_v{}),

      F(Candidate<mcmc_q>{},[&calculate_mcmc](auto const &Lik_fa, auto const& prior_fa,auto const& parameters_a){

return calculate(calculate_mcmc,
                             Cs<Likelihood_f,Prior_f,Parameters_ei>{},
                             Lik_fa,prior_fa,parameters_a);},
          Likelihood_f{},Prior_f{},Candidate<Parameters_ei>{})


  };

  auto Metropolis_hastings_test=quimulun
      {
          F(logAccept_k{},[](auto const& current_mcmc_a,auto const& next_mcmc_a, auto z_a, auto size_Parameters){
                return  next_mcmc_a[logP_k{}].value() - current_mcmc_a[logP_k{}].value()  +
                       log(z_a) * (size_Parameters - 1);

              }, Current<mcmc_q>{}, Candidate<mcmc_q>{},z_v{},z_v{},Size<Parameters_ei>{}),
          F(accept_k{},[](auto const& logA_a, auto& mt_a){
                double A = std::exp(logA_a.value());
                double r = std::uniform_real_distribution<double>(0, 1)(mt_a);
                return  r < A;

              }, logAccept_k{}, rand_e{})


      };

  auto actualize=quimulun {
      F(Cs<Next<Parameters_ei>, Next<mcmc_q>>{},[](auto& current_par_a, auto& current_mcmc_a,auto& candidate_par_a,  auto& candidate_mcmc_a , auto const& accept_k){
            if(accept_k)
              return std::make_tuple(std::move(candidate_par_a),std::move(candidate_mcmc_a));
            else
              return std::make_tuple(std::move(current_par_a),std::move(current_mcmc_a));
          }, Current<Parameters_ei>{}, Current<mcmc_q>{}, Candidate<Parameters_ei>{}, Candidate<Parameters_ei>{},accept_k{})


  };


  auto temperature_jump=quimulun{
      F(all<Transition<beta_ei>>{},[](auto const & beta_a,auto& mt_a, auto num_walkers_a){

            using elem_type=decltype(vector_space(Cs<Start<Position<beta_ei>>,End<Position<beta_ei>>,Start<walker_ei>,End<walker_ei>>{},
                                                    beta_a.begin(),beta_a.begin(),num_walkers_a,num_walkers_a));
            using out_type=vector_field<vec<Transition<beta_ei>>,
                                          elem_type>;
            out_type out;
            auto pos=out.begin();
            auto p=beta_a.begin();

            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a(p))) beta_a.next(p);
            auto p2=p;

            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a - 1);


            while(beta_a.next(p2))

            {
              auto start_w=u(mt_a(p));
              auto end_w=u(mt_a(p2));

              out.insert_at(pos,elem_type(p,p2,start_w,end_w));
              ++pos[Transition<beta_ei>{}]();
              beta_a.next(p2);
              p=p2;
            }
            return out;
          },
          beta_ei{},rand_e{},Size<walker_ei>{}),
      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]();
            return Next_mcmc_a(p)[logLik_ei{}];

          },Transition<beta_ei>{},Next<mcmc_q>{}),
      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]();
            return Next_mcmc_a(p)[logLik_ei{}];

          },Transition<beta_ei>{},all<Next<mcmc_q>>{}),
      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
          {
            auto p=beta_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]()[beta_ei{}]();
            auto start_beta_a=beta_a(p);
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]()[beta_ei{}]();
            auto end_beta_a=beta_a(p);

            return  -(start_beta_a - end_beta_a) *
                   (start_logLik_a -end_logLik_a);
          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
      F(accept_k{},[](auto const& logA_a, auto& mt_a){
            double A = std::exp(logA_a.value());
            double r = std::uniform_real_distribution<double>(0, 1)(mt_a);
            return  r < A;

          }, logAccept_k{}, Start<rand_e>{})




  };


  auto actualize_jump=quimulun{
      F(all<Cs<Next<Parameters_ei>, Next<mcmc_q>>>{},[](auto& next_par_a, auto& next_mcmc_a,auto const& trans_beta_a,  auto const& accept_k){
            auto p=trans_beta_a.begin();
            auto pos=next_par_a.begin();
            do
            {
              if (accept_k(p))
              {
                auto start_pos=pos;
                auto end_pos=pos;
                start_pos[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]()[beta_ei{}]();
                start_pos[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]();
                end_pos[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]()[beta_ei{}]();
                end_pos[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]();

                auto par_t=std::move(next_par_a(start_pos));
                next_par_a(start_pos)=std::move(next_par_a(end_pos));
                next_par_a(end_pos)=std::move(par_t);

                auto mcmc_t=std::move(next_mcmc_a(start_pos));
                next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
                next_mcmc_a(end_pos)=std::move(mcmc_t);


              }

            }while(trans_beta_a.next(p));

            return std::make_tuple(std::move(next_par_a), std::move(next_mcmc_a));


          }, Next<Parameters_ei>{}, Next<mcmc_q>{}, Transition<beta_ei>{}, accept_k{})


  };




 auto mcmc=calculate(start,vector_space<>{});

  for (long int i=0; i<nsamples; ++i)
  {


  }

  return mcmc;








}








#endif // QM_METROPOLIS_PARALLEL_H
