#ifndef QM_METROPOLIS_PARALLEL_H
#define QM_METROPOLIS_PARALLEL_H

#include <qm_tensor_distribution_evaluation.h>
//#include <qm_Metropolis_Distributions.h>
#include <qm_tensor_model_evaluation.h>
#include <qm_tensor_model_evaluation_new.h>

#include "qm_data_frame.h"
#include <fstream>
#include <chrono>
struct data_ei{  constexpr static auto  className=my_static_string("data");};
struct beta_ei{  constexpr static auto  className=my_static_string("beta");};
struct walker_ei{  constexpr static auto  className=my_static_string("walker");};
struct logPrior_ei{   constexpr static auto  className=my_static_string("logPrior");};
struct logLik_ei{   constexpr static auto  className=my_static_string("logLik");};

struct isample{constexpr static auto className=my_static_string("i_sample");};

struct seed_e{constexpr static auto className=my_static_string("seed_e");};

struct rand_e{constexpr static auto className=my_static_string("rand_e");};

struct Model_f{ constexpr static auto className=my_static_string("Model_f");};

struct Evidence_ei{ constexpr static auto className=my_static_string("Evidence");};


struct z_v{ constexpr static auto className=my_static_string("zeta_value");};

struct Parameters_ei{  constexpr static auto  className=my_static_string("Parameters");};
struct Variables_ei{  constexpr static auto  className=my_static_string("Variables");};

struct logP_k{   constexpr static auto  className=my_static_string("logP");};
struct accept_k{  constexpr static auto  className=my_static_string("accept");};
struct logAccept_k{  constexpr static auto  className=my_static_string("logAccept");};

template<class Id> struct jump{constexpr static auto className=Id::className+my_static_string("_jump");};

template<class Id> struct Current{constexpr static auto className=Id::className+my_static_string("_current");};
template<class Id> struct Candidate{constexpr static auto className=Id::className+my_static_string("_candidate");};

template<class Id> struct Forward{constexpr static auto className=Id::className+my_static_string("_forward");};
template<class Id> struct Backward{constexpr static auto className=Id::className+my_static_string("_backward");};


template<class Id> struct Proposal_Distribution{constexpr static auto className=Id::className+my_static_string("_Proposal_Distribution");};


template<class Id> struct Next{constexpr static auto className=Id::className+my_static_string("_next");};
template<class Id> struct Chosen{constexpr static auto className=Id::className+my_static_string("_chosen");};
template<class Id> struct Index_of{constexpr static auto className=Id::className+my_static_string("_index");};
template<class... Id> struct Pos{constexpr static auto className=(Id::className+...+my_static_string("_position"));};
template<class Id> struct Average{constexpr static auto className=Id::className+my_static_string("_average");};
//template<class Id> struct Stddev{constexpr static auto className=Id::className+my_static_string("_stddev");};

struct Current_mcmc{constexpr static auto className=my_static_string("current_mcmc");};
struct mcmc{constexpr static auto className=my_static_string("mcmc");};


struct global_Current_mcmc{constexpr static auto className=my_static_string("global_current_mcmc");};
struct Candidate_mcmc{constexpr static auto className=my_static_string("candidate_mcmc");};
struct Next_mcmc{constexpr static auto className=my_static_string("next_mcmc");};
struct Step_mcmc{constexpr static auto className=my_static_string("step");};

template<class Id> struct Transition{constexpr static auto className=Id::className+my_static_string("_transition");};
struct stretch_alfa{constexpr static auto className=my_static_string("stretch_alfa");};




struct calculate_initial_mcmc_fi{constexpr static auto className=my_static_string("calculate_initial_mcmc_f");};
struct calculate_next_mcmc_fi{constexpr static auto className=my_static_string("calculate_next_mcmc_f");};
struct calculate_step_fi{constexpr static auto className=my_static_string("calculate_step_f");};



template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_parallel_parallel_for(const Model_q& model,
                                          const data_q& data,
                                          const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                                          const v<std::size_t,dimension_less>& nw,
                                          std::mt19937::result_type seed,
                                          long int nsamples,
                                          const std::vector<std::size_t>& decimate_factor,
                                          const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);

  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel_v=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};


  auto calculate_mcmc_parallel_f=quimulun{
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

  /// NOT parallel_for
  auto random_parallel_v=random_calculate(random_f,rand0_parallel_v,modeldata,initwalkers_v);



  auto initParameters_q_parallel=vector_space(f_i(calculate_initial_mcmc_fi{},
                                                    quimulun{
                                                        Fr(Parameters_ei{},[](auto& mt_a,auto & model_fa,auto const& data_a)
                                                            {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
                                                    }+calculate_mcmc_parallel_f));


  auto inside_init_Parameters_parallel=quimulun{
      Fr(Current_mcmc{},[](auto& mt_a, auto const& initial_mcmc_fa,auto const& model_fa,auto const& data_a, auto const& beta_a)
          {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
                           //     ,x_i(data_ei{},data_a));
                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
            return random_calculate(initial_mcmc_fa, mt,var);},
          non_const<rand_e>{},calculate_initial_mcmc_fi{},Model_f{},data_ei{},beta_ei{})
  };

  auto next_inside=
      vector_space(f_i{calculate_next_mcmc_fi{},
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
                       }+calculate_mcmc_parallel_f},
                   f_i(calculate_step_fi{},
                       quimulun{
                           Fr(z_v{},
                               []( auto& mt_a, auto const& alfa)
                               {
                                 return  v(myInvoke(Sample{},stretch_move_Distribution<double>{},mt_a.value(),alfa.value()),dimension_less{});
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
                              calculate_next_mcmc_fi{},
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
                                    f_i_view_const(calculate_next_mcmc_fi{},calc_next_fa),
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
          calculate_step_fi{},
          calculate_next_mcmc_fi{} ,
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


        }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
  };



  auto currentParameters_parallel_v=random_calculate_parallel_for(
      inside_init_Parameters_parallel,random_parallel_v,initParameters_q_parallel,modeldata,initwalkers_v);

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


  std::string f_jump_name_parallel=fname_parallel+"_jump_parallel.txt";

  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());

  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();


  for (long int i=0; i<nsamples; ++i)
  {

    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_f,random_parallel_v,next_inside,currentParameters_parallel_v,modeldata);

    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_v), std::move(next_candidate_parallel_v));

    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


    if (i==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

    }
    if (i%10000==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
    }
    currentParameters_parallel_v=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_v,modeldata);

    auto tnow=std::chrono::steady_clock::now();
    auto d=tnow-startTime;
    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
    if (i%100==0)
      std::cerr<<"\n i_sample="<< i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

    if (i==0)
    {
      //   to_DataFrame_title_index(fname,output);
      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_v,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

    }
    // to_DataFrame_body_index(fname,i,decimate_factor,output);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_v,data,modeldata,initwalkers_v);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);



  }

  return 1;
}


template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_parallel_parallel_for_q(const Model_q& model,
                                            const data_q& data,
                                            const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                                            const v<std::size_t,dimension_less>& nw,
                                            std::mt19937::result_type seed,
                                            long int nsamples,
                                            const std::vector<std::size_t>& decimate_factor,
                                            const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);
  
  
  
  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel_v=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};

  auto calculate_mcmc_parallel_q=quimulun{
      Fq(Variables_ei{},Calculator{},Model_f{},data_ei{},Parameters_ei{}),
      Fq(logPrior_ei{},Calculator_logPrior{},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
      Fq(logLik_ei{},Calculator_logLikelihood{},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
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

  /// NOT parallel_for
  auto random_parallel_v=random_calculate(random_f,rand0_parallel_v,modeldata,initwalkers_v);



  auto initParameters_q_parallel_q=vector_space(f_i(calculate_initial_mcmc_fi{},
                                                      quimulun{
                                                          Frq(Parameters_ei{},Calculator_sampleParameters{},Model_f{},non_const<rand_e>{},data_ei{})
                                                      }+calculate_mcmc_parallel_q));



  auto inside_init_Parameters_parallel_q=quimulun{
      Frq(Current_mcmc{},Calculator_random{},calculate_initial_mcmc_fi{},non_const<pass_id<rand_e>>{},pass_id<Model_f>{},pass_id<data_ei>{},pass_id<beta_ei>{})};


  auto next_inside_q=
      vector_space(f_i{calculate_next_mcmc_fi{},
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
                       }+calculate_mcmc_parallel_q},
                   f_i(calculate_step_fi{},
                       quimulun{
                           Fr(z_v{},
                               []( auto& mt_a, auto const& alfa)
                               {
                                 return  v(myInvoke(Sample{},stretch_move_Distribution<double>{},mt_a.value(),alfa.value()),dimension_less{});
                               }
                               ,non_const<rand_e>{},stretch_alfa{}),
                           Frq{Candidate_mcmc{},
                               Calculator_random{},
                               calculate_next_mcmc_fi{},
                               non_const<pass_id<rand_e>>{},
                               pass_id<Pos<beta_ei,walker_ei>>{},
                               pass_id<Size<walker_ei>>{},
                               pass_id<all<global_Current_mcmc>>{},
                               pass_id<Current_mcmc>{},
                               pass_id<Model_f>{},
                               pass_id<data_ei>{},
                               pass_id<beta_ei>{}, pass_id<z_v>{}},
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


  auto next_parallel_q=quimulun{
      View(global_Current_mcmc{},Current_mcmc{}),
      Frq(pos<Step_mcmc>{}
          ,Calculator_random{},
          calculate_step_fi{},
          non_const<pass_id<rand_e>>{},
          Pos<beta_ei,walker_ei>{},
          pass_id<stretch_alfa>{},
          pass_id<calculate_step_fi>{},
          pass_id<calculate_next_mcmc_fi>{} ,
          pass_id<Size<walker_ei>>{},
          pass_id<all<global_Current_mcmc>>{},
          pass_id<Current_mcmc>{},
          pass_id<Model_f>{},
          pass_id<data_ei>{},
          pass_id<beta_ei>{})

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


        }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
  };

  auto currentParameters_parallel_q=random_calculate_parallel_for(
      inside_init_Parameters_parallel_q,random_parallel_v,initParameters_q_parallel_q,modeldata,initwalkers_v);

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

  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();


  for (long int i=0; i<nsamples; ++i)
  {



    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_q,random_parallel_v,next_inside_q,currentParameters_parallel_q,modeldata);

    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_q), std::move(next_candidate_parallel_v));

    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


    if (i==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

    }
    if (i%100000==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
    }
    currentParameters_parallel_q=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_q,modeldata);

    auto tnow=std::chrono::steady_clock::now();
    auto d=tnow-startTime;
    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
    if (i%100==0)
      std::cerr<<"\n i_sample="<<i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

    if (i==0)
    {
      //   to_DataFrame_title_index(fname,output);
      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_q,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

    }
    // to_DataFrame_body_index(fname,i,decimate_factor,output);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_q,data,modeldata,initwalkers_v);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);
  }

  return 1;
}


