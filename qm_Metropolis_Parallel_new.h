#ifndef QM_METROPOLIS_PARALLEL_NEW_H
#define QM_METROPOLIS_PARALLEL_NEW_H
#include "qm_Metropolis_Parallel.h"
template<class Id> struct Data{constexpr static auto className=my_static_string("Data_")+Id::className;};
template<class Id> struct Fn{constexpr static auto className=my_static_string("Fn_")+Id::className;};

    template<class Id> struct Lik{constexpr static auto className=my_static_string("Lik_")+Id::className;};
template<class Id> struct Ensemble{constexpr static auto className=Id::className+my_static_string("_Ensemble");};
template<class Id> struct Parallel{constexpr static auto className=Id::className+my_static_string("_Parallel");};

    template<class Id> struct Prepare{constexpr static auto className=my_static_string("Prepare_")+Id::className;};
template<class Id> struct Execute{constexpr static auto className=my_static_string("Prepare_")+Id::className;};
template<class Id> struct Test{constexpr static auto className=my_static_string("Test_")+Id::className;};


    struct x_ei{constexpr static auto className=my_static_string("x");};
struct y_ei{constexpr static auto className=my_static_string("y");};

    struct stretch_move{constexpr static auto className=my_static_string("stretch_move");};
struct proposed_distribution{constexpr static auto className=my_static_string("proposed_distribution");};



    struct lik_q
{
  template<typename Model_f,typename data_ei>
  auto operator()(Arguments<Model_f, data_ei>&&)
  {
    return quimulun{
        Dq_new(Parameters_ei{},SampleParameters{},Model_f{},Arguments<data_ei>{}),
        Fq_new(Variables_ei{},Calculator_new{},Model_f{},Arguments<data_ei,Parameters_ei>{}),
        Fq_new(logPrior_ei{},logPrior_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
        Fq_new(logLik_ei{},logLikelihood_new{},Model_f{},Arguments<data_ei,Parameters_ei,Variables_ei>{}),
        };
  }
};

    struct ensemble_q
{
  template<typename Model_f,typename data_ei,typename walker_ei>
  auto operator()(Arguments<Model_f, data_ei,walker_ei>&&)const
  {
    return quimulun{
        Dq_new(Ensemble<x_ei>{},SampleParameters{},Model_f{},Arguments<data_ei>{},Index_struct<walker_ei>{}),
        Fq_new(Ensemble<y_ei>{},Calculator_new{},Model_f{},Arguments<Ensemble<x_ei>,data_ei>{}),
        };
  }
};





struct parallel_model_q
{
  template<typename Model_f,typename data_ei, typename beta_ei>
  auto operator()(Arguments<Model_f, data_ei, beta_ei>&&)
  {
    return quimulun{
        Dq_new(Parallel<x_ei>{},SampleParameters{},Model_f{},Arguments<data_ei>{},Index_struct<beta_ei>{}),
        Fq_new(Parallel<y_ei>{},Calculator_new{},Model_f{},Arguments<data_ei,Parallel<x_ei>>{}),
        F_new(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;},
            Arguments< sub<Parallel<y_ei>,logPrior_ei>,sub<Parallel<y_ei>,logLik_ei>,beta_ei>{})
    };
  }
};

