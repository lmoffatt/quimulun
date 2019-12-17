#ifndef QM_METROPOLIS_PARALLEL_H
#define QM_METROPOLIS_PARALLEL_H

#include <qm_tensor_distribution.h>
//#include <qm_Metropolis_Distributions.h>
#include <qm_tensor_model.h>
#include <qm_data_frame.h>
#include <fstream>
struct data_ei{  constexpr static auto  className=my_static_string("data");};
struct beta_ei{  constexpr static auto  className=my_static_string("beta");};
struct walker_ei{  constexpr static auto  className=my_static_string("walker");};
struct logPrior_ei{   constexpr static auto  className=my_static_string("logPrior");};
struct logLik_ei{   constexpr static auto  className=my_static_string("logLik");};

struct isample{constexpr static auto className=my_static_string("i_sample");};

struct rand_e{constexpr static auto className=my_static_string("rand_e");};

struct Model_f{ constexpr static auto className=my_static_string("Model_f");};

struct Evidence_ei{ constexpr static auto className=my_static_string("Evidence");};


struct z_v{ constexpr static auto className=my_static_string("zeta_value");};

struct Parameters_ei{  constexpr static auto  className=my_static_string("Parameters");};
struct Variables_ei{  constexpr static auto  className=my_static_string("Variables");};

struct logP_k{   constexpr static auto  className=my_static_string("logP");};
struct accept_k{  constexpr static auto  className=my_static_string("accept");};
struct logAccept_k{  constexpr static auto  className=my_static_string("logAccept");};


template<class Id> struct Current{constexpr static auto className=Id::className+my_static_string("_current");};
template<class Id> struct Candidate{constexpr static auto className=Id::className+my_static_string("_candidate");};
template<class Id> struct Next{constexpr static auto className=Id::className+my_static_string("_next");};
template<class Id> struct Chosen{constexpr static auto className=Id::className+my_static_string("_chosen");};
template<class... Id> struct Pos{constexpr static auto className=(Id::className+...+my_static_string("_position"));};
template<class Id> struct Average{constexpr static auto className=Id::className+my_static_string("_average");};
//template<class Id> struct Stddev{constexpr static auto className=Id::className+my_static_string("_stddev");};

struct Current_mcmc{constexpr static auto className=my_static_string("current_mcmc");};
struct global_Current_mcmc{constexpr static auto className=my_static_string("global_current_mcmc");};
struct Candidate_mcmc{constexpr static auto className=my_static_string("candidate_mcmc");};
struct Next_mcmc{constexpr static auto className=my_static_string("next_mcmc");};
struct Step_mcmc{constexpr static auto className=my_static_string("step");};

template<class Id> struct Transition{constexpr static auto className=Id::className+my_static_string("_transition");};
struct stretch_alfa{constexpr static auto className=my_static_string("stretch_alfa");};




struct calculate_initial_mcmc_f{constexpr static auto className=my_static_string("calculate_initial_mcmc_f");};
struct calculate_next_mcmc_f{constexpr static auto className=my_static_string("calculate_next_mcmc_f");};
struct calculate_step_f{constexpr static auto className=my_static_string("calculate_step_f");};