template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
auto parallel_emcee_parallel_parallel_for_q2(const Model_q& model,
                                             const data_q& data,
                                             const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                                             const v<std::size_t,dimension_less>& nw,
                                             std::mt19937::result_type seed,
                                             long int nsamples,
                                             const std::vector<std::size_t>& decimate_factor,
                                             const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);



  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel_v=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};

  auto calculate_mcmc_parallel_q=quimulun{
      Fq_new(Variables_ei{},Calculator_new{},Model_f{},Arguments<data_ei,Parameters_ei>{}),
      Fq_new(logPrior_ei{},logPrior_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
      Fq_new(logLik_ei{},logLikelihood_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
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
      F(Size<seed_e>{},[](auto const&, auto const&){return v(std::mt19937_64::max(),dimension_less{});},beta_ei{},walker_ei{}),
      D(seed_e{},Uniform_Integer_Distribution<typename std::mt19937_64::result_type>{},Arguments<Size<seed_e>>{}),
      F(rand_e{},[](auto seed_a){
            return v(std::mt19937_64(seed_a.value()),dimension_less{});},seed_e{})
  };

  /// NOT parallel_for
  auto random_parallel_v=sample(random_f,rand0_parallel_v[non_const<Start<rand_e>>{}](),modeldata,initwalkers_v);



  auto initParameters_q_parallel_q=vector_space(f_i(calculate_initial_mcmc_fi{},
                                                      quimulun{
                                                          Dq_new(Parameters_ei{},Sample{},Model_f{},Arguments<data_ei>{})
                                                      }+calculate_mcmc_parallel_q));



  auto inside_init_Parameters_parallel_q=quimulun{
      Dq_new(Current_mcmc{},Calculator_random{},calculate_initial_mcmc_fi{},Arguments<pass_id<Model_f>,pass_id<data_ei>,pass_id<beta_ei>>{})};


  auto next_inside_q=
      vector_space(f_i{calculate_next_mcmc_fi{},
                       quimulun{
                           D(Chosen<walker_ei>{},Uniform_Integer_Distribution<std::size_t>{},Arguments<Size<walker_ei>>{}),
                           F(Parameters_ei{},
                               [](auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& chosen_walker_a, auto const& z_a)
                               {
                                 auto const& parameter_a=mcmc_a[Parameters_ei{}]();
                                 auto p2=pos_a.value();
                                 p2[walker_ei{}]()= chosen_walker_a;
                                 auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
                                 return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
                               },
                               Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Chosen<walker_ei>{}, z_v{})
                       }+calculate_mcmc_parallel_q},
                   f_i(calculate_step_fi{},
                       quimulun{
                           D(z_v{},stretch_move_Distribution<double>{},Arguments<stretch_alfa>{}),
                           Dq_new{Candidate_mcmc{},
                                  Calculator_random{},
                                  calculate_next_mcmc_fi{},
                                  Arguments<pass_id<Pos<beta_ei,walker_ei>>,
                                            pass_id<Size<walker_ei>>,
                                            pass_id<all<global_Current_mcmc>>,
                                            pass_id<Current_mcmc>,
                                            pass_id<Model_f>,
                                            pass_id<data_ei>,
                                            pass_id<beta_ei>, pass_id<z_v>>{}},
                           F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

                                 auto const& current_logP_a=current_a[logP_k{}]();
                                 auto const& candidate_logP_a=candidate_a[logP_k{}]();
                                 auto const& Parameters_a=current_a[Parameters_ei{}]();

                                 // TODO: implement logpr as a value so difference between logp are properly assesed
                                 return  (candidate_logP_a-current_logP_a )  +
                                        log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

                               }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
                           D(accept_k{},Bernoulli_Distribution{},Arguments<logAccept_k>{})} )
                   );


  auto next_parallel_q=quimulun{
      View(global_Current_mcmc{},Current_mcmc{}),
      Dq_new(pos<Step_mcmc>{}
             ,Calculator_random{},
             calculate_step_fi{},
             Arguments<Pos<beta_ei,walker_ei>,
                       pass_id<stretch_alfa>,
                       pass_id<calculate_step_fi>,
                       pass_id<calculate_next_mcmc_fi> ,
                       pass_id<Size<walker_ei>>,
                       pass_id<all<global_Current_mcmc>>,
                       pass_id<Current_mcmc>,
                       pass_id<Model_f>,
                       pass_id<data_ei>,
                       pass_id<beta_ei>>{})

  };

  auto accept_candidate_or_current_parallel=quimulun{
      F_new(Next_mcmc{},
          [](auto&& current, auto&& step){
            if (step[accept_k{}]().value())
              return
                  std::move(step)[Candidate_mcmc{}]();
            else
              return std::move(current);}
          ,Arguments<Current_mcmc,Step_mcmc>{})
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


        }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
  };

  auto currentParameters_parallel_q=random_calculate_parallel_for(
      inside_init_Parameters_parallel_q,random_parallel_v,initParameters_q_parallel_q,modeldata,initwalkers_v);

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

  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();


  for (long int i=0; i<nsamples; ++i)
  {



    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_q,random_parallel_v,next_inside_q,currentParameters_parallel_q,modeldata);

    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_q), std::move(next_candidate_parallel_v));

    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


    if (i==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

    }
    if (i%100000==0)
    {
      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
    }
    currentParameters_parallel_q=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_q,modeldata);

    auto tnow=std::chrono::steady_clock::now();
    auto d=tnow-startTime;
    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
    if (i%100==0)
      std::cerr<<"\n i_sample="<<i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

    if (i==0)
    {
      //   to_DataFrame_title_index(fname,output);
      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_q,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

    }
    // to_DataFrame_body_index(fname,i,decimate_factor,output);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_q,data,modeldata,initwalkers_v);
    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);
  }

  return 1;
}







struct parallel_model_q
{
template<typename Model_f,typename data_ei, typename beta_ei, typename walker_ei>
  auto operator()(Arguments<Model_f, data_ei, beta_ei, walker_ei>&&)
{
  return quimulun{
      Dq_new(Parameters_ei{},SampleParameters{},Model_f{},Arguments<data_ei>{},Index_struct<beta_ei,walker_ei>{}),
      Fq_new(Variables_ei{},Calculator_new{},Model_f{},Arguments<data_ei,Parameters_ei>{}),
      Fq_new(logPrior_ei{},logPrior_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
      Fq_new(logLik_ei{},logLikelihood_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
      F_new(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;},Arguments< logPrior_ei,logLik_ei,beta_ei>{})
  };
}
};

struct Stretch_move_q {

  template<class Parameters_ei,class walker_ei,class stretch_alfa>
  auto operator()(Arguments<Parameters_ei, Size_at_Index_new<Parameters_ei,walker_ei>,stretch_alfa>&&)const
  {
  return quimulun{
        D(Chosen<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<Size_at_Index_new<Parameters_ei,walker_ei>>{}),
        F_new(Index_of<Chosen<walker_ei>>{},
          [](auto pos, auto const& walker_a, auto const&  size_walker_a){
              pos[walker_ei{}]()=(pos[walker_ei{}]()+walker_a.value()) % size_walker_a.value();
            return pos;},
          Arguments<pos_new<Parameters_ei>,Chosen<walker_ei>,Size_at_Index_new<Parameters_ei,walker_ei>>{}),
        D(z_v{},stretch_move_Distribution<double>{},Arguments<stretch_alfa>{},Index_struct<Parameters_ei>{}),
        F_new(Out<Parameters_ei>{},
          [](auto const& parameter_a,auto const& chosen_parameter, auto const& z_a)
          {
            return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
          },Arguments<Parameters_ei,subElement<Parameters_ei,Index_of<Chosen<walker_ei>>>,z_v>{})
  };
}
};


struct temperature_jump_eval_q {
  struct even_or_odd{};