struct parallel_ensamble_model_q
{
  template<typename Model_f,typename data_ei, typename beta_ei, typename walker_ei>
  auto operator()(Arguments<Model_f, data_ei, beta_ei,walker_ei>&&)
  {
    return quimulun{
        Dq_new(x_ei{},SampleParameters{},Model_f{},Arguments<data_ei>{},Index_struct<walker_ei,beta_ei>{}),
        Fq_new(y_ei{},Calculator_new{},Model_f{},Arguments<data_ei,x_ei>{}),
        F_new(logP_k{},[](auto const& logPrior_a, auto const& logLik_a, auto beta_a){return logPrior_a+beta_a*logLik_a;},
            Arguments< sub<y_ei,logPrior_ei>,sub<y_ei,logLik_ei>,beta_ei>{})
    };
  }
};





    struct parallel_stretch_move_q {

      template<class x_ei,class walker_ei,class stretch_alfa>
      auto prepare(Arguments<x_ei, walker_ei,stretch_alfa>&&)const
  {
    return quimulun{
        D(Chosen<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<Size_at_Index_new<x_ei,walker_ei>>{}, Index_struct<x_ei>{}),
        F_new(Index_of<Chosen<walker_ei>>{},
            [](auto pos, auto const& walker_a, auto const&  size_walker_a){
              pos[walker_ei{}]()=(pos[walker_ei{}]()+walker_a.value()) % size_walker_a.value();
              return pos;},
            Arguments<pos_new<x_ei>,Chosen<walker_ei>,Size_at_Index_new<walker_ei,walker_ei>>{}),
        D(z_v{},stretch_move_Distribution<double>{},Arguments<stretch_alfa>{},Index_struct<x_ei>{}),
        };
  }
  template<class x_ei,class walker_ei,class stretch_alfa>
  auto sample(Arguments<x_ei, walker_ei,stretch_alfa>&&)const
  {
    return quimulun{
        F_new(Out<x_ei>{},
            [](auto const& parameter_a,auto const& chosen_parameter, auto const& z_a)
            {
              return (parameter_a - chosen_parameter) * z_a + chosen_parameter;
            },Arguments<x_ei,applyPosition<x_ei,Index_of<Chosen<walker_ei>>>,z_v>{})
    };
  }

  template<class x_ei,class y_ei>
  auto test(Arguments_xi<x_ei, y_ei>&&){
    return quimulun(
        F_new(logAccept_k{},[](auto const& current_logP_a,auto const& candidate_logP_a, auto size_param, auto z_a){

              return  (candidate_logP_a-current_logP_a )  +
                     log(z_a) * v(size_param - 1.0,dimension_less{});

            }, Arguments<sub<Candidate<y_ei>,logP_k>, sub<Current<y_ei>,logP_k>,Size<sub<Current<x_ei>,Parameters_ei>>,z_v>{}),

        D(accept_k{},Bernoulli_Distribution{},Arguments<logAccept_k>{}));}





};

struct Proposal{ constexpr static auto  className=my_static_string("Proposal");};

struct Proposed_preparation{ constexpr static auto  className=my_static_string("Proposed_preparation");};


struct Metropolis_step_outcome{constexpr static auto  className=my_static_string("Metropolis_step_outcome");};


struct Metropolis_Hastings_ensemble_q
{

  template< class model_fi>
  auto start(Arguments<model_fi>&&)const
  {
    return quimulun{
        Dq_new(Current<x_ei>{},SampleParameters{},Fn<model_fi>{},Arguments<Data<model_fi>>{}),
        Fq_new(Current<y_ei>{},Calculator_new{},Fn<model_fi>{},Arguments<Current<x_ei>,Data<model_fi>>{})

    };

  }

  template< class model,class proposed_distribution>
  auto step(Arguments<model, proposed_distribution,x_ei,y_ei>&&)const
  {
    return quimulun{
        Dq_new(Proposed_preparation{},Sample{},Fn<Prepare<proposed_distribution>>{},Arguments<Current<x_ei>,Data<proposed_distribution>>{}),
        Fq_new(Candidate<x_ei>{},Calculator_new{},Fn<Execute<proposed_distribution>>{},Arguments<Proposed_preparation,Current<x_ei>,Data<proposed_distribution>>{}),

        Fq_new(Candidate<y_ei>{},Calculator_new{},Fn<model>{},Arguments<Candidate<x_ei>,Data<model>>{}),
        Dq_new(Metropolis_step_outcome{}, Sample{},Fn<Test<proposed_distribution>>{},
               Arguments<Proposed_preparation,Current<x_ei>,Current<y_ei>,Candidate<y_ei>>{}  ),
        Fq_new(Out<Current<x_ei>>{},Calculator_new{},
               quimulun{
                   F_new( x_ei{}, Conditional_choice{},
                         Arguments<sub<Metropolis_step_outcome,accept_k>,sub<non_const<Current<x_ei>>,non_const<x_ei>>,sub<non_const<Candidate<x_ei>>,non_const<x_ei>>>{})
               },Arguments_xi<Metropolis_step_outcome,non_const<Current<x_ei>>,non_const<Candidate<x_ei>>>{}),
        F_new(Out<Current<y_ei>>{},Conditional_choice{},
              Arguments<sub<Metropolis_step_outcome, accept_k>,non_const<Current<y_ei>>,non_const<Candidate<y_ei>>>{})
    };

  }

  template<class nstep, class nsteps_max>
  auto end_criteria(Arguments<nstep,nsteps_max>)
  {
    return quimulun{
        F_new(accept_k{}, [](auto nstep_a, auto n_max_nstep_a){return nstep_a<n_max_nstep_a;}, Arguments<nstep, nsteps_max>{})
    };
  }
};




struct even_or_odd{};


struct temperature_jump_q {