template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_series(const Model_q& model,  const data_q& data,const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                    const v<std::size_t,dimension_less>& nw, std::mt19937::result_type seed, long int nsamples, const std::vector<std::size_t>& decimate_factor,const std::string& filename)
{
  std::mt19937_64 mt0(seed);

  auto fname=filename+"_"+ time_now() + "_" + std::to_string(seed);

  auto rand0=vector_space{x_i(Start<rand_e>{},v(std::move(mt0),dimension_less{}))};

  auto calculate_mcmc=quimulun{
      F(Candidate<Variables_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {return calculate(model_fa,data_a,parameters_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}),
      F(Candidate<logPrior_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logPrior(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}, Candidate<Variables_ei>{}),
      F(Candidate<logLik_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}, Candidate<Variables_ei>{}),
      F(Candidate<logP_k>{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, Candidate<logPrior_ei>{},Candidate<logLik_ei>{},beta_ei{})
  };

  auto modeldata=quimulun{
      f_i(Model_f{},model),
      x_i(stretch_alfa{},v(2.0,dimension_less{})),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      x_i(Size<walker_ei>{},nw)
  };
  
  
  
  
  
  
  auto initwalkers_f=quimulun{
      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{})
  };
  
  auto initwalkers_v=calculate(initwalkers_f,modeldata);
  
  auto random_f=quimulun{
      Fr(rand_e{},[](auto& mt_a,auto const &, auto const &){
            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
            return v(std::mt19937_64(useed(mt_a.value())),dimension_less{});},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
  };
  
  auto random_v=random_calculate(random_f,rand0,modeldata,initwalkers_v);
  

  auto initParameters_q=quimulun{
                              Fr(Candidate<Parameters_ei>{},[](auto& mt_a,auto const& model_fa,auto const& data_a)
                                  {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
                          }+calculate_mcmc;

  auto candidate_to_current=quimulun{
      F(Current<Parameters_ei>{},[](auto&& x){return x;},Candidate<Parameters_ei>{}),
      F(Current<Variables_ei>{},[](auto&& x){return x;},Candidate<Variables_ei>{}),
      F(Current<logPrior_ei>{},[](auto&& x){return x;},Candidate<logPrior_ei>{}),
      F(Current<logLik_ei>{},[](auto&& x){return x;},Candidate<logLik_ei>{}),
      F(Current<logP_k>{},[](auto&& x){return x;},Candidate<logP_k>{})
  };
  
  auto next=quimulun{
                  Fr(all<Chosen<Parameters_ei>>{},
                      [](auto &mt_a, auto const& parameters_a, auto const& nwalkers_a)
                      {  std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                        auto out=parameters_a.create();
                        auto p=parameters_a.begin();
                        do
                        {
                          auto p2=p;
                          p2[walker_ei{}]()= u(mt_a(p).value());
                          out(p)=parameters_a(p2);
                        } while (parameters_a.next(p));
                        return out;
                      },
                      non_const<rand_e>{},Current<Parameters_ei>{},Size<walker_ei>{}),
                  Fr(z_v{},
                      []( auto& mt_a, auto const& alfa)
                      {
                        return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                      }
                      ,non_const<rand_e>{},stretch_alfa{}),
                  F(Candidate<Parameters_ei>{},
                      [](auto const& parameter_a, auto const& parameter_b, auto const& z_a)
                      {
                        return (parameter_a - parameter_b) * z_a + parameter_b;
                      },
                      Current<Parameters_ei>{}, Chosen<Parameters_ei>{},z_v{})
              }+calculate_mcmc;

  auto Metropolis_hastings_test=quimulun
      {
          F(logAccept_k{},[](auto const& current_logP_a,auto const& candidate_loP_a, auto z_a, auto Parameters_a){

                // TODO: implement logpr as a value so difference between logp are properly assesed
                return  (candidate_loP_a-current_logP_a )  +
                       log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

              }, Current<logP_k>{}, Candidate<logP_k>{},z_v{},Current<Parameters_ei>{}),
          Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
                double A = std::exp(logA_a.value());
                double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
                return  int(r < A);

              }, non_const<rand_e>{}, logAccept_k{})
      };



  auto next_2=quimulun{
                    Fr(pos<Chosen<Parameters_ei>>{},
                        [](auto &mt_a, auto const& p,auto const& parameters_a, auto const& nwalkers_a)
                        {
                          std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                          auto p2=p;
                          p2[walker_ei{}]()= u(mt_a(p).value());
                          auto out=parameters_a(p2);
                          return out;
                        },
                        non_const<rand_e>{},all<Current<Parameters_ei>>{},Size<walker_ei>{}),
                    Fr(z_v{},
                        []( auto& mt_a, auto const& alfa)
                        {
                          return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                        }
                        ,non_const<rand_e>{},stretch_alfa{}),
                    F(Candidate<Parameters_ei>{},
                        [](auto const& parameter_a, auto const& parameter_b, auto const& z_a)
                        {
                          return (parameter_a - parameter_b) * z_a + parameter_b;
                        },
                        Current<Parameters_ei>{}, Chosen<Parameters_ei>{},z_v{})
                }+calculate_mcmc;


  auto accept_candidate_or_current=quimulun{
      F(Next<Parameters_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<Parameters_ei>{}, Candidate<Parameters_ei>{}),
      F(Next<Variables_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<Variables_ei>{}, Candidate<Variables_ei>{}),
      F(Next<logPrior_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logPrior_ei>{}, Candidate<logPrior_ei>{}),
      F(Next<logLik_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logLik_ei>{}, Candidate<logLik_ei>{}),
      F(Next<logP_k>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logP_k>{}, Candidate<logP_k>{})
  };

  auto temperature_jump=quimulun{
      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
            using pos_type=std::decay_t<decltype (beta_a.begin())>;
            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<Start<walker_ei>,nwalkers_type>,
                                           x_i<End<walker_ei>,nwalkers_type>>;
            
            
            using out_type=vector_field<vec<Transition<beta_ei>>,
                                          elem_type>;
            out_type out;
            auto pos=out.begin();
            auto p=beta_a.begin();
            
            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
              beta_a.next(p);
            auto p2=p;
            
            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);
            
            
            while(beta_a.next(p2))
            
            {
              auto start_w=v(u(mt_a(p).value()),dimension_less{});
              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});
              
              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
              ++pos[Transition<beta_ei>{}]();
              if (!beta_a.next(p2)) break;
              p=p2;
            }
            return out;
          },
          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_logLik_a)
          {
            auto p=Next_logLik_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
            return Next_logLik_a(p);
            
          },Transition<beta_ei>{},all<Next<logLik_ei>>{}),
      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_logLik_a)
          {
            auto p=Next_logLik_a.begin();
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
            return Next_logLik_a(p);
            
          },Transition<beta_ei>{},all<Next<logLik_ei>>{}),
      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
          {
            auto p=beta_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto start_beta_a=beta_a(p);
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto end_beta_a=beta_a(p);
            
            return  -(start_beta_a - end_beta_a) *
                   (start_logLik_a -end_logLik_a);
          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
            double A = std::exp(logA_a.value());
            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
                        return  v<int,dimension_less>(int(r < A));

          }, non_const<Start<rand_e>>{}, logAccept_k{})
      
      
      

  };

   auto actualize_jump=quimulun{
      F{all<Current<Parameters_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if ( accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();
              
              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }
            
          }while(trans_beta_a.next(p));
          
          return std::move(next_elem_a);
          
          
        }, Next<Parameters_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<Variables_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();
              
              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }
            
          }while(trans_beta_a.next(p));
          
          return std::move(next_elem_a);
          
          
        }, Next<Variables_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<logPrior_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();
              
              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }
            
          }while(trans_beta_a.next(p));
          
          return std::move(next_elem_a);
          
          
        }, Next<logPrior_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<logLik_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();
              
              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }
            
          }while(trans_beta_a.next(p));
          
          return std::move(next_elem_a);
          
          
        }, Next<logLik_ei>{}, Transition<beta_ei>{}, accept_k{}}
      ,
      F(all<Current<logP_k>>{},[](auto&& next_logP_a, auto const& logLik_a, auto const& logPrior_a,  auto const& beta_a,auto const& trans_beta_a,  auto const& accept_k){
            auto p=trans_beta_a.begin();
            auto pos=next_logP_a.begin();
            do
            {
              if (accept_k(p).value())
              {
                auto start_pos=pos;
                auto end_pos=pos;
                start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
                start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
                end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
                end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();
                
                next_logP_a(start_pos)=logPrior_a(start_pos)+logLik_a(start_pos)*beta_a(start_pos);
                next_logP_a(end_pos)=logPrior_a(end_pos)+logLik_a(end_pos)*beta_a(end_pos);
                
              }
              
            }while(trans_beta_a.next(p));
            
            return std::move(next_logP_a);
            
            
          }, Next<logP_k>{}, Current<logLik_ei>{}, Current<logPrior_ei>{},beta_ei{},Transition<beta_ei>{}, accept_k{})
  };
  
  auto intial_Parameters_v=random_calculate(initParameters_q,random_v,modeldata,initwalkers_v);

  auto currentParameters_v=transfer(candidate_to_current, std::move(intial_Parameters_v));
  
  auto compute_Evidence=quimulun{
      F(all<Average<Current<logLik_ei>>>{},[](auto const& logLik,auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            auto out=beta.template create<value_type>();
            auto pos=out.begin();
            auto n=beta.create();
            auto p=logLik.begin();
            do{
              out(p)=out(p)+logLik(p);
              n(p)=n(p)+v(1.0);
            }while(logLik.next(p));
            
            do{
              out(pos)=out(pos)*(v(1)/n(pos));
            }while(out.next(pos));
            
            return out;
            
            
          },Current<logLik_ei>{}, beta_ei{}),
      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            value_type out;
            auto p=beta.begin();
            auto beta0=beta(p);
            auto db_sum=beta0-beta0;
            if (beta0.value()<0.5)
            {
              beta0.value()=0;
              auto beta1=beta(p);
              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
              auto db=(beta1-beta0)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              db_sum=db_sum+db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta2-beta0)/v(2.0);
                out=out+logL0*db;
                db_sum=db_sum+db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
              out=out+logL0*db;
              db_sum=db_sum+db;
              return out;
              
            }
            else
            {
              beta0.value()=1.0;
              auto beta1=beta(p);
              auto db=(beta0-beta1)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta0-beta2)/v(2.0);
                out=out+logL0*db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2-beta1)*v(1.0/2.0);
              out=out+logL0*db;
              return out;
            }
            
          }, Average<Current<logLik_ei>>{},beta_ei{} )
      
  };
  
  std::string f_jump_name=filename+"_jump.txt";

  std::ofstream f_jump(f_jump_name.c_str());





  for (long int i=0; i<nsamples; ++i)
  {
    auto next_candidate_v=random_calculate(next_2,random_v,currentParameters_v,modeldata);

    auto Met_test_v=random_calculate(Metropolis_hastings_test,random_v,currentParameters_v,next_candidate_v,modeldata);
    
    
    auto next_parameter_v=transfer(accept_candidate_or_current,Met_test_v,std::move(currentParameters_v), std::move(next_candidate_v));

    auto after_temperature_jump=random_calculate(temperature_jump,rand0,next_parameter_v,modeldata);


    auto output_jump=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump|myremove<accept_k>{});

    if (i==0)
      to_DataFrame_title(f_jump,output_jump);
    if (i%10==0)
      to_DataFrame_body(f_jump,output_jump);

    currentParameters_v=transfer(actualize_jump,std::move(next_parameter_v),after_temperature_jump,modeldata);

    auto Evidence_v=calculate(compute_Evidence,currentParameters_v,modeldata);


    auto output=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+currentParameters_v+data+Evidence_v;
     std::cerr<<"i_sample="<<i<<"\t"<<(Evidence_v|myselect<Current<Evidence_ei>>{})<<"\n";
    
    if (i==0)
    {
   //   to_DataFrame_title_index(fname,output);
      to_DataFrame_title_index_new(fname,currentParameters_v,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname,Evidence_v,data,modeldata,initwalkers_v);

    }
   // to_DataFrame_body_index(fname,i,decimate_factor,output);
    to_DataFrame_body_index_new(fname,i,decimate_factor,currentParameters_v,data,modeldata,initwalkers_v);
    to_DataFrame_body_index_new(fname,i,decimate_factor,Evidence_v,data,modeldata,initwalkers_v);

    
  }
  
  return 0;
  
}