  template<class beta_ei, class num_walkers_ei, class mcmc>
  auto operator()(Arguments<beta_ei, num_walkers_ei, mcmc>&&)const
  {
    return quimulun
        {
            D(even_or_odd{}, Bernoulli_Distribution{}, Arguments<>{}),
            Coord_new(index_new<Transition<beta_ei>>{},FoG{IndexCoordinate{}, [](auto size_beta_a, auto even_or_odd_a){
                  if (even_or_odd_a.value()) return (size_beta_a-1)/2;
                                                              else return size_beta_a/2;}},
                Arguments<Size<beta_ei>,even_or_odd>{}),
            F_new(From<num_walkers_ei>{},
                [](auto index_transition, auto const& walker_size_a, auto even_or_odd_a){
                  auto pos=Position<beta_ei>{};
                  pos[beta_ei{}]()=index_transition.value()*2;
                  if (even_or_odd_a) ++pos[beta_ei{}]();
                  return walker_size_a[pos];
                },
                Arguments<index_new<Transition<beta_ei>>,num_walkers_ei,even_or_odd>{}),
            F_new(To<num_walkers_ei>{},
                [](auto index_transition, auto const& walker_size_a, auto even_or_odd_a){
                  auto pos=Position<beta_ei>{};
                  pos[beta_ei{}]()=index_transition.value()*2+1;
                  if (even_or_odd_a) ++pos[beta_ei{}]();
                  return walker_size_a[pos];
                },
                Arguments<index_new<Transition<beta_ei>>,num_walkers_ei,even_or_odd>{}),
            D(From<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<From<num_walkers_ei>>{}),
            D(To<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<To<num_walkers_ei>>{}),
            F_new(pos_new<From<walker_ei>>{},[](auto const &index_a,auto const& From_walker, auto even_or_odd_a){
                  auto pos=Position<beta_ei,walker_ei>{};
                  std::size_t i0=even_or_odd_a?1:0;
                  pos[beta_ei{}]()=index_a*2+i0;
                  pos[walker_ei{}]()=From_walker.value();
                  return pos;
                },Arguments<index_new<Transition<beta_ei>>,From<walker_ei>,even_or_odd>{}),
            F_new(pos_new<To<walker_ei>>{},[](auto const &index_a,auto const& To_walker,  auto even_or_odd_a){
                  auto pos=Position<beta_ei,walker_ei>{};
                  std::size_t i0=even_or_odd_a?1:0;
                  pos[beta_ei{}]()=index_a*2+i0+1;
                  pos[walker_ei{}]()=To_walker.value();
                  return pos;
                },Arguments<index_new<Transition<beta_ei>>,To<walker_ei>,even_or_odd>{}),
            F_new(Out<logAccept_k>{},[](auto const &start_beta_a,auto const& end_beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
                {

                  return  -(start_beta_a - end_beta_a) *
                         (start_logLik_a -end_logLik_a);
                },Arguments<subElement<beta_ei,pos_new<From<walker_ei>>>,
                          subElement<beta_ei,pos_new<To<walker_ei>>>,
                          subElement<sub<mcmc,logLik_ei>,pos_new<From<walker_ei>>>,
                          subElement<sub<mcmc,logLik_ei>,pos_new<To<walker_ei>>>>{}),
            D(Out<accept_k>{},Bernoulli_Distribution{},Arguments<Out<logAccept_k>>{}),
                    };
  }

};

struct temperature_jump_exec_q {
  struct even_or_odd{};

  template< class Parameters_ei, class walker_ei, class accept_k,class mcmc>
  auto operator()(Arguments<Parameters_ei,  walker_ei, accept_k,mcmc>&&)const
  {
    return quimulun
        {
            F_new(all<Out<Parameters_ei>>{},[](auto&& parameters_a, auto const& to_w, auto const& from_w,  auto const& accept_a){
                  auto p=to_w.begin();
                  do
                  {
                    if ( accept_a(p).value())
                    {
                      auto start_pos=from_w(p);
                      auto end_pos=to_w(p);

                      auto par_t=std::move(parameters_a(start_pos));
                      parameters_a(start_pos)=std::move(parameters_a(end_pos));
                      parameters_a(end_pos)=std::move(par_t);
                    }

                  }while(to_w.next(p));

                  return std::move(parameters_a);


                }, Arguments<non_const<Parameters_ei>, pos_new<To<walker_ei>>,pos_new<From<walker_ei>>, accept_k>{}),
            F_new(all<Out<mcmc>>{},[](auto&& next_mcmc_a, auto const& to_w, auto const& from_w,  auto const& accept_a, auto const& beta_a){
                  auto p=to_w.begin();
                  do
                  {
                    if ( accept_a(p).value())
                    {
                      auto start_pos=from_w(p);
                      auto end_pos=to_w(p);

                      auto par_t=std::move(next_mcmc_a(start_pos));
                      next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
                      next_mcmc_a(end_pos)=std::move(par_t);
                      next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
                      next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
                    }

                  }while(to_w.next(p));

                  return std::move(next_mcmc_a);


                }, Arguments<non_const<mcmc>, pos_new<To<walker_ei>>,pos_new<From<walker_ei>>, accept_k, beta_ei>{})
        };
  }

};

struct nstep{};
struct Proposal{
  constexpr static auto  className=my_static_string("Proposal");

};

struct Metropolis_Hastings_emcee_q
{
template< class model_fi,class model_data_ei, class proposed_distribution_fi, class distribution_data_ei>
  auto operator() (Arguments<model_fi, model_data_ei,proposed_distribution_fi,distribution_data_ei>&&)const
{
  return quimulun{
      Dq_new(Current<Parameters_ei>{},Sample{},model_fi{},Arguments<model_data_ei>{}),
      Fq_new(Current<mcmc>{},Calculator_new{},model_fi{},Arguments<Current<Parameters_ei>,model_data_ei>{}),

      Dq_new(Proposal{},Sample{},proposed_distribution_fi{},Arguments<Current<Parameters_ei>,distribution_data_ei>{}),
      Fq_new(Candidate<mcmc>{},Calculator_new{},model_fi{},Arguments<Proposal,model_data_ei>{}),

      F_new(logAccept_k{},[](auto const& current_logP_a,auto const& candidate_logP_a, auto size_param, auto z_a){

            return  (candidate_logP_a-current_logP_a )  +
                   log(z_a) * v(size_param - 1.0,dimension_less{});

          }, Arguments<sub<Current<mcmc>,logP_k>, sub<Candidate<mcmc>,logP_k>,Size<sub<Current<Parameters_ei>,Parameters_ei>>,sub<Proposal,z_v>>{}),

      D(accept_k{},Bernoulli_Distribution{},Arguments<logAccept_k>{}),
      F_new(sub<Out<Current<Parameters_ei>>,Parameters_ei>{},[](bool accept, auto&& current_xs, auto&& proposal)
          {
            return accept? std::move(proposal): std::move(current_xs);
          }, Arguments<accept_k,sub<Current<Parameters_ei>,Parameters_ei>,sub<Proposal,Parameters_ei>>{}),
      F_new(Out<Current<mcmc>>{},[](bool accept, auto&& current_mcmc, auto&& candidate_mcmc)
          {
            return accept? std::move(candidate_mcmc): std::move(current_mcmc);
          }, Arguments<accept_k,Current<mcmc>,Candidate<mcmc>>{}),


  };

}
};

struct Parallel_Metropolis_Hastings_emcee_step_q
{
  struct nstep{};
  struct Thermo_Jump{};

  template< class emcee_fi,class input_data_ei, class thermo_jump_result_fi, class thermo_jump_apply_result_fi>
  auto operator() (Arguments<emcee_fi, input_data_ei,thermo_jump_result_fi, thermo_jump_apply_result_fi>&&)const
  {


    return quimulun
    {

            Dq_new(Start_new<Step_mcmc,nstep>{},Sample{},emcee_fi{},Arguments<input_data_ei>{}),
            Dq_new(Candidate_mcmc{},Sample{},emcee_fi{},Arguments<Step_mcmc,input_data_ei>{}),
            Dq_new(Thermo_Jump{},Sample{},thermo_jump_result_fi{},Arguments<Step_mcmc,input_data_ei>{}),
            Fq_new(Next_new<Step_mcmc,nstep>{},Calculator_new{},thermo_jump_apply_result_fi{}, Arguments<Thermo_Jump, Candidate_mcmc>{})


        };



  }
};

struct parallel_model{};
struct parallel_data{};
struct proposed_distribution_fi{};
struct distribution_data_ei{};
struct emcee_fi{};
struct input_data_ei{};
struct thermo_jump_result_fi{};
struct thermo_jump_apply_result_fi{};
struct Parallel_Metropolis_Hastings_emcee_q
{

template< class Model_f,class data_ei, class beta_ei, class num_walkers_ei, class walker_ei, class distribution_data_ei>
  auto operator() (Arguments<num_walkers_ei,Model_f, data_ei,beta_ei, walker_ei,distribution_data_ei>&&)const
{



  auto model_def=quimulun{
      Coord_new(walker_ei{},IndexCoordinate{},Arguments<num_walkers_ei>{}),
      f_i(parallel_model{},parallel_model_q{}(Arguments<Model_f, data_ei, beta_ei, walker_ei>{})),
      F_new(parallel_data{},Glue_new{},Arguments<Model_f,data_ei,beta_ei,walker_ei>{}),
      f_i(proposed_distribution_fi{},Stretch_move_q{}(Arguments<Parameters_ei, Size_at_Index_new<Parameters_ei,walker_ei>,stretch_alfa>{})),
      F_new(distribution_data_ei{},Glue_new{},Arguments<stretch_alfa>{}),
      f_i(emcee_fi{},Metropolis_Hastings_emcee_q{}(Arguments<parallel_model, parallel_data,proposed_distribution_fi,distribution_data_ei>{})),
      F_new(input_data_ei{},Glue_new{},Arguments<parallel_model, parallel_data,proposed_distribution_fi,distribution_data_ei>{}),
      f_i(thermo_jump_result_fi{},temperature_jump_eval_q{}(Arguments<beta_ei, num_walkers_ei, mcmc>{})),
      f_i(thermo_jump_apply_result_fi{},temperature_jump_exec_q{}(Arguments<Parameters_ei,  walker_ei, accept_k,mcmc>{}))


  };


  return std::move(model_def)+
         Parallel_Metropolis_Hastings_emcee_step_q{}
         (Arguments<emcee_fi, input_data_ei,thermo_jump_result_fi, thermo_jump_apply_result_fi>{});




}
};





struct parallel_emcee_parallel_parallel_for_declarativa {

  struct   num_walkers_ei{};
  struct distribution_data_ei{};

template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
  auto operator()(const Model_q& model,
                                                      const data_q& data,
                                                      const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
                                                      const v<std::size_t,dimension_less>& nw,
                                                      std::mt19937::result_type seed,
                                                      long int nsamples,
                                                      const std::vector<std::size_t>& decimate_factor,
                                                      const std::string& filename)
{
  std::mt19937_64 mt0_parallel(seed);

  auto mt=v(mt0_parallel,dimension_less{});
  auto modeldata=quimulun{
      f_i(Model_f{},model),
      x_i(stretch_alfa{},v(2.0,dimension_less{})),
      x_i(data_ei{},data),
      x_i(beta_ei{},betas),
      x_i(num_walkers_ei{},nw)
  };

  auto mcee=Parallel_Metropolis_Hastings_emcee_q{}(Arguments<num_walkers_ei,Model_f, data_ei,beta_ei, walker_ei,distribution_data_ei>{});

  auto res=sample_Operation(mcee,mt,modeldata);




  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

  auto rand0_parallel_v=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};