  template<class beta_ei, class walkers_ei, class y_ei>
  auto sample_and_test(Arguments<beta_ei, walkers_ei, y_ei>&&)const
  {
    return quimulun
        {
            D(even_or_odd{}, Bernoulli_Distribution{}, Arguments<>{}),
            Coord_new(index_new<Transition<beta_ei>>{},FoG{IndexCoordinate{}, [](auto size_beta_a, auto even_or_odd_a){
                                                              if (even_or_odd_a.value())
                                                                return (size_beta_a-v(1ul,dimension_less{}))/v(2ul,dimension_less{});
                                                              else return size_beta_a/v(2ul,dimension_less{});}},
                      Arguments<Size_at_Index_new<beta_ei,beta_ei>,even_or_odd>{}),
            F_new(From<Size<walkers_ei>>{},
                [](auto index_transition, auto const& walker_size_a, auto even_or_odd_a){
                  auto pos=Position<beta_ei>{};
                  pos[beta_ei{}]()=index_transition.value()*2;
                  if (even_or_odd_a.value()) ++pos[beta_ei{}]();
                  return walker_size_a(pos);
                },
                Arguments<index_new<Transition<beta_ei>>,Size_at_Index_new<walkers_ei,walkers_ei>,even_or_odd>{}),
            F_new(To<Size<walkers_ei>>{},
                [](auto index_transition, auto const& walker_size_a, auto even_or_odd_a){
                  auto pos=Position<beta_ei>{};
                  pos[beta_ei{}]()=index_transition.value()*2+1;
                  if (even_or_odd_a.value()) ++pos[beta_ei{}]();
                  return walker_size_a(pos);
                },
                Arguments<index_new<Transition<beta_ei>>,Size_at_Index_new<walkers_ei,walkers_ei>,even_or_odd>{}),
            D(From<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<From<Size<walkers_ei>>>{}),
            D(To<walker_ei>{},Uniform_Index_Distribution<std::size_t>{},Arguments<To<Size<walkers_ei>>>{}),
            F_new(pos<From<walker_ei>>{},[](auto const &index_a,auto const& From_walker, auto even_or_odd_a){
                  auto pos=Position<beta_ei,walker_ei>{};
                  auto  i0=even_or_odd_a.value()?1ul:0ul;
                  pos[beta_ei{}]()=index_a.value()*2+i0;
                  pos[walker_ei{}]()=From_walker.value();
                  return pos;
                },Arguments<index_new<Transition<beta_ei>>,From<walker_ei>,even_or_odd>{}),
            F_new(pos<To<walker_ei>>{},[](auto const &index_a,auto const& To_walker,  auto even_or_odd_a){
                  auto pos=Position<beta_ei,walker_ei>{};
                  auto  i0=even_or_odd_a.value()?1ul:0ul;
                  pos[beta_ei{}]()=index_a.value()*2+i0+1;
                  pos[walker_ei{}]()=To_walker.value();
                  return pos;
                },Arguments<index_new<Transition<beta_ei>>,To<walker_ei>,even_or_odd>{}),
            F_new(Out<logAccept_k>{},[](auto const &start_beta_a,auto const& end_beta_a, auto const& start_logLik_a, auto const & end_logLik_a)
                {
               //   using test=typename decltype (start_beta_a)::test;
                      return  -(start_beta_a - end_beta_a) *
                             (start_logLik_a -end_logLik_a);
                  },      Arguments<
                    applyPosition<beta_ei,pos<From<walker_ei>>>,
                    applyPosition<beta_ei,pos<To<walker_ei>>>,
                    applyPosition<sub<y_ei,logLik_ei>,pos<From<walker_ei>>>,
                    applyPosition<sub<y_ei,logLik_ei>,pos<To<walker_ei>>>>{}),
            D(Out<accept_k>{},Bernoulli_Distribution{},Arguments<Out<logAccept_k>>{}),
            };
  }