template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_series_parallel_for(const Model_q& model,  const data_q& data,const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                           const v<std::size_t,dimension_less>& nw, std::mt19937::result_type seed, long int nsamples, const std::vector<std::size_t>& decimate_factor,const std::string& filename)
{
  std::mt19937_64 mt0(seed);

  auto fname=filename+"_"+ time_now() + "_" + std::to_string(seed);

  auto rand0=vector_space{x_i(Start<rand_e>{},v(std::move(mt0),dimension_less{}))};

  auto calculate_mcmc=quimulun{
      F(Candidate<Variables_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {return calculate(model_fa,data_a,parameters_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}),
      F(Candidate<logPrior_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logPrior(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}, Candidate<Variables_ei>{}),
      F(Candidate<logLik_ei>{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Candidate<Parameters_ei>{}, Candidate<Variables_ei>{}),
      F(Candidate<logP_k>{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, Candidate<logPrior_ei>{},Candidate<logLik_ei>{},beta_ei{})
  };

  auto modeldata=quimulun{
      f_i(Model_f{},model),
      x_i(stretch_alfa{},v(2.0,dimension_less{})),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      x_i(Size<walker_ei>{},nw)
  };






  auto initwalkers_f=quimulun{
      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{})
  };

  auto initwalkers_v=calculate(initwalkers_f,modeldata);

  auto random_f=quimulun{
      Fr(rand_e{},[](auto& mt_a,auto const &, auto const &){
            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
            return v(std::mt19937_64(useed(mt_a.value())),dimension_less{});},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
  };

  auto random_v=random_calculate(random_f,rand0,modeldata,initwalkers_v);


  auto initParameters_q=quimulun{
                              Fr(Candidate<Parameters_ei>{},[](auto& mt_a,auto const& model_fa,auto const& data_a)
                                  {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
                          }+calculate_mcmc;

  auto candidate_to_current=quimulun{
      F(Current<Parameters_ei>{},[](auto&& x){return x;},Candidate<Parameters_ei>{}),
      F(Current<Variables_ei>{},[](auto&& x){return x;},Candidate<Variables_ei>{}),
      F(Current<logPrior_ei>{},[](auto&& x){return x;},Candidate<logPrior_ei>{}),
      F(Current<logLik_ei>{},[](auto&& x){return x;},Candidate<logLik_ei>{}),
      F(Current<logP_k>{},[](auto&& x){return x;},Candidate<logP_k>{})
  };

  auto next=quimulun{
                  Fr(all<Chosen<Parameters_ei>>{},
                      [](auto &mt_a, auto const& parameters_a, auto const& nwalkers_a)
                      {  std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                        auto out=parameters_a.create();
                        auto p=parameters_a.begin();
                        do
                        {
                          auto p2=p;
                          p2[walker_ei{}]()= u(mt_a(p).value());
                          out(p)=parameters_a(p2);
                        } while (parameters_a.next(p));
                        return out;
                      },
                      non_const<rand_e>{},Current<Parameters_ei>{},Size<walker_ei>{}),
                  Fr(z_v{},
                      []( auto& mt_a, auto const& alfa)
                      {
                        return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                      }
                      ,non_const<rand_e>{},stretch_alfa{}),
                  F(Candidate<Parameters_ei>{},
                      [](auto const& parameter_a, auto const& parameter_b, auto const& z_a)
                      {
                        return (parameter_a - parameter_b) * z_a + parameter_b;
                      },
                      Current<Parameters_ei>{}, Chosen<Parameters_ei>{},z_v{})
              }+calculate_mcmc;

  auto Metropolis_hastings_test=quimulun
      {
          F(logAccept_k{},[](auto const& current_logP_a,auto const& candidate_loP_a, auto z_a, auto Parameters_a){

                // TODO: implement logpr as a value so difference between logp are properly assesed
                return  (candidate_loP_a-current_logP_a )  +
                       log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

              }, Current<logP_k>{}, Candidate<logP_k>{},z_v{},Current<Parameters_ei>{}),
          Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
                double A = std::exp(logA_a.value());
                double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
                return  int(r < A);

              }, non_const<rand_e>{}, logAccept_k{})
      };



  auto next_2=quimulun{
                    Fr(pos<Chosen<Parameters_ei>>{},
                        [](auto &mt_a, auto const& p,auto const& parameters_a, auto const& nwalkers_a)
                        {
                          std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                          auto p2=p;
                          p2[walker_ei{}]()= u(mt_a(p).value());
                          auto out=parameters_a(p2);
                          return out;
                        },
                        non_const<rand_e>{},all<Current<Parameters_ei>>{},Size<walker_ei>{}),
                    Fr(z_v{},
                        []( auto& mt_a, auto const& alfa)
                        {
                          return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                        }
                        ,non_const<rand_e>{},stretch_alfa{}),
                    F(Candidate<Parameters_ei>{},
                        [](auto const& parameter_a, auto const& parameter_b, auto const& z_a)
                        {
                          return (parameter_a - parameter_b) * z_a + parameter_b;
                        },
                        Current<Parameters_ei>{}, Chosen<Parameters_ei>{},z_v{})
                }+calculate_mcmc;


  auto accept_candidate_or_current=quimulun{
      F(Next<Parameters_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<Parameters_ei>{}, Candidate<Parameters_ei>{}),
      F(Next<Variables_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<Variables_ei>{}, Candidate<Variables_ei>{}),
      F(Next<logPrior_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logPrior_ei>{}, Candidate<logPrior_ei>{}),
      F(Next<logLik_ei>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logLik_ei>{}, Candidate<logLik_ei>{}),
      F(Next<logP_k>{},
          [](auto const& accept_a, auto&& current, auto&& candidate){ if (accept_a) return std::move(candidate); else return std::move(current);}
          ,accept_k{}, Current<logP_k>{}, Candidate<logP_k>{})
  };

  auto temperature_jump=quimulun{
      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
            using pos_type=std::decay_t<decltype (beta_a.begin())>;
            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<Start<walker_ei>,nwalkers_type>,
                                           x_i<End<walker_ei>,nwalkers_type>>;


            using out_type=vector_field<vec<Transition<beta_ei>>,
                                          elem_type>;
            out_type out;
            auto pos=out.begin();
            auto p=beta_a.begin();

            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
              beta_a.next(p);
            auto p2=p;

            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


            while(beta_a.next(p2))

            {
              auto start_w=v(u(mt_a(p).value()),dimension_less{});
              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
              ++pos[Transition<beta_ei>{}]();
              if (!beta_a.next(p2)) break;
              p=p2;
            }
            return out;
          },
          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_logLik_a)
          {
            auto p=Next_logLik_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
            return Next_logLik_a(p);

          },Transition<beta_ei>{},all<Next<logLik_ei>>{}),
      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_logLik_a)
          {
            auto p=Next_logLik_a.begin();
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
            return Next_logLik_a(p);

          },Transition<beta_ei>{},all<Next<logLik_ei>>{}),
      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
          {
            auto p=beta_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto start_beta_a=beta_a(p);
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto end_beta_a=beta_a(p);

            return  -(start_beta_a - end_beta_a) *
                   (start_logLik_a -end_logLik_a);
          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
            double A = std::exp(logA_a.value());
            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
            return  v<int,dimension_less>(int(r < A));

          }, non_const<Start<rand_e>>{}, logAccept_k{})




  };

  auto actualize_jump=quimulun{
      F{all<Current<Parameters_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if ( accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_elem_a);


        }, Next<Parameters_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<Variables_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_elem_a);


        }, Next<Variables_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<logPrior_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_elem_a);


        }, Next<logPrior_ei>{}, Transition<beta_ei>{}, accept_k{}}
      , F{all<Current<logLik_ei>>{},[](auto&& next_elem_a, auto const& trans_beta_a,  auto const& accept_k){
          auto p=trans_beta_a.begin();
          auto pos=next_elem_a.begin();
          do
          {
            if (accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_elem_a(start_pos));
              next_elem_a(start_pos)=std::move(next_elem_a(end_pos));
              next_elem_a(end_pos)=std::move(par_t);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_elem_a);


        }, Next<logLik_ei>{}, Transition<beta_ei>{}, accept_k{}}
      ,
      F(all<Current<logP_k>>{},[](auto&& next_logP_a, auto const& logLik_a, auto const& logPrior_a,  auto const& beta_a,auto const& trans_beta_a,  auto const& accept_k){
            auto p=trans_beta_a.begin();
            auto pos=next_logP_a.begin();
            do
            {
              if (accept_k(p).value())
              {
                auto start_pos=pos;
                auto end_pos=pos;
                start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
                start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
                end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
                end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

                next_logP_a(start_pos)=logPrior_a(start_pos)+logLik_a(start_pos)*beta_a(start_pos);
                next_logP_a(end_pos)=logPrior_a(end_pos)+logLik_a(end_pos)*beta_a(end_pos);

              }

            }while(trans_beta_a.next(p));

            return std::move(next_logP_a);


          }, Next<logP_k>{}, Current<logLik_ei>{}, Current<logPrior_ei>{},beta_ei{},Transition<beta_ei>{}, accept_k{})
  };

  auto intial_Parameters_v=random_calculate(initParameters_q,random_v,modeldata,initwalkers_v);

  auto currentParameters_v=transfer(candidate_to_current, std::move(intial_Parameters_v));

  auto compute_Evidence=quimulun{
      F(all<Average<Current<logLik_ei>>>{},[](auto const& logLik,auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            auto out=beta.template create<value_type>();
            auto pos=out.begin();
            auto n=beta.create();
            auto p=logLik.begin();
            do{
              out(p)=out(p)+logLik(p);
              n(p)=n(p)+v(1.0);
            }while(logLik.next(p));

            do{
              out(pos)=out(pos)*(v(1)/n(pos));
            }while(out.next(pos));

            return out;


          },Current<logLik_ei>{}, beta_ei{}),
      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            value_type out;
            auto p=beta.begin();
            auto beta0=beta(p);
            auto db_sum=beta0-beta0;
            if (beta0.value()<0.5)
            {
              beta0.value()=0;
              auto beta1=beta(p);
              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
              auto db=(beta1-beta0)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              db_sum=db_sum+db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta2-beta0)/v(2.0);
                out=out+logL0*db;
                db_sum=db_sum+db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
              out=out+logL0*db;
              db_sum=db_sum+db;
              return out;

            }
            else
            {
              beta0.value()=1.0;
              auto beta1=beta(p);
              auto db=(beta0-beta1)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta0-beta2)/v(2.0);
                out=out+logL0*db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2-beta1)*v(1.0/2.0);
              out=out+logL0*db;
              return out;
            }

          }, Average<Current<logLik_ei>>{},beta_ei{} )

  };

  std::string f_jump_name=filename+"_jump.txt";

  std::ofstream f_jump(f_jump_name.c_str());





  for (long int i=0; i<nsamples; ++i)
  {
    auto nsample=vector_space{x_i{isample{},v(i,dimension_less{})}};

    auto next_candidate_v=random_calculate_parallel_for(next_2,random_v,currentParameters_v,modeldata);

    auto Met_test_v=random_calculate_parallel_for(Metropolis_hastings_test,random_v,currentParameters_v,next_candidate_v,modeldata);


    auto next_parameter_v=transfer(accept_candidate_or_current,Met_test_v,std::move(currentParameters_v), std::move(next_candidate_v));

    auto after_temperature_jump=random_calculate_parallel_for(temperature_jump,rand0,next_parameter_v,modeldata);


    auto output_jump=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump|myremove<accept_k>{});

    if (i==0)
      to_DataFrame_title(f_jump,output_jump);
    if (i%10==0)
      to_DataFrame_body(f_jump,output_jump);

    currentParameters_v=transfer(actualize_jump,std::move(next_parameter_v),after_temperature_jump,modeldata);

    auto Evidence_v=calculate(compute_Evidence,currentParameters_v,modeldata);


    auto output=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+currentParameters_v+data+Evidence_v;
    std::cerr<<"i_sample="<<i<<"\t"<<(Evidence_v|myselect<Current<Evidence_ei>>{})<<"\n";

    if (i==0)
    {
      to_DataFrame_title_index(fname,output);
    }
    to_DataFrame_body_index(fname,i,decimate_factor,output);


  }

  return 0;

}