  std::string f_jump_name_parallel=filename+"_jump_parallel.txt";

  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());

  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();





//  for (long int i=0; i<nsamples; ++i)
//  {



//    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_q,random_parallel_v,next_inside_q,currentParameters_parallel_q,modeldata);

//    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_q), std::move(next_candidate_parallel_v));

//    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


//    if (i==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
//      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

//    }
//    if (i%100000==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

//      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
//    }
//    currentParameters_parallel_q=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

//    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_q,modeldata);

//    auto tnow=std::chrono::steady_clock::now();
//    auto d=tnow-startTime;
//    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
//    if (i%100==0)
//      std::cerr<<"\n i_sample="<<i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

//    if (i==0)
//    {
//      //   to_DataFrame_title_index(fname,output);
//      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_q,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
//      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

//    }
//    // to_DataFrame_body_index(fname,i,decimate_factor,output);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_q,data,modeldata,initwalkers_v);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);
//  }

  return 1;
}

};

//template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
//auto parallel_emcee_parallel_parallel_for_declarative(const Model_q& model,
//                                            const data_q& data,
//                                            const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
//                                            const v<std::size_t,dimension_less>& nw,
//                                            std::mt19937::result_type seed,
//                                            long int nsamples,
//                                            const std::vector<std::size_t>& decimate_factor,
//                                            const std::string& filename)
//{


//    std::mt19937_64 mt0_parallel(seed);





//  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

//  auto rand0_parallel_v=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};

//  auto calculate_mcmc_parallel_q=quimulun{
//      Fq(Variables_ei{},Calculator{},Model_f{},data_ei{},Parameters_ei{}),
//      Fq(logPrior_ei{},Calculator_logPrior{},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
//      Fq(logLik_ei{},Calculator_logLikelihood{},Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
//      F(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, logPrior_ei{},logLik_ei{},beta_ei{})
//  };
//  auto modeldata=quimulun{
//      f_i(Model_f{},model),
//      x_i(stretch_alfa{},v(2.0,dimension_less{})),
//      x_i(data_ei{},data),
//      x_i(beta_ei{},betas),
//      x_i(Size<walker_ei>{},nw)
//  };


//  auto random_f=quimulun{
//      Sampler(rand_e{},[](auto const& seed_a){return v(std::mt19937_64(seed_a),dimension_less{});},
//         Uniform_Integer_Distribution<typename std::mt19937_64::result_type>{},
// [](auto& mt_a,auto const &, auto const &){return mt_a;},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
//  };


//  auto initParameters_q_parallel_q=vector_space(f_i(calculate_initial_mcmc_fi{},
//                                                      quimulun{
//                                                          Frq(Parameters_ei{},Calculator_sampleParameters{},Model_f{},non_const<rand_e>{},data_ei{})
//                                                      }+calculate_mcmc_parallel_q));



//  auto next_inside_q=
//      vector_space(f_i{calculate_next_mcmc_fi{},
//                       quimulun{
//                           Fr(Parameters_ei{},
//                               [](auto &mt_a, auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& nwalkers_a, auto const& z_a)
//                               {
//                                 auto const& parameter_a=mcmc_a[Parameters_ei{}]();
//                                 std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
//                                 auto p2=pos_a.value();
//                                 p2[walker_ei{}]()= u(mt_a.value());
//                                 auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
//                                 return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
//                               },
//                               non_const<rand_e>{},Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Size<walker_ei>{}, z_v{})
//                       }+calculate_mcmc_parallel_q},
//                   f_i(calculate_step_fi{},
//                       quimulun{
//                           Fr(z_v{},
//                               []( auto& mt_a, auto const& alfa)
//                               {
//                                 return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
//                               }
//                               ,non_const<rand_e>{},stretch_alfa{}),
//                           Frq{Candidate_mcmc{},
//                               Calculator_random{},
//                               calculate_next_mcmc_fi{},
//                               non_const<pass_id<rand_e>>{},
//                               pass_id<Pos<beta_ei,walker_ei>>{},
//                               pass_id<Size<walker_ei>>{},
//                               pass_id<all<global_Current_mcmc>>{},
//                               pass_id<Current_mcmc>{},
//                               pass_id<Model_f>{},
//                               pass_id<data_ei>{},
//                               pass_id<beta_ei>{}, pass_id<z_v>{}},
//                           F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

//                                 auto const& current_logP_a=current_a[logP_k{}]();
//                                 auto const& candidate_logP_a=candidate_a[logP_k{}]();
//                                 auto const& Parameters_a=current_a[Parameters_ei{}]();

//                                 // TODO: implement logpr as a value so difference between logp are properly assesed
//                                 return  (candidate_logP_a-current_logP_a )  +
//                                        log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

//                               }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
//                           Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
//                                 double A = std::exp(logA_a.value());
//                                 double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//                                 return  v(int(r < A),dimension_less{});
//                                 //       return  int(r < -1.0);

//                               }, non_const<rand_e>{}, logAccept_k{})} )
//                   );


//  auto next_parallel_q=quimulun{
//      View(global_Current_mcmc{},Current_mcmc{}),
//      Frq(pos<Step_mcmc>{}
//          ,Calculator_random{},
//          calculate_step_fi{},
//          non_const<pass_id<rand_e>>{},
//          Pos<beta_ei,walker_ei>{},
//          pass_id<stretch_alfa>{},
//          pass_id<calculate_step_fi>{},
//          pass_id<calculate_next_mcmc_fi>{} ,
//          pass_id<Size<walker_ei>>{},
//          pass_id<all<global_Current_mcmc>>{},
//          pass_id<Current_mcmc>{},
//          pass_id<Model_f>{},
//          pass_id<data_ei>{},
//          pass_id<beta_ei>{})

//  };

//  auto accept_candidate_or_current_parallel=quimulun{
//      F(Next_mcmc{},
//          [](auto&& current, auto&& step){
//            if (step[accept_k{}]().value())
//              return
//                  std::move(step)[Candidate_mcmc{}]();
//            else
//              return std::move(current);}
//          ,Current_mcmc{},Step_mcmc{})
//  };

//  auto temperature_jump_parallel=quimulun{
//      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
//            using pos_type=std::decay_t<decltype (beta_a.begin())>;
//            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
//            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<Start<walker_ei>,nwalkers_type>,
//                                           x_i<End<walker_ei>,nwalkers_type>>;


//            using out_type=vector_field<vec<Transition<beta_ei>>,
//                                          elem_type>;
//            out_type out;
//            auto pos=out.begin();
//            auto p=beta_a.begin();

//            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
//              beta_a.next(p);
//            auto p2=p;

//            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


//            while(beta_a.next(p2))

//            {
//              auto start_w=v(u(mt_a(p).value()),dimension_less{});
//              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
//              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
//              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

//              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
//              ++pos[Transition<beta_ei>{}]();
//              if (!beta_a.next(p2)) break;
//              p=p2;
//            }
//            return out;
//          },
//          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
//      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
//          {
//            auto p=beta_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto start_beta_a=beta_a(p);
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto end_beta_a=beta_a(p);

//            return  -(start_beta_a - end_beta_a) *
//                   (start_logLik_a -end_logLik_a);
//          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
//      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
//            double A = std::exp(logA_a.value());
//            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//            return  v<int,dimension_less>(int(r < A));

//          }, non_const<Start<rand_e>>{}, logAccept_k{})
//  };

//  auto actualize_jump_parallel=quimulun{
//      F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
//          auto p=trans_beta_a.begin();
//          auto pos=next_mcmc_a.begin();
//          do
//          {
//            if ( accept_k(p).value())
//            {
//              auto start_pos=pos;
//              auto end_pos=pos;
//              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
//              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

//              auto par_t=std::move(next_mcmc_a(start_pos));
//              next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
//              next_mcmc_a(end_pos)=std::move(par_t);
//              next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
//              next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
//            }

//          }while(trans_beta_a.next(p));

//          return std::move(next_mcmc_a);


//        }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
//  };


//  auto compute_Evidence_parallel=quimulun{
//      F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
//            auto out=beta.template create<value_type>();
//            auto pos=out.begin();
//            auto n=beta.create();
//            auto p=mcmc_a.begin();
//            do{
//              out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
//              n(p)=n(p)+v(1.0);
//            }while(mcmc_a.next(p));

//            do{
//              out(pos)=out(pos)*(v(1)/n(pos));
//            }while(out.next(pos));

//            return out;
//          },Current_mcmc{}, beta_ei{}),
//      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
//            value_type out;
//            auto p=beta.begin();
//            auto beta0=beta(p);
//            auto db_sum=beta0-beta0;
//            if (beta0.value()<0.5)
//            {
//              beta0.value()=0;
//              auto beta1=beta(p);
//              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
//              auto db=(beta1-beta0)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              db_sum=db_sum+db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta2-beta0)/v(2.0);
//                out=out+logL0*db;
//                db_sum=db_sum+db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
//              out=out+logL0*db;
//              db_sum=db_sum+db;
//              return out;

//            }
//            else
//            {
//              beta0.value()=1.0;
//              auto beta1=beta(p);
//              auto db=(beta0-beta1)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta0-beta2)/v(2.0);
//                out=out+logL0*db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2-beta1)*v(1.0/2.0);
//              out=out+logL0*db;
//              return out;
//            }

//          }, Average<Current<logLik_ei>>{},beta_ei{} )

//  };





//  std::string f_jump_name_parallel=filename+"_jump_parallel.txt";

//  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());

//  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();



//      auto startIt=
//      quimulun{
//    Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{}),

//   Sampler(
//   rand_e{},
//   [](auto const& seed_a){return v(std::mt19937_64(seed_a),dimension_less{});},
//   Uniform_Integer_Distribution<typename std::mt19937_64::result_type>{},
//   [](auto& mt_a,auto const &, auto const &){return mt_a;},non_const<Start<rand_e>>{},beta_ei{},walker_ei{}
//   ),