  template< class walker_ei,class x_ei, class y_ei>
  auto execute(Arguments<walker_ei,x_ei, y_ei>&&)const
  {
    return quimulun
        {
            Update_new(Out<x_ei>{},[](auto&& parameters_start, auto && parameters_end, auto const& accept_a){
                  if ( accept_a.value())
                  {
                    auto par_t=std::move(parameters_start);
                    parameters_start=std::move(parameters_end);
                    parameters_end=std::move(par_t);
                  }

                }, Arguments<non_const<x_ei>>{},Operations<applyPosition<non_const<x_ei>,pos<To<walker_ei>>>,
                                                         applyPosition<non_const<x_ei>,pos<From<walker_ei>>>>{},
                Arguments< accept_k>{}),
            Update_new(Out<y_ei>{},[](auto&& mcmc_end_pos, auto && mcmc_start_pos,
                                auto&& mcmc_end_logP, auto && mcmc_start_logP,
                                auto const& mcmc_end_logPrior, auto const& mcmc_start_logPrior,
                                auto const& mcmc_end_logLik, auto const& mcmc_start_logLik,
                                auto const& accept_a, auto const& beta_end,auto const& beta_start){
                  if ( accept_a.value())
                  {
                    auto par_t=std::move(mcmc_start_pos);
                    mcmc_start_pos=std::move(mcmc_end_pos);
                    mcmc_end_pos=std::move(par_t);
                    mcmc_end_logP=mcmc_end_logPrior+mcmc_end_logLik*beta_end;
                    mcmc_start_logP=mcmc_start_logPrior+mcmc_start_logLik*beta_start;
                  }
                }, Arguments<non_const<y_ei>>{},
                Operations< applyPosition<non_const<y_ei>,pos<To<walker_ei>>>,
                           applyPosition<non_const<y_ei>,pos<From<walker_ei>>>,
                           sub<applyPosition<non_const<y_ei>,pos<To<walker_ei>>>,non_const<logP_k>>,
                           sub<applyPosition<non_const<y_ei>,pos<From<walker_ei>>>,non_const<logP_k>>,
                           sub<applyPosition<y_ei,pos<To<walker_ei>>>,logPrior_ei>,
                           sub<applyPosition<y_ei,pos<From<walker_ei>>>,logPrior_ei>,
                           sub<applyPosition<y_ei,pos<To<walker_ei>>>,logLik_ei>,
                           sub<applyPosition<y_ei,pos<From<walker_ei>>>,logLik_ei>
                           >{},

                Arguments<accept_k,
                          applyPosition<beta_ei,pos<To<walker_ei>>>,
                          applyPosition<beta_ei,pos<From<walker_ei>>>>{})
        };
  }

};

struct temperature_jump_exec_q {
  struct even_or_odd{};


};

struct nstep{};
struct nstep_max{};



struct pick_Parameter_f{};

struct Metropolis_Hastings_emcee_q
{

  template< class model_fi>
  auto start(Arguments<model_fi>&&)const
  {
    return quimulun{
        Dq_new(Current<x_ei>{},SampleParameters{},Fn<model_fi>{},Arguments<Data<model_fi>>{}),
        Fq_new(Current<y_ei>{},Calculator_new{},Fn<model_fi>{},Arguments<Current<x_ei>,Data<model_fi>>{})

    };

  }




      template< class model,class proposed_distribution>
      auto step(Arguments<model, proposed_distribution,x_ei,y_ei>&&)const
  {
    return quimulun{
        Dq_new(Proposed_preparation{},Sample{},Fn<Prepare<proposed_distribution>>{},Arguments<Current<x_ei>,Data<proposed_distribution>>{}),
        Fq_new(Candidate<x_ei>{},Calculator_new{},Fn<Execute<proposed_distribution>>{},Arguments<Proposed_preparation,Current<x_ei>,Data<proposed_distribution>>{}),

        Fq_new(Candidate<y_ei>{},Calculator_new{},Fn<model>{},Arguments<Candidate<x_ei>,Data<model>>{}),
        Dq_new(Metropolis_step_outcome{}, Sample{},Fn<Test<proposed_distribution>>{},
               Arguments<Proposed_preparation>{},Arguments_xi<Current<x_ei>,Current<y_ei>,Candidate<y_ei>>{}  ),
        Fq_new(Out<Current<x_ei>>{},Calculator_new{},
                   quimulun{
                       F_new( x_ei{}, Conditional_choice{},
                             Arguments<sub<Metropolis_step_outcome,accept_k>,sub<non_const<Current<x_ei>>,non_const<x_ei>>,sub<non_const<Candidate<x_ei>>,non_const<x_ei>>>{})
                   },Arguments_xi<Metropolis_step_outcome,non_const<Current<x_ei>>,non_const<Candidate<x_ei>>>{}),
        Fq_new(Out<Current<y_ei>>{},Calculator_new{},
                   quimulun{
                       F_new( y_ei{}, Conditional_choice{},
                             Arguments<sub<Metropolis_step_outcome,accept_k>,sub<non_const<Current<y_ei>>,non_const<y_ei>>,sub<non_const<Candidate<y_ei>>,non_const<y_ei>>>{})
                   },Arguments_xi<Metropolis_step_outcome,non_const<Current<y_ei>>,non_const<Candidate<y_ei>>>{})

//        F_new(Out<Current<x_ei>>{},Conditional_choice{},
//              Arguments<sub<Metropolis_step_outcome, accept_k>,non_const<Current<x_ei>>,non_const<Candidate<x_ei>>>{}),
//        F_new(Out<Current<y_ei>>{},Conditional_choice{},
//              Arguments<sub<Metropolis_step_outcome, accept_k>,non_const<Current<y_ei>>,non_const<Candidate<y_ei>>>{}),


        };

  }