template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_parallel(const Model_q& model,  const data_q& data,const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                    const v<std::size_t,dimension_less>& nw, std::mt19937::result_type seed, long int nsamples, const std::vector<std::size_t>& decimate_factor,const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);

  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};


  auto calculate_mcmc_parallel=quimulun{
      F(Variables_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {
            return calculate(model_fa,data_a,parameters_a);
          },
          Model_f{},data_ei{},Parameters_ei{}),
      F(logPrior_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logPrior(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logLik_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, logPrior_ei{},logLik_ei{},beta_ei{})
  };

  auto modeldata=quimulun{
      f_i(Model_f{},model),
      x_i(stretch_alfa{},v(2.0,dimension_less{})),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      x_i(Size<walker_ei>{},nw)
  };


  auto initwalkers_f=quimulun{
      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{})
  };

  auto initwalkers_v=calculate(initwalkers_f,modeldata);

  auto random_f=quimulun{
      Fr(rand_e{},[](auto& mt_a,auto const &, auto const &){
            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
            return v(std::mt19937_64(useed(mt_a.value())),dimension_less{});},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
  };

  auto random_parallel_v=random_calculate(random_f,rand0_parallel,modeldata,initwalkers_v);



  auto initParameters_q_parallel=vector_space(f_i(calculate_initial_mcmc_f{},
                                                  quimulun{
                                                      Fr(Parameters_ei{},[](auto& mt_a,auto const& model_fa,auto const& data_a)
                                                          {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
                                                  }+calculate_mcmc_parallel));


  auto inside_init_Parameters_parallel=quimulun{
      Fr(Current_mcmc{},[](auto& mt_a, auto const& initial_mcmc_fa,auto const& model_fa,auto const& data_a, auto const& beta_a)
          {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
                           //     ,x_i(data_ei{},data_a));
                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
            return random_calculate(initial_mcmc_fa, mt,var);},
          non_const<rand_e>{},calculate_initial_mcmc_f{},Model_f{},data_ei{},beta_ei{})
  };

  auto next_inside=
      vector_space(f_i{calculate_next_mcmc_f{},
                       quimulun{
                           Fr(Parameters_ei{},
                               [](auto &mt_a, auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& nwalkers_a, auto const& z_a)
                               {
                                 auto const& parameter_a=mcmc_a[Parameters_ei{}]();
                                 std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                                 auto p2=pos_a.value();
                                 p2[walker_ei{}]()= u(mt_a.value());
                                 auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
                                 return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
                               },
                               non_const<rand_e>{},Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Size<walker_ei>{}, z_v{})
                       }+calculate_mcmc_parallel},
                   f_i(calculate_step_f{},
                       quimulun{
                           Fr(z_v{},
                               []( auto& mt_a, auto const& alfa)
                               {
                                 return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                               }
                               ,non_const<rand_e>{},stretch_alfa{}),

                           Fr{Candidate_mcmc{},[](auto& mt_a,
                                                   auto const& pos,
                                                   auto const& calc_next_fa,
                                                   auto const& nwalkers_a,
                                                   auto const& all_current_mcmc_a,
                                                   auto const& current_mcmc_a ,
                                                   auto const& model_fa,
                                                   auto const& data_ea ,
                                                   auto const& beta_a,
                                                   auto const & z_a)
                              {

                                auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
                                auto var=vector_space(x_i(Pos<beta_ei,walker_ei>{},pos),
                                                        x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
                                                        x_i_view_const(Current_mcmc{},current_mcmc_a),
                                                        x_i_view_const(Size<walker_ei>{},nwalkers_a),
                                                        f_i_view_const(Model_f{},model_fa),
                                                        x_i_view_const(data_ei{},data_ea),
                                                        x_i_view_const(beta_ei{},beta_a),
                                                        x_i_view_const(z_v{},z_a));
                                return random_calculate(calc_next_fa,mt,var);
                              },non_const<rand_e>{},
                              Pos<beta_ei,walker_ei>{},
                              calculate_next_mcmc_f{},
                              Size<walker_ei>{},
                              all<global_Current_mcmc>{},
                              Current_mcmc{},
                              Model_f{},
                              data_ei{},
                              beta_ei{}, z_v{}},
                           F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

                                 auto const& current_logP_a=current_a[logP_k{}]();
                                 auto const& candidate_logP_a=candidate_a[logP_k{}]();
                                 auto const& Parameters_a=current_a[Parameters_ei{}]();

                                 // TODO: implement logpr as a value so difference between logp are properly assesed
                                 return  (candidate_logP_a-current_logP_a )  +
                                        log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

                               }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
                           Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
                                 double A = std::exp(logA_a.value());
                                 double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
                                 return  v(int(r < A),dimension_less{});
                                 //       return  int(r < -1.0);

                               }, non_const<rand_e>{}, logAccept_k{})} )
                   );


  auto next_parallel_f=quimulun{
      Fr(pos<Step_mcmc>{},[](auto& mt_a,
                               auto const& pos_a,
                               auto const& alfa,
                               auto const& calc_step_fa,
                               auto const& calc_next_fa,
                               auto const& nwalkers_a,
                               auto const& all_current_mcmc_a,
                               auto const& current_mcmc_a ,
                               auto const& model_fa,
                               auto const& data_ea ,
                               auto const& beta_a){
            auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
            auto var=vector_space(x_i_view_const(stretch_alfa{},alfa),
                                    f_i_view_const(calculate_next_mcmc_f{},calc_next_fa),
                                    x_i(Pos<beta_ei,walker_ei>{},v(pos_a,dimension_less{})),
                                    x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
                                    x_i_view_const(Current_mcmc{},current_mcmc_a),
                                    x_i_view_const(Size<walker_ei>{},nwalkers_a),
                                    f_i_view_const(Model_f{},model_fa),
                                    x_i_view_const(data_ei{},data_ea),
                                    x_i_view_const(beta_ei{},beta_a));
            return random_calculate(calc_step_fa,mt,var);

          },
          non_const<rand_e>{},
          stretch_alfa{},
          calculate_step_f{},
          calculate_next_mcmc_f{} ,
          Size<walker_ei>{},
          all<Current_mcmc>{},
          Current_mcmc{},
          Model_f{},
          data_ei{},
          beta_ei{})
  };


  auto accept_candidate_or_current_parallel=quimulun{
      F(Next_mcmc{},
          [](auto&& current, auto&& step){
            if (step[accept_k{}]().value())
              return
                  std::move(step)[Candidate_mcmc{}]();
            else
              return std::move(current);}
          ,Current_mcmc{},Step_mcmc{})
  };


  auto temperature_jump_parallel=quimulun{
      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
            using pos_type=std::decay_t<decltype (beta_a.begin())>;
            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<Start<walker_ei>,nwalkers_type>,
                                           x_i<End<walker_ei>,nwalkers_type>>;


            using out_type=vector_field<vec<Transition<beta_ei>>,
                                          elem_type>;
            out_type out;
            auto pos=out.begin();
            auto p=beta_a.begin();

            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
              beta_a.next(p);
            auto p2=p;

            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


            while(beta_a.next(p2))

            {
              auto start_w=v(u(mt_a(p).value()),dimension_less{});
              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
              ++pos[Transition<beta_ei>{}]();
              if (!beta_a.next(p2)) break;
              p=p2;
            }
            return out;
          },
          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
            return Next_mcmc_a(p)[logLik_ei{}]();

          },Transition<beta_ei>{},all<Next_mcmc>{}),
      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
            return Next_mcmc_a(p)[logLik_ei{}]();

          },Transition<beta_ei>{},all<Next_mcmc>{}),
      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
          {
            auto p=beta_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto start_beta_a=beta_a(p);
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto end_beta_a=beta_a(p);

            return  -(start_beta_a - end_beta_a) *
                   (start_logLik_a -end_logLik_a);
          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
            double A = std::exp(logA_a.value());
            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
            return  v<int,dimension_less>(int(r < A));

          }, non_const<Start<rand_e>>{}, logAccept_k{})




  };


  auto actualize_jump_parallel=quimulun{
      F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
          auto p=trans_beta_a.begin();
          auto pos=next_mcmc_a.begin();
          do
          {
            if ( accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_mcmc_a(start_pos));
              next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
              next_mcmc_a(end_pos)=std::move(par_t);
              next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
              next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_mcmc_a);


        }, Next_mcmc{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
  };



  auto currentParameters_parallel_v=random_calculate(inside_init_Parameters_parallel,random_parallel_v,initParameters_q_parallel,modeldata,initwalkers_v);

  auto compute_Evidence_parallel=quimulun{
      F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
          {
            using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
            auto out=beta.template create<value_type>();
            auto pos=out.begin();
            auto n=beta.create();
            auto p=mcmc_a.begin();
            do{
              out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
              n(p)=n(p)+v(1.0);
            }while(mcmc_a.next(p));

            do{
              out(pos)=out(pos)*(v(1)/n(pos));
            }while(out.next(pos));

            return out;
          },Current_mcmc{}, beta_ei{}),
      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            value_type out;
            auto p=beta.begin();
            auto beta0=beta(p);
            auto db_sum=beta0-beta0;
            if (beta0.value()<0.5)
            {
              beta0.value()=0;
              auto beta1=beta(p);
              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
              auto db=(beta1-beta0)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              db_sum=db_sum+db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta2-beta0)/v(2.0);
                out=out+logL0*db;
                db_sum=db_sum+db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
              out=out+logL0*db;
              db_sum=db_sum+db;
              return out;

            }
            else
            {
              beta0.value()=1.0;
              auto beta1=beta(p);
              auto db=(beta0-beta1)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta0-beta2)/v(2.0);
                out=out+logL0*db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2-beta1)*v(1.0/2.0);
              out=out+logL0*db;
              return out;
            }

          }, Average<Current<logLik_ei>>{},beta_ei{} )

  };


  std::string f_jump_name_parallel=filename+"_jump_parallel.txt";

  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());



  for (long int i=0; i<nsamples; ++i)
  {
    auto nsample=vector_space{x_i{isample{},v(i,dimension_less{})}};

    auto next_candidate_parallel_v=random_calculate(next_parallel_f,random_parallel_v,next_inside,currentParameters_parallel_v,modeldata);

    auto output_candidate_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+next_candidate_parallel_v+currentParameters_parallel_v+data;

    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_v), std::move(next_candidate_parallel_v));

    auto output_next_parallel=output_candidate_parallel+next_parameter_parallel_v;

    if (i==0)
    {
      to_DataFrame_title_index("test",output_next_parallel);
    }
    to_DataFrame_body_index("test",i,decimate_factor,output_next_parallel);



    auto after_temperature_jump_parallel=random_calculate(temperature_jump_parallel,rand0_parallel,next_parameter_parallel_v,modeldata);

    auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

    if (i==0)
      to_DataFrame_title(f_jump_parallel,output_jump_parallel);
    if (i%10==0)
      to_DataFrame_body(f_jump_parallel,output_jump_parallel);

    currentParameters_parallel_v=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

    auto Evidence_parallel_v=calculate(compute_Evidence_parallel,currentParameters_parallel_v,modeldata);


    auto output_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+currentParameters_parallel_v+data+Evidence_parallel_v;
    std::cerr<<"\n i_sample="<<i<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

     if (i==0)
    {
      to_DataFrame_title_index(fname_parallel,output_parallel);
    }
    to_DataFrame_body_index(fname_parallel,i,decimate_factor,output_parallel);




  }

  return 1;
}