//   Frq(
//   Current_mcmc{},
//   Calculator_random{},calculate_initial_mcmc_fi{},non_const<pass_id<rand_e>>{},pass_id<Model_f>{},pass_id<data_ei>{},pass_id<beta_ei>{})
//   };


//      auto stepIt=quimulun{
//      View(global_Current_mcmc{},Current_mcmc{}),
//      Frq(pos<Step_mcmc>{}
//          ,Calculator_random{},
//          calculate_step_fi{},
//          non_const<pass_id<rand_e>>{},
//          Pos<beta_ei,walker_ei>{},
//          pass_id<stretch_alfa>{},
//          pass_id<calculate_step_fi>{},
//          pass_id<calculate_next_mcmc_fi>{} ,
//          pass_id<Size<walker_ei>>{},
//          pass_id<all<global_Current_mcmc>>{},
//          pass_id<Current_mcmc>{},
//          pass_id<Model_f>{},
//          pass_id<data_ei>{},
//          pass_id<beta_ei>{})

//,      F(Next_mcmc{},
//          [](auto&& current, auto&& step){
//            if (step[accept_k{}]().value())
//              return
//                  std::move(step)[Candidate_mcmc{}]();
//            else
//              return std::move(current);}
//          ,Current_mcmc{},Step_mcmc{})
// ,     Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
//            using pos_type=std::decay_t<decltype (beta_a.begin())>;
//            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
//            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<Start<walker_ei>,nwalkers_type>,
//                                           x_i<End<walker_ei>,nwalkers_type>>;


//            using out_type=vector_field<vec<Transition<beta_ei>>,
//                                          elem_type>;
//            out_type out;
//            auto pos=out.begin();
//            auto p=beta_a.begin();

//            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
//              beta_a.next(p);
//            auto p2=p;

//            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


//            while(beta_a.next(p2))

//            {
//              auto start_w=v(u(mt_a(p).value()),dimension_less{});
//              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
//              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
//              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

//              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
//              ++pos[Transition<beta_ei>{}]();
//              if (!beta_a.next(p2)) break;
//              p=p2;
//            }
//            return out;
//          },
//          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
//      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
//          {
//            auto p=beta_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto start_beta_a=beta_a(p);
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto end_beta_a=beta_a(p);

//            return  -(start_beta_a - end_beta_a) *
//                   (start_logLik_a -end_logLik_a);
//          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
//      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
//            double A = std::exp(logA_a.value());
//            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//            return  v<int,dimension_less>(int(r < A));

//          }, non_const<Start<rand_e>>{}, logAccept_k{})
// ,    F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
//          auto p=trans_beta_a.begin();
//          auto pos=next_mcmc_a.begin();
//          do
//          {
//            if ( accept_k(p).value())
//            {
//              auto start_pos=pos;
//              auto end_pos=pos;
//              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
//              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

//              auto par_t=std::move(next_mcmc_a(start_pos));
//              next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
//              next_mcmc_a(end_pos)=std::move(par_t);
//              next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
//              next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
//            }

//          }while(trans_beta_a.next(p));

//          return std::move(next_mcmc_a);


//        }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
// ,     F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
//            auto out=beta.template create<value_type>();
//            auto pos=out.begin();
//            auto n=beta.create();
//            auto p=mcmc_a.begin();
//            do{
//              out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
//              n(p)=n(p)+v(1.0);
//            }while(mcmc_a.next(p));

//            do{
//              out(pos)=out(pos)*(v(1)/n(pos));
//            }while(out.next(pos));

//            return out;
//          },Current_mcmc{}, beta_ei{}),
//      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
//            value_type out;
//            auto p=beta.begin();
//            auto beta0=beta(p);
//            auto db_sum=beta0-beta0;
//            if (beta0.value()<0.5)
//            {
//              beta0.value()=0;
//              auto beta1=beta(p);
//              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
//              auto db=(beta1-beta0)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              db_sum=db_sum+db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta2-beta0)/v(2.0);
//                out=out+logL0*db;
//                db_sum=db_sum+db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
//              out=out+logL0*db;
//              db_sum=db_sum+db;
//              return out;

//            }
//            else
//            {
//              beta0.value()=1.0;
//              auto beta1=beta(p);
//              auto db=(beta0-beta1)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta0-beta2)/v(2.0);
//                out=out+logL0*db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2-beta1)*v(1.0/2.0);
//              out=out+logL0*db;
//              return out;
//            }

//          }, Average<Current<logLik_ei>>{},beta_ei{} )

//  };




//  for (long int i=0; i<nsamples; ++i)
//  {



//    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_q,random_parallel_v,next_inside_q,currentParameters_parallel_q,modeldata);

//    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_q), std::move(next_candidate_parallel_v));

//    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


//    if (i==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
//      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

//    }
//    if (i%100000==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

//      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
//    }
//    currentParameters_parallel_q=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

//    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_q,modeldata);

//    auto tnow=std::chrono::steady_clock::now();
//    auto d=tnow-startTime;
//    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
//    if (i%100==0)
//      std::cerr<<"\n i_sample="<<i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

//    if (i==0)
//    {
//      //   to_DataFrame_title_index(fname,output);
//      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_q,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
//      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

//    }
//    // to_DataFrame_body_index(fname,i,decimate_factor,output);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_q,data,modeldata,initwalkers_v);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);
//  }

//  return 1;
//}



//template<class Model_q, class data_q /* ,class betas_v, class nwalkers_v*/>
//struct  parallel_emcee_parallel_parallel_for_non_template
//{

//  template<class Paramters>
//  static auto calculate_mcmc_parallel_f( const Model_q& model_fa, const data_q& data_a, Paramters const& parameters_a, double beta_a)
//  {
//    auto variables_a= calculate(model_fa,data_a,parameters_a);
//    auto logPrior_a= logPrior(model_fa,data_a,parameters_a,variables_a);
//    auto logLik_a=logLikelihood(model_fa,data_a,parameters_a,variables_a);
//    auto logP_a= logPrior_a+beta_a*logLik_a;
//    return std::tuple(variables_a,logPrior_a,logLik_a,logP_a);

//  }


//  auto calculate_initial_mcmc_fi(v<std::mt19937_64,dimension_less>& mt,const Model_q& model_fa, const data_q& data_a )
//      {

//                return sampleParameters(model_fa,mt,data_a);
//     }

//     template<class initial_mcmc_f>
//     auto inside_init_Parameters_parallel(v<std::mt19937_64,dimension_less>& mt_a, initial_mcmc_f const& initial_mcmc_fa,Model_q const& model_fa,data_q const& data_a, double const& beta_a)
//         {
//          auto Current_mcmc=,[]
//             {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
//                           //     ,x_i(data_ei{},data_a));
//                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
//            return random_calculate(initial_mcmc_fa, mt,model_fa,data_a,);},
//             non_const<rand_e>{},calculate_initial_mcmc_fi{},Model_f{},data_ei{},beta_ei{})
//     };


//  auto operator ()(const Model_q& model,
//                  const data_q& data,
//                  const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas,
//                  const v<std::size_t,dimension_less>& nw,
//                  std::mt19937::result_type seed,
//                  long int nsamples,
//                  const std::vector<std::size_t>& decimate_factor,
//                  const std::string& filename)
//  {
//    std::mt19937_64 mt_a(seed);
//    std::vector<double> betaes(betas.value().size());
//    for (std::size_t i=0; i<betaes.size(); ++i) betaes[i]=betas.value()[i].value();

//    auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

//    double stretch_alfa=2.0;
//    std::vector<double> walkers(nw.value());
//    for (std::size_t i=0; i<nw.value(); ++i) walkers[i]=i;

//    std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
//    std::vector<std::vector<std::mt19937_64>> mts(betaes.size(),std::vector<std::mt19937_64>(walkers.size()));
//    for (auto& mt_i:mts) for(auto& mt:mt_i) mt=std::mt19937_64(useed(mt_a));




//    auto inside_init_Parameters_parallel=quimulun{
//        Fr(Current_mcmc{},[](auto& mt_a, auto const& initial_mcmc_fa,auto const& model_fa,auto const& data_a, auto const& beta_a)
//            {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
//                           //     ,x_i(data_ei{},data_a));
//                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
//            return random_calculate(initial_mcmc_fa, mt,var);},
//            non_const<rand_e>{},calculate_initial_mcmc_fi{},Model_f{},data_ei{},beta_ei{})
//    };

//    auto next_inside=
//        vector_space(f_i{calculate_next_mcmc_fi{},
//                         quimulun{
//                             Fr(Parameters_ei{},
//                                 [](auto &mt_a, auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& nwalkers_a, auto const& z_a)
//                                 {
//                                   auto const& parameter_a=mcmc_a[Parameters_ei{}]();
//                                   std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
//                                   auto p2=pos_a.value();
//                                   p2[walker_ei{}]()= u(mt_a.value());
//                                   auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
//                                   return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
//                                 },
//                                 non_const<rand_e>{},Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Size<walker_ei>{}, z_v{})
//                         }+calculate_mcmc_parallel_f},
//                     f_i(calculate_step_fi{},
//                         quimulun{
//                             Fr(z_v{},
//                                 []( auto& mt_a, auto const& alfa)
//                                 {
//                                   return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
//                                 }
//                                 ,non_const<rand_e>{},stretch_alfa{}),

//                             Fr{Candidate_mcmc{},[](auto& mt_a,
//                                                     auto const& pos,
//                                                     auto const& calc_next_fa,
//                                                     auto const& nwalkers_a,
//                                                     auto const& all_current_mcmc_a,
//                                                     auto const& current_mcmc_a ,
//                                                     auto const& model_fa,
//                                                     auto const& data_ea ,
//                                                     auto const& beta_a,
//                                                     auto const & z_a)
//                                {