  template<class nstep, class nsteps_max>
  auto end_criteria(Arguments<nstep,nsteps_max>)
  {
    return quimulun{
        F_new(accept_k{}, [](auto nstep_a, auto n_max_nstep_a){return nstep_a<n_max_nstep_a;}, Arguments<nstep, nsteps_max>{})
    };
  }
};
struct Thermo_Jump{};

struct Parallel_Metropolis_Hastings_emcee_step_q
{

  template< class emcee_fi_start,class emcee_fi_step,class input_data_ei, class thermo_jump_result_fi, class thermo_jump_apply_result_fi>
  auto operator() (Arguments<emcee_fi_start, emcee_fi_step,input_data_ei,thermo_jump_result_fi, thermo_jump_apply_result_fi>&&)const
  {


    return quimulun
        {

            Dq_new(Start_new<Step_mcmc,nstep>{},Sample{},emcee_fi_start{},Arguments<input_data_ei>{}),
            Dq_new(Candidate_mcmc{},Sample{},emcee_fi_step{},Arguments<non_const<Step_mcmc>,input_data_ei>{}),
            Dq_new(Thermo_Jump{},Sample{},thermo_jump_result_fi{},Arguments<Candidate_mcmc,input_data_ei>{}),
            Fq_new(Next_new<Step_mcmc,nstep>{},Calculator_new{},thermo_jump_apply_result_fi{}, Arguments<Thermo_Jump, Candidate_mcmc,input_data_ei>{})


        };



  }
};

struct Parallel_Metropolis_Hastings_emcee_step_q_new
{

  template< class emcee_fi_start,class emcee_fi_step, class thermo_jump_result_fi, class thermo_jump_apply_result_fi, class emcee_fi_end>
  auto operator() (Arguments<emcee_fi_start, emcee_fi_step,thermo_jump_result_fi, thermo_jump_apply_result_fi,emcee_fi_end>&&)const
  {

    return quimulun
        {

            Dq_new(Start_new<Step_mcmc,nstep>{},Sample{},Fn<emcee_fi_start>{},Arguments<Data<emcee_fi_start>>{}),
            Dq_new(Candidate_mcmc{},Sample{},Fn<emcee_fi_step>{},Arguments<non_const<Step_mcmc>,Data<emcee_fi_step>>{}),
            Dq_new(Thermo_Jump{},Sample{},Fn<thermo_jump_result_fi>{},Arguments<Candidate_mcmc,Data<thermo_jump_result_fi>>{}),
            Fq_new(Next_new<Step_mcmc,nstep>{},Calculator_new{},Fn<thermo_jump_apply_result_fi>{}, Arguments<Thermo_Jump, Candidate_mcmc,Data<thermo_jump_result_fi>>{}),
            Fq_new(End_new<Step_mcmc,nstep>{},Calculator_new{},Fn<emcee_fi_end>{}, Arguments_xi<nstep>{},Arguments<Data<emcee_fi_end>>{})


        };



  }
};

struct ensemble_model{};

struct parallel_model{};
struct parallel_data{};
struct proposed_distribution_preparation_fi{};
struct proposed_distribution_execution_fi{};
struct proposed_distribution_metropolis_test_fi{};
struct distribution_data_ei{};;
struct emcee_fi_start{constexpr static auto className=my_static_string("emcee_fi_start");};
struct emcee_fi_step{constexpr static auto className=my_static_string("emcee_fi_step");};
struct emcee_fi_end{constexpr static auto className=my_static_string("emcee_fi_end");};
struct input_data_ei{};
struct thermo_jump_result_fi{constexpr static auto className=my_static_string("thermo_jump_result_fi");};
struct thermo_jump_apply_result_fi{constexpr static auto className=my_static_string("thermo_jump_apply_result_fi");};
struct Parallel_Metropolis_Hastings_emcee_q
{