template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_parallel_parallel_for(const Model_q& model,  const data_q& data,const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                             const v<std::size_t,dimension_less>& nw, std::mt19937::result_type seed, long int nsamples, const std::vector<std::size_t>& decimate_factor,const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);

  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};


  auto calculate_mcmc_parallel=quimulun{
      F(Variables_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
          {
            return calculate(model_fa,data_a,parameters_a);
          },
          Model_f{},data_ei{},Parameters_ei{}),
      F(logPrior_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logPrior(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logLik_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},
          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      F(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, logPrior_ei{},logLik_ei{},beta_ei{})
  };

  auto modeldata=quimulun{
      f_i(Model_f{},model),
      x_i(stretch_alfa{},v(2.0,dimension_less{})),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      x_i(Size<walker_ei>{},nw)
  };


  auto initwalkers_f=quimulun{
      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{})
  };

  auto initwalkers_v=calculate_parallel_for(initwalkers_f,modeldata);

  auto random_f=quimulun{
      Fr(rand_e{},[](auto& mt_a,auto const &, auto const &){
            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
            return v(std::mt19937_64(useed(mt_a.value())),dimension_less{});},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
  };

  /// NOT parallel_for
  auto random_parallel_v=random_calculate(random_f,rand0_parallel,modeldata,initwalkers_v);



  auto initParameters_q_parallel=vector_space(f_i(calculate_initial_mcmc_f{},
                                                    quimulun{
                                                        Fr(Parameters_ei{},[](auto& mt_a,auto const& model_fa,auto const& data_a)
                                                            {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
                                                    }+calculate_mcmc_parallel));


  auto inside_init_Parameters_parallel=quimulun{
      Fr(Current_mcmc{},[](auto& mt_a, auto const& initial_mcmc_fa,auto const& model_fa,auto const& data_a, auto const& beta_a)
          {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
                           //     ,x_i(data_ei{},data_a));
                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
            return random_calculate(initial_mcmc_fa, mt,var);},
          non_const<rand_e>{},calculate_initial_mcmc_f{},Model_f{},data_ei{},beta_ei{})
  };

  auto next_inside=
      vector_space(f_i{calculate_next_mcmc_f{},
                       quimulun{
                           Fr(Parameters_ei{},
                               [](auto &mt_a, auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& nwalkers_a, auto const& z_a)
                               {
                                 auto const& parameter_a=mcmc_a[Parameters_ei{}]();
                                 std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
                                 auto p2=pos_a.value();
                                 p2[walker_ei{}]()= u(mt_a.value());
                                 auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
                                 return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
                               },
                               non_const<rand_e>{},Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Size<walker_ei>{}, z_v{})
                       }+calculate_mcmc_parallel},
                   f_i(calculate_step_f{},
                       quimulun{
                           Fr(z_v{},
                               []( auto& mt_a, auto const& alfa)
                               {
                                 return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
                               }
                               ,non_const<rand_e>{},stretch_alfa{}),

                           Fr{Candidate_mcmc{},[](auto& mt_a,
                                                   auto const& pos,
                                                   auto const& calc_next_fa,
                                                   auto const& nwalkers_a,
                                                   auto const& all_current_mcmc_a,
                                                   auto const& current_mcmc_a ,
                                                   auto const& model_fa,
                                                   auto const& data_ea ,
                                                   auto const& beta_a,
                                                   auto const & z_a)
                              {

                                auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
                                auto var=vector_space(x_i(Pos<beta_ei,walker_ei>{},pos),
                                                        x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
                                                        x_i_view_const(Current_mcmc{},current_mcmc_a),
                                                        x_i_view_const(Size<walker_ei>{},nwalkers_a),
                                                        f_i_view_const(Model_f{},model_fa),
                                                        x_i_view_const(data_ei{},data_ea),
                                                        x_i_view_const(beta_ei{},beta_a),
                                                        x_i_view_const(z_v{},z_a));
                                return random_calculate(calc_next_fa,mt,var);
                              },non_const<rand_e>{},
                              Pos<beta_ei,walker_ei>{},
                              calculate_next_mcmc_f{},
                              Size<walker_ei>{},
                              all<global_Current_mcmc>{},
                              Current_mcmc{},
                              Model_f{},
                              data_ei{},
                              beta_ei{}, z_v{}},
                           F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

                                 auto const& current_logP_a=current_a[logP_k{}]();
                                 auto const& candidate_logP_a=candidate_a[logP_k{}]();
                                 auto const& Parameters_a=current_a[Parameters_ei{}]();

                                 // TODO: implement logpr as a value so difference between logp are properly assesed
                                 return  (candidate_logP_a-current_logP_a )  +
                                        log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

                               }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
                           Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
                                 double A = std::exp(logA_a.value());
                                 double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
                                 return  v(int(r < A),dimension_less{});
                                 //       return  int(r < -1.0);

                               }, non_const<rand_e>{}, logAccept_k{})} )
                   );


  auto next_parallel_f=quimulun{
      Fr(pos<Step_mcmc>{},[](auto& mt_a,
                               auto const& pos_a,
                               auto const& alfa,
                               auto const& calc_step_fa,
                               auto const& calc_next_fa,
                               auto const& nwalkers_a,
                               auto const& all_current_mcmc_a,
                               auto const& current_mcmc_a ,
                               auto const& model_fa,
                               auto const& data_ea ,
                               auto const& beta_a){
            auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
            auto var=vector_space(x_i_view_const(stretch_alfa{},alfa),
                                    f_i_view_const(calculate_next_mcmc_f{},calc_next_fa),
                                    x_i(Pos<beta_ei,walker_ei>{},v(pos_a,dimension_less{})),
                                    x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
                                    x_i_view_const(Current_mcmc{},current_mcmc_a),
                                    x_i_view_const(Size<walker_ei>{},nwalkers_a),
                                    f_i_view_const(Model_f{},model_fa),
                                    x_i_view_const(data_ei{},data_ea),
                                    x_i_view_const(beta_ei{},beta_a));
            return random_calculate(calc_step_fa,mt,var);

          },
          non_const<rand_e>{},
          stretch_alfa{},
          calculate_step_f{},
          calculate_next_mcmc_f{} ,
          Size<walker_ei>{},
          all<Current_mcmc>{},
          Current_mcmc{},
          Model_f{},
          data_ei{},
          beta_ei{})
  };


  auto accept_candidate_or_current_parallel=quimulun{
      F(Next_mcmc{},
          [](auto&& current, auto&& step){
            if (step[accept_k{}]().value())
              return
                  std::move(step)[Candidate_mcmc{}]();
            else
              return std::move(current);}
          ,Current_mcmc{},Step_mcmc{})
  };


  auto temperature_jump_parallel=quimulun{
      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
            using pos_type=std::decay_t<decltype (beta_a.begin())>;
            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
                                           x_i<Start<walker_ei>,nwalkers_type>,
                                           x_i<End<walker_ei>,nwalkers_type>>;


            using out_type=vector_field<vec<Transition<beta_ei>>,
                                          elem_type>;
            out_type out;
            auto pos=out.begin();
            auto p=beta_a.begin();

            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
              beta_a.next(p);
            auto p2=p;

            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


            while(beta_a.next(p2))

            {
              auto start_w=v(u(mt_a(p).value()),dimension_less{});
              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
              ++pos[Transition<beta_ei>{}]();
              if (!beta_a.next(p2)) break;
              p=p2;
            }
            return out;
          },
          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
            return Next_mcmc_a(p)[logLik_ei{}]();

          },Transition<beta_ei>{},all<Next_mcmc>{}),
      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
          {
            auto p=Next_mcmc_a.begin();
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
            return Next_mcmc_a(p)[logLik_ei{}]();

          },Transition<beta_ei>{},all<Next_mcmc>{}),
      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
          {
            auto p=beta_a.begin();
            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto start_beta_a=beta_a(p);
            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
            auto end_beta_a=beta_a(p);

            return  -(start_beta_a - end_beta_a) *
                   (start_logLik_a -end_logLik_a);
          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
            double A = std::exp(logA_a.value());
            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
            return  v<int,dimension_less>(int(r < A));

          }, non_const<Start<rand_e>>{}, logAccept_k{})




  };


  auto actualize_jump_parallel=quimulun{
      F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
          auto p=trans_beta_a.begin();
          auto pos=next_mcmc_a.begin();
          do
          {
            if ( accept_k(p).value())
            {
              auto start_pos=pos;
              auto end_pos=pos;
              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

              auto par_t=std::move(next_mcmc_a(start_pos));
              next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
              next_mcmc_a(end_pos)=std::move(par_t);
              next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
              next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
            }

          }while(trans_beta_a.next(p));

          return std::move(next_mcmc_a);


        }, Next_mcmc{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
  };



  auto currentParameters_parallel_v=random_calculate_parallel_for(inside_init_Parameters_parallel,random_parallel_v,initParameters_q_parallel,modeldata,initwalkers_v);

  auto compute_Evidence_parallel=quimulun{
      F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
          {
            using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
            auto out=beta.template create<value_type>();
            auto pos=out.begin();
            auto n=beta.create();
            auto p=mcmc_a.begin();
            do{
              out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
              n(p)=n(p)+v(1.0);
            }while(mcmc_a.next(p));

            do{
              out(pos)=out(pos)*(v(1)/n(pos));
            }while(out.next(pos));

            return out;
          },Current_mcmc{}, beta_ei{}),
      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
          {
            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
            value_type out;
            auto p=beta.begin();
            auto beta0=beta(p);
            auto db_sum=beta0-beta0;
            if (beta0.value()<0.5)
            {
              beta0.value()=0;
              auto beta1=beta(p);
              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
              auto db=(beta1-beta0)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              db_sum=db_sum+db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta2-beta0)/v(2.0);
                out=out+logL0*db;
                db_sum=db_sum+db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
              out=out+logL0*db;
              db_sum=db_sum+db;
              return out;

            }
            else
            {
              beta0.value()=1.0;
              auto beta1=beta(p);
              auto db=(beta0-beta1)*v(1.0/2.0);
              auto logL0=logLik(p);
              out=logL0*db;
              while(beta.next(p))
              {
                auto beta2=beta(p);
                auto db=(beta0-beta2)/v(2.0);
                out=out+logL0*db;
                beta0=beta1;
                beta1=beta2;
                logL0=logLik(p);
              }
              auto beta2=v(1.0,dimension_less{});
              db=(beta2-beta1)*v(1.0/2.0);
              out=out+logL0*db;
              return out;
            }

          }, Average<Current<logLik_ei>>{},beta_ei{} )

  };


  std::string f_jump_name_parallel=filename+"_jump_parallel.txt";

  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());



  for (long int i=0; i<nsamples; ++i)
  {
    auto nsample=vector_space{x_i{isample{},v(i,dimension_less{})}};

    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_f,random_parallel_v,next_inside,currentParameters_parallel_v,modeldata);

    auto output_candidate_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+next_candidate_parallel_v+currentParameters_parallel_v+data;

    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_v), std::move(next_candidate_parallel_v));

    auto output_next_parallel=output_candidate_parallel+next_parameter_parallel_v;

    if (i==0)
    {
      to_DataFrame_title_index("test",output_next_parallel);
    }
    to_DataFrame_body_index("test",i,decimate_factor,output_next_parallel);



    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel,next_parameter_parallel_v,modeldata);

    auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

    if (i==0)
      to_DataFrame_title(f_jump_parallel,output_jump_parallel);
    if (i%10==0)
      to_DataFrame_body(f_jump_parallel,output_jump_parallel);

    currentParameters_parallel_v=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_v,modeldata);


    auto output_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+currentParameters_parallel_v+data+Evidence_parallel_v;
    std::cerr<<"\n i_sample="<<i<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

    if (i==0)
    {
      to_DataFrame_title_index(fname_parallel,output_parallel);
    }
    to_DataFrame_body_index(fname_parallel,i,decimate_factor,output_parallel);




  }

  return 1;
}






#endif // QM_METROPOLIS_PARALLEL_H