//                                  auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//                                  auto var=vector_space(x_i(Pos<beta_ei,walker_ei>{},pos),
//                                                          x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
//                                                          x_i_view_const(Current_mcmc{},current_mcmc_a),
//                                                          x_i_view_const(Size<walker_ei>{},nwalkers_a),
//                                                          f_i_view_const(Model_f{},model_fa),
//                                                          x_i_view_const(data_ei{},data_ea),
//                                                          x_i_view_const(beta_ei{},beta_a),
//                                                          x_i_view_const(z_v{},z_a));
//                                  return random_calculate(calc_next_fa,mt,var);
//                                },non_const<rand_e>{},
//                                Pos<beta_ei,walker_ei>{},
//                                calculate_next_mcmc_fi{},
//                                Size<walker_ei>{},
//                                all<global_Current_mcmc>{},
//                                Current_mcmc{},
//                                Model_f{},
//                                data_ei{},
//                                beta_ei{}, z_v{}},
//                             F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

//                                   auto const& current_logP_a=current_a[logP_k{}]();
//                                   auto const& candidate_logP_a=candidate_a[logP_k{}]();
//                                   auto const& Parameters_a=current_a[Parameters_ei{}]();

//                                   // TODO: implement logpr as a value so difference between logp are properly assesed
//                                   return  (candidate_logP_a-current_logP_a )  +
//                                          log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

//                                 }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
//                             Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
//                                   double A = std::exp(logA_a.value());
//                                   double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//                                   return  v(int(r < A),dimension_less{});
//                                   //       return  int(r < -1.0);

//                                 }, non_const<rand_e>{}, logAccept_k{})} )
//                     );


//    auto next_parallel_f=quimulun{
//        Fr(pos<Step_mcmc>{},[](auto& mt_a,
//                                 auto const& pos_a,
//                                 auto const& alfa,
//                                 auto const& calc_step_fa,
//                                 auto const& calc_next_fa,
//                                 auto const& nwalkers_a,
//                                 auto const& all_current_mcmc_a,
//                                 auto const& current_mcmc_a ,
//                                 auto const& model_fa,
//                                 auto const& data_ea ,
//                                 auto const& beta_a){
//              auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//              auto var=vector_space(x_i_view_const(stretch_alfa{},alfa),
//                                      f_i_view_const(calculate_next_mcmc_fi{},calc_next_fa),
//                                      x_i(Pos<beta_ei,walker_ei>{},v(pos_a,dimension_less{})),
//                                      x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
//                                      x_i_view_const(Current_mcmc{},current_mcmc_a),
//                                      x_i_view_const(Size<walker_ei>{},nwalkers_a),
//                                      f_i_view_const(Model_f{},model_fa),
//                                      x_i_view_const(data_ei{},data_ea),
//                                      x_i_view_const(beta_ei{},beta_a));
//              return random_calculate(calc_step_fa,mt,var);

//            },
//            non_const<rand_e>{},
//            stretch_alfa{},
//            calculate_step_fi{},
//            calculate_next_mcmc_fi{} ,
//            Size<walker_ei>{},
//            all<Current_mcmc>{},
//            Current_mcmc{},
//            Model_f{},
//            data_ei{},
//            beta_ei{})
//    };


//    auto accept_candidate_or_current_parallel=quimulun{
//        F(Next_mcmc{},
//            [](auto&& current, auto&& step){
//              if (step[accept_k{}]().value())
//                return
//                    std::move(step)[Candidate_mcmc{}]();
//              else
//                return std::move(current);}
//            ,Current_mcmc{},Step_mcmc{})
//    };


//    auto temperature_jump_parallel=quimulun{
//        Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
//              using pos_type=std::decay_t<decltype (beta_a.begin())>;
//              using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
//              using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                             x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                             x_i<Start<walker_ei>,nwalkers_type>,
//                                             x_i<End<walker_ei>,nwalkers_type>>;


//              using out_type=vector_field<vec<Transition<beta_ei>>,
//                                            elem_type>;
//              out_type out;
//              auto pos=out.begin();
//              auto p=beta_a.begin();

//              if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
//                beta_a.next(p);
//              auto p2=p;

//              std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


//              while(beta_a.next(p2))

//              {
//                auto start_w=v(u(mt_a(p).value()),dimension_less{});
//                auto end_w=v(u(mt_a(p2).value()),dimension_less{});
//                auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
//                auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

//                insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
//                ++pos[Transition<beta_ei>{}]();
//                if (!beta_a.next(p2)) break;
//                p=p2;
//              }
//              return out;
//            },
//            non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
//        F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//            {
//              auto p=Next_mcmc_a.begin();
//              p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
//              return Next_mcmc_a(p)[logLik_ei{}]();

//            },Transition<beta_ei>{},all<Next_mcmc>{}),
//        F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//            {
//              auto p=Next_mcmc_a.begin();
//              p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
//              return Next_mcmc_a(p)[logLik_ei{}]();

//            },Transition<beta_ei>{},all<Next_mcmc>{}),
//        F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
//            {
//              auto p=beta_a.begin();
//              p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              auto start_beta_a=beta_a(p);
//              p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              auto end_beta_a=beta_a(p);

//              return  -(start_beta_a - end_beta_a) *
//                     (start_logLik_a -end_logLik_a);
//            },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
//        Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
//              double A = std::exp(logA_a.value());
//              double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//              return  v<int,dimension_less>(int(r < A));

//            }, non_const<Start<rand_e>>{}, logAccept_k{})




//    };


//    auto actualize_jump_parallel=quimulun{
//        F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
//            auto p=trans_beta_a.begin();
//            auto pos=next_mcmc_a.begin();
//            do
//            {
//              if ( accept_k(p).value())
//              {
//                auto start_pos=pos;
//                auto end_pos=pos;
//                start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//                start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
//                end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//                end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

//                auto par_t=std::move(next_mcmc_a(start_pos));
//                next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
//                next_mcmc_a(end_pos)=std::move(par_t);
//                next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
//                next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
//              }

//            }while(trans_beta_a.next(p));

//            return std::move(next_mcmc_a);


//          }, non_const<Next_mcmc>{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
//    };



//    auto currentParameters_parallel_v=random_calculate_parallel_for(
//        inside_init_Parameters_parallel,random_parallel_v,initParameters_q_parallel,modeldata,initwalkers_v);

//    auto compute_Evidence_parallel=quimulun{
//        F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
//            {
//              using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
//              auto out=beta.template create<value_type>();
//              auto pos=out.begin();
//              auto n=beta.create();
//              auto p=mcmc_a.begin();
//              do{
//                out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
//                n(p)=n(p)+v(1.0);
//              }while(mcmc_a.next(p));

//              do{
//                out(pos)=out(pos)*(v(1)/n(pos));
//              }while(out.next(pos));

//              return out;
//            },Current_mcmc{}, beta_ei{}),
//        F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
//            {
//              using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
//              value_type out;
//              auto p=beta.begin();
//              auto beta0=beta(p);
//              auto db_sum=beta0-beta0;
//              if (beta0.value()<0.5)
//              {
//                beta0.value()=0;
//                auto beta1=beta(p);
//                if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
//                auto db=(beta1-beta0)*v(1.0/2.0);
//                auto logL0=logLik(p);
//                out=logL0*db;
//                db_sum=db_sum+db;
//                while(beta.next(p))
//                {
//                  auto beta2=beta(p);
//                  auto db=(beta2-beta0)/v(2.0);
//                  out=out+logL0*db;
//                  db_sum=db_sum+db;
//                  beta0=beta1;
//                  beta1=beta2;
//                  logL0=logLik(p);
//                }
//                auto beta2=v(1.0,dimension_less{});
//                db=(beta2+beta1-2*beta0)*v(1.0/4.0);
//                out=out+logL0*db;
//                db_sum=db_sum+db;
//                return out;

//              }
//              else
//              {
//                beta0.value()=1.0;
//                auto beta1=beta(p);
//                auto db=(beta0-beta1)*v(1.0/2.0);
//                auto logL0=logLik(p);
//                out=logL0*db;
//                while(beta.next(p))
//                {
//                  auto beta2=beta(p);
//                  auto db=(beta0-beta2)/v(2.0);
//                  out=out+logL0*db;
//                  beta0=beta1;
//                  beta1=beta2;
//                  logL0=logLik(p);
//                }
//                auto beta2=v(1.0,dimension_less{});
//                db=(beta2-beta1)*v(1.0/2.0);
//                out=out+logL0*db;
//                return out;
//              }

//            }, Average<Current<logLik_ei>>{},beta_ei{} )

//    };


//    std::string f_jump_name_parallel=fname_parallel+"_jump_parallel.txt";

//    std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());

//    std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();


//    for (long int i=0; i<nsamples; ++i)
//    {

//      auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_f,random_parallel_v,next_inside,currentParameters_parallel_v,modeldata);

//      auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_v), std::move(next_candidate_parallel_v));

//      auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel_v,next_parameter_parallel_v,modeldata);


//      if (i==0)
//      {
//        auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
//        to_DataFrame_title(f_jump_parallel,output_jump_parallel);

//      }
//      if (i%1000==0)
//      {
//        auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

//        to_DataFrame_body(f_jump_parallel,output_jump_parallel);
//      }
//      currentParameters_parallel_v=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

//      auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_v,modeldata);

//      auto tnow=std::chrono::steady_clock::now();
//      auto d=tnow-startTime;
//      double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
//      if (i%100==0)
//        std::cerr<<"\n i_sample="<< i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

//      if (i==0)
//      {
//        //   to_DataFrame_title_index(fname,output);
//        to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_v,data,modeldata,initwalkers_v);
//        to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
//        to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
//        to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

//      }
//      // to_DataFrame_body_index(fname,i,decimate_factor,output);
//      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_v,data,modeldata,initwalkers_v);
//      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);



//    }

//    return 1;
//  }

//};