  template< class Model_f,class data_ei, class beta_ei, class num_walkers_ei, class walker_ei, class distribution_data_ei>
  auto operator() (Arguments<num_walkers_ei,Model_f, data_ei,beta_ei, walker_ei,distribution_data_ei,nstep_max>&&)const
  {



    auto model_def=quimulun{
        Coord_new(walker_ei{},IndexCoordinate{},Arguments<num_walkers_ei>{}),
        f_i(Fn<Lik<Model_f>>{},lik_q{}(Arguments<Model_f, data_ei>{})),
        F_new(Data<Lik<Model_f>>{},Glue_new{},Arguments<Model_f,data_ei>{}),
        f_i(Fn<Parallel<Model_f>>{},parallel_ensamble_model_q{}(Arguments<Fn<Lik<Model_f>>,Data< Lik<Model_f>>, beta_ei,walker_ei>{})),
        F_new(Data<Parallel<Model_f>>{},Glue_new{},Arguments<Fn<Lik<Model_f>>,Data< Lik<Model_f>>, beta_ei,walker_ei>{}),
        f_i(Fn<Prepare<proposed_distribution>>{},parallel_stretch_move_q{}.prepare(Arguments<x_ei, walker_ei,stretch_alfa>{})),
        f_i(Fn<Execute<proposed_distribution>>{},parallel_stretch_move_q{}.sample(Arguments<x_ei, walker_ei,stretch_alfa>{})),
        f_i(Fn<Test<proposed_distribution>>{},parallel_stretch_move_q{}.test(Arguments_xi<x_ei, y_ei>{})),
        F_new(Data<proposed_distribution>{},Glue_new{},Arguments<stretch_alfa,walker_ei>{}),
        f_i(Fn<emcee_fi_start>{},Metropolis_Hastings_emcee_q{}.start(Arguments<Parallel<Model_f>>{})),
        F_new(Data<emcee_fi_start>{},Glue_new{},Arguments<Fn<Parallel<Model_f>>,Data<Parallel<Model_f>>>{}),
        f_i(Fn<emcee_fi_step>{},Metropolis_Hastings_emcee_q{}.step(
                                     Arguments<Parallel<Model_f>,proposed_distribution,x_ei,y_ei>{})),
        F_new(Data<emcee_fi_step>{},Glue_new{},
              Arguments<Fn<Prepare<proposed_distribution>>,Fn<Execute<proposed_distribution>>,Fn<Test<proposed_distribution>>,
                        Fn<Parallel<Model_f>>,Data<Parallel<Model_f>>,Data<proposed_distribution>>{}),
        f_i(Fn<emcee_fi_end>{},Metropolis_Hastings_emcee_q{}.end_criteria(Arguments<nstep,nstep_max>{})),
        F_new(Data<emcee_fi_end>{},Glue_new{},Arguments<nstep_max>{}),
        f_i(Fn<thermo_jump_result_fi>{},temperature_jump_q{}.sample_and_test(Arguments<beta_ei, walker_ei, Current<y_ei>>{})),
        f_i(Fn<thermo_jump_apply_result_fi>{},temperature_jump_q{}.execute(Arguments<  walker_ei, Current<x_ei>,Current<y_ei>>{})),
        F_new(Data<thermo_jump_result_fi>{},Glue_new{},Arguments<beta_ei, walker_ei>{})
    };




    //  using test=typename decltype (std::move(model_def)+
    //                                 Parallel_Metropolis_Hastings_emcee_step_q{}
    //                                 (Arguments<emcee_fi, input_data_ei,thermo_jump_result_fi, thermo_jump_apply_result_fi>{}))::q;

    return std::move(model_def)+
           Parallel_Metropolis_Hastings_emcee_step_q_new{}
           (Arguments<emcee_fi_start,emcee_fi_step,thermo_jump_result_fi, thermo_jump_apply_result_fi,emcee_fi_end>{});




  }
};



struct   num_walkers_ei{};


struct parallel_emcee_parallel_parallel_for_declarativa {


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
        x_i(num_walkers_ei{},nw),
        x_i(nstep_max{},v(nsamples,dimension_less{}))
    };

    auto mcee=Parallel_Metropolis_Hastings_emcee_q{}(Arguments<num_walkers_ei,Model_f, data_ei,beta_ei, walker_ei,distribution_data_ei,nstep_max>{});

    auto res=sample_loop_Operation(mcee,mt,modeldata);




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






#endif // QM_METROPOLIS_PARALLEL_NEW_H