//template<class Model_q0, class data_q0,class Model_q1, class data_q1  /* ,class betas_v, class nwalkers_v*/>
//auto parallel_emcee_parallel_parallel_for(const Model_q0& model_0,
//                                          const data_q0& data_0,
//                                          const Model_q1& model_1,
//                                          const data_q1& data_1,
//                                          const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas_0,
//                                          const v<std::size_t,dimension_less>& nw_0,
//                                          const vector_field<vec<beta_ei>,v<double,dimension_less>>& betas_1,
//                                          const v<std::size_t,dimension_less>& nw_1,
//                                          std::mt19937::result_type seed,
//                                          long int nsamples,
//                                          const std::vector<std::size_t>& decimate_factor,
//                                          const std::string& filename)
//{
//  std::mt19937_64 mt0_parallel(seed);

//  auto fname_parallel=filename+"_"+ time_now() + "_" + std::to_string(seed)+"_parallel";

//  auto rand0_parallel=vector_space{x_i(Start<rand_e>{},v(std::move(mt0_parallel),dimension_less{}))};


//  auto calculate_mcmc_parallel=quimulun{
//      F(Variables_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a)
//          {
//            return calculate(model_fa,data_a,parameters_a);
//          },
//          Model_f{},data_ei{},Parameters_ei{}),
//      F(logPrior_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
//          {return logPrior(model_fa,data_a,parameters_a,variables_a);},
//          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
//      F(logLik_ei{},[](auto const& model_fa,auto const& data_a, auto const& parameters_a, auto const& variables_a)
//          {return logLikelihood(model_fa,data_a,parameters_a,variables_a);},
//          Model_f{},data_ei{},Parameters_ei{}, Variables_ei{}),
//      F(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;}, logPrior_ei{},logLik_ei{},beta_ei{})
//  };

//  auto modeldata_0=quimulun{
//      f_i(Model_f{},model_0),
//      x_i(stretch_alfa{},v(2.0,dimension_less{})),
//      x_i(data_ei{},data_0),
//      x_i(beta_ei{},betas_0),
//      x_i(Size<walker_ei>{},nw_0)
//  };

//  auto modeldata_1=quimulun{
//      f_i(Model_f{},model_1),
//      x_i(stretch_alfa{},v(2.0,dimension_less{})),
//      x_i(data_ei{},data_1),
//      x_i(beta_ei{},betas_1),
//      x_i(Size<walker_ei>{},nw_1)
//  };




//  auto initwalkers_f=quimulun{
//      Coord(walker_ei{},IndexCoordinate{},Size<walker_ei>{})
//  };

//  auto initwalkers_v_0=calculate(initwalkers_f,modeldata_0);
//  auto initwalkers_v_1=calculate(initwalkers_f,modeldata_1);

//  auto random_f=quimulun{
//      Fr(rand_e{},[](auto& mt_a,auto const &, auto const &){
//            std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
//            return v(std::mt19937_64(useed(mt_a.value())),dimension_less{});},non_const<Start<rand_e>>{},beta_ei{},walker_ei{})
//  };

//  /// NOT parallel_for
//  auto random_parallel_v_0=random_calculate(random_f,rand0_parallel,modeldata_0,initwalkers_v_0);

//  auto random_parallel_v_1=random_calculate(random_f,rand0_parallel,modeldata_1,initwalkers_v_1);


//  auto initParameters_q_parallel=vector_space(f_i(calculate_initial_mcmc_f{},
//                                                    quimulun{
//                                                        Fr(Parameters_ei{},[](auto& mt_a,auto const& model_fa,auto const& data_a)
//                                                            {return sampleParameters(model_fa,mt_a,data_a);},non_const<rand_e>{},Model_f{},data_ei{})
//                                                    }+calculate_mcmc_parallel));


//  auto inside_init_Parameters_parallel=quimulun{
//      Fr(Current_mcmc{},[](auto& mt_a, auto const& initial_mcmc_fa,auto const& model_fa,auto const& data_a, auto const& beta_a)
//          {auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//            auto var=vector_space(f_i_view_const(Model_f{},model_fa)
//                           //     ,x_i(data_ei{},data_a));
//                            ,x_i_view_const(data_ei{},data_a),x_i_view_const(beta_ei{},beta_a));
//            return random_calculate(initial_mcmc_fa, mt,var);},
//          non_const<rand_e>{},calculate_initial_mcmc_f{},Model_f{},data_ei{},beta_ei{})
//  };

//  auto next_inside=
//      vector_space(f_i{calculate_next_mcmc_f{},
//                       quimulun{
//                           Fr(Parameters_ei{},
//                               [](auto &mt_a, auto const& pos_a,auto const& global_curent_mcmc_a, auto const& mcmc_a,auto const& nwalkers_a, auto const& z_a)
//                               {
//                                 auto const& parameter_a=mcmc_a[Parameters_ei{}]();
//                                 std::uniform_int_distribution<std::size_t> u(0, nwalkers_a.value()-1);
//                                 auto p2=pos_a.value();
//                                 p2[walker_ei{}]()= u(mt_a.value());
//                                 auto chosen_parameter=global_curent_mcmc_a(p2)[Parameters_ei{}]();
//                                 return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
//                               },
//                               non_const<rand_e>{},Pos<beta_ei,walker_ei>{},all<global_Current_mcmc>{},Current_mcmc{},Size<walker_ei>{}, z_v{})
//                       }+calculate_mcmc_parallel},
//                   f_i(calculate_step_f{},
//                       quimulun{
//                           Fr(z_v{},
//                               []( auto& mt_a, auto const& alfa)
//                               {
//                                 return  v(stretch_move_Distribution{}.sample(alfa.value(),mt_a.value()),dimension_less{});
//                               }
//                               ,non_const<rand_e>{},stretch_alfa{}),

//                           Fr{Candidate_mcmc{},[](auto& mt_a,
//                                                   auto const& pos,
//                                                   auto const& calc_next_fa,
//                                                   auto const& nwalkers_a,
//                                                   auto const& all_current_mcmc_a,
//                                                   auto const& current_mcmc_a ,
//                                                   auto const& model_fa,
//                                                   auto const& data_ea ,
//                                                   auto const& beta_a,
//                                                   auto const & z_a)
//                              {

//                                auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//                                auto var=vector_space(x_i(Pos<beta_ei,walker_ei>{},pos),
//                                                        x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
//                                                        x_i_view_const(Current_mcmc{},current_mcmc_a),
//                                                        x_i_view_const(Size<walker_ei>{},nwalkers_a),
//                                                        f_i_view_const(Model_f{},model_fa),
//                                                        x_i_view_const(data_ei{},data_ea),
//                                                        x_i_view_const(beta_ei{},beta_a),
//                                                        x_i_view_const(z_v{},z_a));
//                                return random_calculate(calc_next_fa,mt,var);
//                              },non_const<rand_e>{},
//                              Pos<beta_ei,walker_ei>{},
//                              calculate_next_mcmc_f{},
//                              Size<walker_ei>{},
//                              all<global_Current_mcmc>{},
//                              Current_mcmc{},
//                              Model_f{},
//                              data_ei{},
//                              beta_ei{}, z_v{}},
//                           F(logAccept_k{},[](auto const& current_a,auto const& candidate_a, auto z_a){

//                                 auto const& current_logP_a=current_a[logP_k{}]();
//                                 auto const& candidate_logP_a=candidate_a[logP_k{}]();
//                                 auto const& Parameters_a=current_a[Parameters_ei{}]();

//                                 // TODO: implement logpr as a value so difference between logp are properly assesed
//                                 return  (candidate_logP_a-current_logP_a )  +
//                                        log(z_a) * v(double(Parameters_a.size() - 1),dimension_less{});

//                               }, Current_mcmc{}, Candidate_mcmc{},z_v{}),
//                           Fr(accept_k{},[](auto& mt_a,auto const& logA_a){
//                                 double A = std::exp(logA_a.value());
//                                 double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//                                 return  v(int(r < A),dimension_less{});
//                                 //       return  int(r < -1.0);

//                               }, non_const<rand_e>{}, logAccept_k{})} )
//                   );


//  auto next_parallel_f=quimulun{
//      Fr(pos<Step_mcmc>{},[](auto& mt_a,
//                               auto const& pos_a,
//                               auto const& alfa,
//                               auto const& calc_step_fa,
//                               auto const& calc_next_fa,
//                               auto const& nwalkers_a,
//                               auto const& all_current_mcmc_a,
//                               auto const& current_mcmc_a ,
//                               auto const& model_fa,
//                               auto const& data_ea ,
//                               auto const& beta_a){
//            auto mt=vector_space(x_i_view_non_const(rand_e{},mt_a));
//            auto var=vector_space(x_i_view_const(stretch_alfa{},alfa),
//                                    f_i_view_const(calculate_next_mcmc_f{},calc_next_fa),
//                                    x_i(Pos<beta_ei,walker_ei>{},v(pos_a,dimension_less{})),
//                                    x_i_view_const(global_Current_mcmc{},all_current_mcmc_a),
//                                    x_i_view_const(Current_mcmc{},current_mcmc_a),
//                                    x_i_view_const(Size<walker_ei>{},nwalkers_a),
//                                    f_i_view_const(Model_f{},model_fa),
//                                    x_i_view_const(data_ei{},data_ea),
//                                    x_i_view_const(beta_ei{},beta_a));
//            return random_calculate(calc_step_fa,mt,var);

//          },
//          non_const<rand_e>{},
//          stretch_alfa{},
//          calculate_step_f{},
//          calculate_next_mcmc_f{} ,
//          Size<walker_ei>{},
//          all<Current_mcmc>{},
//          Current_mcmc{},
//          Model_f{},
//          data_ei{},
//          beta_ei{})
//  };


//  auto accept_candidate_or_current_parallel=quimulun{
//      F(Next_mcmc{},
//          [](auto&& current, auto&& step){
//            if (step[accept_k{}]().value())
//              return
//                  std::move(step)[Candidate_mcmc{}]();
//            else
//              return std::move(current);}
//          ,Current_mcmc{},Step_mcmc{})
//  };


//  auto temperature_jump_parallel=quimulun{
//      Fr(all<Transition<beta_ei>>{},[](auto& mt_a,auto const & beta_a, auto const &num_walkers_a){
//            using pos_type=std::decay_t<decltype (beta_a.begin())>;
//            using nwalkers_type=std::decay_t<decltype (num_walkers_a)>;
//            using elem_type=vector_space<x_i<Start<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<End<Pos<beta_ei>>,v<pos_type,dimension_less>>,
//                                           x_i<Start<walker_ei>,nwalkers_type>,
//                                           x_i<End<walker_ei>,nwalkers_type>>;


//            using out_type=vector_field<vec<Transition<beta_ei>>,
//                                          elem_type>;
//            out_type out;
//            auto pos=out.begin();
//            auto p=beta_a.begin();

//            if(std::uniform_int_distribution<std::size_t>(0,1)(mt_a.value()))
//              beta_a.next(p);
//            auto p2=p;

//            std::uniform_int_distribution<std::size_t> u(0, num_walkers_a.value() - 1);


//            while(beta_a.next(p2))

//            {
//              auto start_w=v(u(mt_a(p).value()),dimension_less{});
//              auto end_w=v(u(mt_a(p2).value()),dimension_less{});
//              auto p_v=v<pos_type,dimension_less>(p,dimension_less{});
//              auto p2_v=v<pos_type,dimension_less>(p2,dimension_less{});

//              insert_at(out,pos,elem_type(Cs<>{},p_v,p2_v,start_w,end_w));
//              ++pos[Transition<beta_ei>{}]();
//              if (!beta_a.next(p2)) break;
//              p=p2;
//            }
//            return out;
//          },
//          non_const<Start<rand_e>>{},beta_ei{},Size<walker_ei>{}),
//      F(Start<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[Start<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(End<logLik_ei>{},[](auto const& trans_beta_a, auto const & Next_mcmc_a)
//          {
//            auto p=Next_mcmc_a.begin();
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            p[walker_ei{}]()=trans_beta_a[End<walker_ei>{}]().value();
//            return Next_mcmc_a(p)[logLik_ei{}]();

//          },Transition<beta_ei>{},all<Next_mcmc>{}),
//      F(logAccept_k{},[](auto const &trans_beta_a,auto const& beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
//          {
//            auto p=beta_a.begin();
//            p[beta_ei{}]()=trans_beta_a[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto start_beta_a=beta_a(p);
//            p[beta_ei{}]()=trans_beta_a[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//            auto end_beta_a=beta_a(p);

//            return  -(start_beta_a - end_beta_a) *
//                   (start_logLik_a -end_logLik_a);
//          },Transition<beta_ei>{},all<beta_ei>{},Start<logLik_ei>{},End<logLik_ei>{} ),
//      Fr(accept_k{},[](auto& mt_a,auto const& logA_a ){
//            double A = std::exp(logA_a.value());
//            double r = std::uniform_real_distribution<double>(0, 1)(mt_a.value());
//            return  v<int,dimension_less>(int(r < A));

//          }, non_const<Start<rand_e>>{}, logAccept_k{})




//  };


//  auto actualize_jump_parallel=quimulun{
//      F{all<Current_mcmc>{},[](auto&& next_mcmc_a, auto const& trans_beta_a,  auto const& accept_k, auto const& beta_a){
//          auto p=trans_beta_a.begin();
//          auto pos=next_mcmc_a.begin();
//          do
//          {
//            if ( accept_k(p).value())
//            {
//              auto start_pos=pos;
//              auto end_pos=pos;
//              start_pos[beta_ei{}]()=trans_beta_a(p)[Start<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              start_pos[walker_ei{}]()=trans_beta_a(p)[Start<walker_ei>{}]().value();
//              end_pos[beta_ei{}]()=trans_beta_a(p)[End<Pos<beta_ei>>{}]().value()[beta_ei{}]();
//              end_pos[walker_ei{}]()=trans_beta_a(p)[End<walker_ei>{}]().value();

//              auto par_t=std::move(next_mcmc_a(start_pos));
//              next_mcmc_a(start_pos)=std::move(next_mcmc_a(end_pos));
//              next_mcmc_a(end_pos)=std::move(par_t);
//              next_mcmc_a(start_pos)[non_const<logP_k>{}]()=next_mcmc_a(start_pos)[logPrior_ei{}]()()+next_mcmc_a(start_pos)[logLik_ei{}]()()*beta_a(start_pos);
//              next_mcmc_a(end_pos)[non_const<logP_k>{}]()=next_mcmc_a(end_pos)[logPrior_ei{}]()()+next_mcmc_a(end_pos)[logLik_ei{}]()()*beta_a(end_pos);
//            }

//          }while(trans_beta_a.next(p));

//          return std::move(next_mcmc_a);


//        }, Next_mcmc{}, Transition<beta_ei>{}, accept_k{}, beta_ei{}}
//  };



//  auto currentParameters_parallel_v=random_calculate_parallel_for(inside_init_Parameters_parallel,random_parallel_v,initParameters_q_parallel,modeldata,initwalkers_v);

//  auto compute_Evidence_parallel=quimulun{
//      F(all<Average<Current<logLik_ei>>>{},[](auto const& mcmc_a,auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (mcmc_a(mcmc_a.begin())[logLik_ei{}]())>;
//            auto out=beta.template create<value_type>();
//            auto pos=out.begin();
//            auto n=beta.create();
//            auto p=mcmc_a.begin();
//            do{
//              out(p)=out(p)+mcmc_a(p)[logLik_ei{}]();
//              n(p)=n(p)+v(1.0);
//            }while(mcmc_a.next(p));

//            do{
//              out(pos)=out(pos)*(v(1)/n(pos));
//            }while(out.next(pos));

//            return out;
//          },Current_mcmc{}, beta_ei{}),
//      F(all<Current<Evidence_ei>>{},[](auto const& logLik, auto const& beta)
//          {
//            using value_type=std::decay_t<decltype (logLik(logLik.begin()))>;
//            value_type out;
//            auto p=beta.begin();
//            auto beta0=beta(p);
//            auto db_sum=beta0-beta0;
//            if (beta0.value()<0.5)
//            {
//              beta0.value()=0;
//              auto beta1=beta(p);
//              if (beta1.value()==0) {beta.next(p); beta1=beta(p);}
//              auto db=(beta1-beta0)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              db_sum=db_sum+db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta2-beta0)/v(2.0);
//                out=out+logL0*db;
//                db_sum=db_sum+db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2+beta1-2*beta0)*v(1.0/4.0);
//              out=out+logL0*db;
//              db_sum=db_sum+db;
//              return out;

//            }
//            else
//            {
//              beta0.value()=1.0;
//              auto beta1=beta(p);
//              auto db=(beta0-beta1)*v(1.0/2.0);
//              auto logL0=logLik(p);
//              out=logL0*db;
//              while(beta.next(p))
//              {
//                auto beta2=beta(p);
//                auto db=(beta0-beta2)/v(2.0);
//                out=out+logL0*db;
//                beta0=beta1;
//                beta1=beta2;
//                logL0=logLik(p);
//              }
//              auto beta2=v(1.0,dimension_less{});
//              db=(beta2-beta1)*v(1.0/2.0);
//              out=out+logL0*db;
//              return out;
//            }

//          }, Average<Current<logLik_ei>>{},beta_ei{} )

//  };


//  std::string f_jump_name_parallel=filename+"_jump_parallel.txt";

//  std::ofstream f_jump_parallel(f_jump_name_parallel.c_str());

//  std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now();


//  for (long int i=0; i<nsamples; ++i)
//  {

//    auto next_candidate_parallel_v=random_calculate_parallel_for(next_parallel_f,random_parallel_v,next_inside,currentParameters_parallel_v,modeldata);

//    //auto output_candidate_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+next_candidate_parallel_v+currentParameters_parallel_v+data;

//    auto next_parameter_parallel_v=transfer(accept_candidate_or_current_parallel,std::move(currentParameters_parallel_v), std::move(next_candidate_parallel_v));

//    //auto output_next_parallel=output_candidate_parallel+next_parameter_parallel_v;




//    auto after_temperature_jump_parallel=random_calculate_parallel_for(temperature_jump_parallel,rand0_parallel,next_parameter_parallel_v,modeldata);


//    if (i==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});
//      to_DataFrame_title(f_jump_parallel,output_jump_parallel);

//    }
//    if (i%100000==0)
//    {
//      auto output_jump_parallel=vector_space(x_i{isample{},v(i,dimension_less{})})+(after_temperature_jump_parallel|myremove<accept_k>{});

//      to_DataFrame_body(f_jump_parallel,output_jump_parallel);
//    }
//    currentParameters_parallel_v=transfer(actualize_jump_parallel,std::move(next_parameter_parallel_v),after_temperature_jump_parallel,modeldata);

//    auto Evidence_parallel_v=calculate_parallel_for(compute_Evidence_parallel,currentParameters_parallel_v,modeldata);


//    //  auto output_parallel=vector_space(x_i{isample{},v(i,dimension_less{})},x_i(beta_ei{},betas),initwalkers_v[walker_ei{}])+currentParameters_parallel_v+data+Evidence_parallel_v;


//    auto tnow=std::chrono::steady_clock::now();
//    auto d=tnow-startTime;
//    double time=1.0e-6*std::chrono::duration_cast<std::chrono::microseconds>(d).count()/60.0;
//    if (i%100==0)
//      std::cerr<<"\n i_sample="<<i<<"\t "<<"time="<<time<<"\t "<<(Evidence_parallel_v|myselect<Current<Evidence_ei>>{})<<"\n";

//    if (i==0)
//    {
//      //   to_DataFrame_title_index(fname,output);
//      to_DataFrame_title_index_new(fname_parallel,currentParameters_parallel_v,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,Evidence_parallel_v,data,modeldata,initwalkers_v);
//      to_DataFrame_title_index_new(fname_parallel,data,modeldata,initwalkers_v);
//      to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,data,modeldata);

//    }
//    // to_DataFrame_body_index(fname,i,decimate_factor,output);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,currentParameters_parallel_v,data,modeldata,initwalkers_v);
//    to_DataFrame_body_index_new(fname_parallel,i,time,decimate_factor,Evidence_parallel_v,modeldata);



//  }

//  return 1;
//}





#endif // QM_METROPOLIS_PARALLEL_H
