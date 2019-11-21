#ifndef QM_EVIDENCE_H
#define QM_EVIDENCE_H

#include "qm_vector_space.h"

#include <static_string.h>

#include <cmath>
#include <random>


struct beta_k{  constexpr static auto  className=my_static_string("beta");};
struct logPrior_k{   constexpr static auto  className=my_static_string("logPrior");};
struct logLik_k{   constexpr static auto  className=my_static_string("logLik");};




template <class logPrior_T,class logLik_T>
class ThlogLikelihood   {
public:
  typedef ThlogLikelihood self_type;
  typedef ThlogLikelihood element_type;

  constexpr static auto const className = my_static_string("Th_")+logPrior_T::className+my_static_string("_")+logLik_T::className;

  double beta() const { return beta_; }
  auto &logPrior() const { return prior_; }
  auto &logLik() const { return lik_; }
  auto &logPrior()  { return prior_; }
  auto &logLik()  { return lik_; }

  ThlogLikelihood(logPrior_T&& prior, logLik_T&& lik, double beta)
      : prior_{std::move(prior)}, lik_{std::move(lik)}, beta_{beta} {}

  ThlogLikelihood() = default;

  void set_beta(double beta) {
    beta_ = beta;
      }


      using cols=pack_concatenation_t<
          Cs<beta_k>,
          recursive_t<logPrior_k,typename logPrior_T::cols>,
            recursive_t<logLik_k,typename logLik_T::cols>
          >;

      using cols_w_unit=pack_concatenation_t<
          Cs<beta_k>,
          recursive_t<logPrior_k,typename logPrior_T::cols_w_unit>,
          recursive_t<logLik_k,typename logLik_T::cols_w_unit>
          >;

      using row_type=decltype(std::tuple_cat(std::tuple<double>{},
          row_type_t<logPrior_T>{},row_type_t<logLik_T>{}));

      using row_type_w_unit=decltype(std::tuple_cat(std::tuple<double>{},
                                                      row_type_w_unit_t<logPrior_T>{},row_type_w_unit_t<logLik_T>{}));


      template<class...Is>
      void insert_at(const Position<Is...>& p, row_type&& r)
      {

        auto [beta_r,logP_r,logLik_r]=distribute(Cs<double>{},
                                      transfer_t<row_type_t<logPrior_T>,Cs<> >{},
                                      transfer_t<row_type_t<logLik_T>,Cs<>>{},
                                      std::move(r));
        set_beta(std::get<0>(beta_r));
        logPrior().insert_at(p,std::move(logP_r));
        logLik().insert_at(p,std::move(logLik_r));


      }

      template<class...Is>
      void insert_at(const Position<Is...>& p, row_type_w_unit&& r)
      {

        auto [beta_r,logP_r,logLik_r]=distribute(Cs<double>{},
                                                     transfer_t<row_type_w_unit_t<logPrior_T>,Cs<>>{},
                                                     transfer_t<row_type_w_unit_t<logLik_T>,Cs<>>{},
                                                     std::move(r));
        set_beta(std::get<0>(beta_r));
        logPrior().insert_at(p,std::move(logP_r));
        logLik().insert_at(p,std::move(logLik_r));


      }


    public:

      friend   constexpr auto begin(const ThlogLikelihood& me) {
        return
            me.logPrior().begin()&& ::begin(me.logLik());
      }


      template<class Position>
      auto operator()(const Position& p,beta_k)const
      {return beta();}


      template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename logPrior_T::cols_w_unit{}),int>>
      auto operator()(const Position& p,recursive<logPrior_k,ind>)const->decltype (logPrior()(p,ind{}))
      {return logPrior()(p,ind{});}

      template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename logLik_T::cols_w_unit{}),bool>>
      auto operator()(const Position& p,recursive<logLik_k,ind> )const ->decltype (logLik()(p,ind{}))
      {return logLik()(p,ind{});}




      static auto begin() {return Position<>{};}
      bool next(Position<>& )const   {     return false;}

      template<class Position>
      auto& operator()(const Position& ){ return *this;}

      template<class Position>
      auto& operator()(const Position& )const{ return *this;}



      friend std::ostream& operator<<(std::ostream& os, const ThlogLikelihood& me)
      {
        return os<<" beta="<<me.beta()<<" logPrior="<<me.logPrior()<<" logLik="<<me.logLik();
      }

      friend std::istream& operator>>(std::istream& is,  ThlogLikelihood& me)
      {
        return is<<my_static_string(" beta=")>>me.beta()>>my_static_string(" logPrior=")>>me.logPrior()>>my_static_string(" logLik=")>>me.logLik();
      }


      friend bool operator==(const ThlogLikelihood& me, const ThlogLikelihood& ti)
      {
        return (me.beta()==ti.beta())&&(me.logPrior()==ti.logPrior())&&(me.logLik()==ti.logLik());
      }



private:
  logPrior_T prior_;
  logLik_T lik_;
  double beta_;
};


template <class logPrior_T,class logLik_T>
class ThlogLikelihoodV :public vector_space<x_i<beta_k,v<double,p<>>>,
    x_i<logPrior_k,logPrior_T>,x_i<logLik_k,logLik_T>>

{
public:
  using base_type=vector_space<x_i<beta_k,v<double,p<>>>,
                                 x_i<logPrior_k,logPrior_T>,x_i<logLik_k,logLik_T>>;
  typedef ThlogLikelihoodV self_type;
  typedef ThlogLikelihoodV element_type;

  constexpr static auto const className = my_static_string("Th_")+logPrior_T::className+my_static_string("_")+logLik_T::className;

  double& beta() const { return (*this)[beta_k{}]; }
  auto &logPrior() const { return (*this)[logPrior_k{}]; }
  auto &logLik() const { return (*this)[logPrior_k{}];; }
  double& beta()  { return (*this)[beta_k{}]; }
  auto &logPrior()  { return (*this)[logPrior_k{}]; }
  auto &logLik()  { return (*this)[logPrior_k{}];; }

  ThlogLikelihoodV(logPrior_T&& prior, logLik_T&& lik, double beta)
      : base_type {x_i{beta_k{},v(beta)},x_i{logPrior_k{},std::move(prior)},x_i{logLik_k{},std::move(lik)}}{}

  ThlogLikelihoodV() = default;

  using cols=typename base_type::myCols;

  using cols_w_unit=typename base_type::myCols_w_unit;

  using row_type= typename base_type::myrow;
  using row_type_w_unit= typename base_type::myrow_type_w_unit;

  using base_type::insert_at;

  using base_type::operator();








  static auto begin() {return Position<>{};}
  bool next(Position<>& )const   {     return false;}

  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const{ return *this;}




  friend std::istream& operator>>(std::istream& is,  ThlogLikelihoodV& me)
  {
    base_type& meb(me);
    return is>>meb;
  }


  friend bool operator==(const ThlogLikelihoodV& me, const ThlogLikelihoodV& ti)
  {
    base_type& meb(me);
    base_type& tib(ti);

    return  meb==tib;
  }



};



struct Parameters_ei{  constexpr static auto  className=my_static_string("Parameters");};
struct logP_k{   constexpr static auto  className=my_static_string("logP");};
struct Distribution_k{   constexpr static auto  className=my_static_string("Distribution");};


template <class Parameters_T, class logP_T, class Distribution_T>
class mcmc_sample
{
public:
  typedef mcmc_sample self_type;
  typedef mcmc_sample element_type;
  using template_types=Cs<Parameters_T,logP_T,Distribution_T>;

  constexpr static auto const className = my_static_string("mcmc_sample")+template_types::className;

  auto& x()  { return x_; }
  auto &logP()  { return logP_; }
  auto &distribution()  { return dist_; }
  auto& x() const { return x_; }
  auto &logP() const { return logP_; }
  auto &distribution() const { return dist_; }

  mcmc_sample(Parameters_T&& x, logP_T&& logP, Distribution_T d)
      : x_{std::move(x)}, logP_{std::move(logP)}, dist_{std::move(d)} {}

  mcmc_sample() = default;


  using cols=pack_concatenation_t<
      recursive_t<Parameters_ei,typename Parameters_T::cols>,
      recursive_t<logP_k,typename logP_T::cols>,
      recursive_t<Distribution_k,typename Distribution_T::cols>
      >;

  using cols_w_unit=pack_concatenation_t<
      recursive_t<Parameters_ei,typename Parameters_T::cols_w_unit>,
      recursive_t<logP_k,typename logP_T::cols_w_unit>,
      recursive_t<Distribution_k,typename Distribution_T::ccols_w_unitols>
      >;

  using row_type=decltype(std::tuple_cat(  row_type_t<Parameters_T>{},
                                           row_type_t<logP_T>{},
                                           row_type_t<Distribution_T>{}));

  using row_type_w_unit=decltype(std::tuple_cat(  row_type_w_unit_t<Parameters_T>{},
                                           row_type_w_unit_t<logP_T>{},
                                           row_type_w_unit_t<Distribution_T>{}));


  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type&& r)
  {

    auto [x_r,logP_r,d_r]=distribute( transfer_t<row_type_t<Parameters_T>,Cs<> >{},
                                                 transfer_t<row_type_t<logP_T>,Cs<> >{},
                                                 transfer_t<row_type_t<Distribution_T>,Cs<>>{},
                                                 std::move(r));
    x().insert_at(p,std::move(x_r));
    logP().insert_at(p,std::move(logP_r));
    distribution().insert_at(p,std::move(d_r));


  }

  template<class...Is>
  void insert_at(const Position<Is...>& p, row_type_w_unit&& r)
  {

    auto [x_r,logP_r,d_r]=distribute( transfer_t<row_type_w_unit_t<Parameters_T>,Cs<> >{},
                                         transfer_t<row_type_w_unit_t<logP_T>,Cs<> >{},
                                         transfer_t<row_type_w_unit_t<Distribution_T>,Cs<>>{},
                                         std::move(r));
    x().insert_at(p,std::move(x_r));
    logP().insert_at(p,std::move(logP_r));
    distribution().insert_at(p,std::move(d_r));


  }



  friend   constexpr auto begin(const mcmc_sample& me) {
    return
        me.x().begin()&& me.logP().begin()&& me.distribution().begin();
  }


  template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename Parameters_T::cols_w_unit{}),int>>
  auto operator()(const Position& p,recursive<Parameters_ei,ind>)const->decltype (x()(p,ind{}))
  {return x()(p,ind{});}


  template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename logP_T::cols_w_unit{}),int>>
  auto operator()(const Position& p,recursive<logP_k,ind>)const->decltype (logP()(p,ind{}))
  {return logP()(p,ind{});}

  template<class Position, class ind, typename=std::enable_if_t<is_in_pack(ind{},typename Distribution_T::cols_w_unit{}),bool>>
  auto operator()(const Position& p,recursive<Distribution_k,ind> )const ->decltype (distribution()(p,ind{}))
  {return distribution()(p,ind{});}




  static auto begin() {return Position<>{};}
  bool next(Position<>& )const   {     return false;}

  template<class Position>
  auto& operator()(const Position& ){ return *this;}

  template<class Position>
  auto& operator()(const Position& )const{ return *this;}



  friend std::ostream& operator<<(std::ostream& os, const mcmc_sample& me)
  {
    return os<<" Paramters="<<me.x()<<" logP="<<me.logP()<<" Distribution="<<me.distribution();
  }

  friend std::istream& operator>>(std::istream& is,  mcmc_sample& me)
  {
    return is>>my_static_string(" Paramters=")>>me.x()>>my_static_string(" logP=")>>me.logP()>>my_static_string(" Distribution=")>>me.distribution();
  }


  friend bool operator==(const mcmc_sample& me, const mcmc_sample& ti)
  {
    return (me.x()==ti.x())&&(me.logP()==ti.logP())&&(me.distribution()==ti.distribution());
  }



private:
  Parameters_T x_;
  logP_T logP_;
  Distribution_T dist_;
};



struct walker_k{  constexpr static auto  className=my_static_string("walker");};


template <class mcmc_s>
class emcee_sample: public vector_field<vec<walker_k>,mcmc_s>
{
public:
  typedef emcee_sample self_type;
  using base_type=vector_field<vec<walker_k>,mcmc_s>;
  constexpr static auto const className =
      my_static_string("emcee_sample") + mcmc_s::className;

  mcmc_s &Walker(std::size_t i) { return base_type::operator()(Position<walker_k>(i)); }
  mcmc_s const &Walker(std::size_t i) const { return base_type::operator()(Position<walker_k>(i)); }
  std::size_t numWalkers() const { return base_type::size(); }


  base_type &walkers() const { return *this; }
  // auto& acceptance()const {return  accept_;}
  emcee_sample() = default;
  emcee_sample(std::vector<mcmc_s> &&walkers) : base_type{std::move(walkers)} {}
  emcee_sample(const std::vector<mcmc_s> &walkers)
      : base_type{walkers} {}



};


  template <class PriorModel_T,class LikelihoodModel_T>
  class Thermodynamic_Model :public vector_space<x_i<beta_k,v<double,p<>>>,
                                               x_i<logPrior_k,PriorModel_T>,x_i<logLik_k,LikelihoodModel_T>>

  {
  public:
    using base_type=vector_space<x_i<beta_k,v<double,p<>>>,
                                   x_i<logPrior_k,PriorModel_T>,x_i<logLik_k,LikelihoodModel_T>>;
    typedef Thermodynamic_Model self_type;
    typedef Thermodynamic_Model element_type;

    constexpr static auto const className = my_static_string("Th_")+PriorModel_T::className+my_static_string("_")+LikelihoodModel_T::className;

    double& beta() const { return (*this)[beta_k{}]; }
    auto &logPrior() const { return (*this)[logPrior_k{}]; }
    auto &logLik() const { return (*this)[logPrior_k{}];; }
    double& beta()  { return (*this)[beta_k{}]; }
    auto &logPrior()  { return (*this)[logPrior_k{}]; }
    auto &logLik()  { return (*this)[logPrior_k{}];; }

    Thermodynamic_Model(PriorModel_T&& prior, LikelihoodModel_T&& lik, double beta)
        : base_type {x_i{beta_k{},v(beta)},x_i{logPrior_k{},std::move(prior)},x_i{logLik_k{},std::move(lik)}}{}

    Thermodynamic_Model() = default;

    using cols=typename base_type::myCols;

    using cols_w_unit=typename base_type::myCols_w_unit;

    using row_type= typename base_type::myrow;
    using row_type_w_unit= typename base_type::myrow_type_w_unit;

    using base_type::insert_at;

    using base_type::operator();



    static auto begin() {return Position<>{};}
    bool next(Position<>& )const   {     return false;}

    template<class Position>
    auto& operator()(const Position& ){ return *this;}

    template<class Position>
    auto& operator()(const Position& )const{ return *this;}

    friend std::istream& operator>>(std::istream& is,  ThlogLikelihoodV& me)
    {
      base_type& meb(me);
      return is>>meb;
    }


    friend bool operator==(const Thermodynamic_Model& me, const Thermodynamic_Model& ti)
    {
      base_type& meb(me);
      base_type& tib(ti);

      return  meb==tib;
    }



  };





template <class PriorModel, class LikelihoodModel>
class Thermodynamic_Model_Series {
  static_assert(std::is_same_v<typename PriorModel::Parameters,
                               typename LikelihoodModel::Parameters>);

public:
  typedef Thermodynamic_Model_Series self_type;
  typedef Cs<PriorModel, LikelihoodModel> template_types;
  constexpr static auto const className =
      my_static_string("Thermodynamic_Model_Series") +
      my_trait<template_types>::className;

public:
  typedef typename PriorModel::Parameters Parameters;

  typedef Thermodynamic_Model<PriorModel, LikelihoodModel> subModel;

  typedef ThDlogLikelihood DLikelihoodResult;
  typedef ThlogLikelihood LikelihoodResult;
  static auto get_constructor_fields() {
    return std::make_tuple(
        grammar::field(C<self_type>{}, "prior", &self_type::prior),
        grammar::field(C<self_type>{}, "likelihood", &self_type::likelihood),
        grammar::field(C<self_type>{}, "betas", &self_type::betas));
  }

  Thermodynamic_Model_Series(const PriorModel &prior,
                             const LikelihoodModel &lik,
                             std::vector<double> beta)
      : prior_{prior}, lik_{lik}, beta_{beta}, models_{getModels(prior_, lik_,
                                                                 beta)} {}

  double beta(std::size_t i) const { return models_[i].beta(); }

  auto &betas() const { return beta_; }

  auto name(std::size_t i) const { return model(0).name(i); }

  auto name_size() const { return model(0).size(); }

  std::size_t size() const { return models_.size(); }

  void set_beta(double abeta, std::size_t i) {
    beta_[i] = abeta;
    models_[i].set_beta(abeta);
  }

  Thermodynamic_Model<PriorModel, LikelihoodModel> &model(std::size_t i) {
    return models_[i];
  }
  Thermodynamic_Model<PriorModel, LikelihoodModel> const &
  model(std::size_t i) const {
    return models_[i];
  }

  Thermodynamic_Model_Series(const Thermodynamic_Model_Series &) = default;

  auto &prior() const { return prior_; }
  auto &likelihood() const { return lik_; }
  Thermodynamic_Model_Series() = default;

private:
  PriorModel prior_;
  LikelihoodModel lik_;
  std::vector<double> beta_;
  std::vector<Thermodynamic_Model<PriorModel, LikelihoodModel>> models_;

  static std::vector<Thermodynamic_Model<PriorModel, LikelihoodModel>>
  getModels(const PriorModel &prior, const LikelihoodModel &lik,
            std::vector<double> beta) {
    std::vector<Thermodynamic_Model<PriorModel, LikelihoodModel>> out;
    for (std::size_t i = 0; i < beta.size(); ++i)
      out.push_back(Thermodynamic_Model<PriorModel, LikelihoodModel>(prior, lik,
                                                                     beta[i]));
    return out;
  }
};


template <class Model> class stretch_move {
public:
  typedef typename Model::LikelihoodResult LikelihoodResult;
  typedef stretch_move_Distribution Distribution;
  typedef typename Model::Parameters Parameters;

  static myOptional_t<LikelihoodResult> compute_Likelihood(const Model &m,
                                                           Parameters &x) {
    return m.compute_Likelihood(x);
  }

  constexpr static auto const className =
      my_static_string("stretch_move") + my_trait<Model>::className;


  static double Acceptance(double Z, const logLikelihood &candidateLogLik,
                           const logLikelihood &currentLogLik,
                           std::size_t numParam) {
    double logA = candidateLogLik.logL() - currentLogLik.logL() +
                  log(Z) * (numParam - 1);
    return std::min(0.0, logA);
  }

  template <class Adaptive_Strecth_Move>
  static void move(const Model &model, std::mt19937_64 &mt,
                   const stretch_move_Distribution &d,
                   const emcee_sample<Model, Adaptive_Strecth_Move> &s,
                   const std::vector<std::size_t> &index,
                   mcmc_sample_t<Model, Adaptive_Strecth_Move> &current) {
    std::uniform_int_distribution<std::size_t> u(1, index.size());
    std::size_t i = index[u(mt) - 1];
    double z = d.sample(mt);
    Parameters candidate =
        (current.x() - (s.Walker(i).x())) * z + s.Walker(i).x();
    auto lik = compute_Likelihood(model, candidate);
    if (!lik) {
      current.push_rejected();

    } else {
      auto logA = Acceptance(z, lik.value(), current, candidate.size());
      double A = std::exp(logA);
      std::uniform_real_distribution<double> real(0, 1);
      double r = real(mt);
      bool accept = r < A;
      if (accept) {
        current = mcmc_sample_t<Model, Adaptive_Strecth_Move>(
            std::move(candidate), std::move(lik).value(),
            std::move(current).g());
        current.push_accepted();
      } else
        current.push_rejected();
    }
  }
};

template <class Model> class adaptive_stretch_mover {
public:
  typedef stretch_move<Model> Mover;
  typedef typename Model::LikelihoodResult LikelihoodResult;
  typedef typename Mover::Distribution Distribution;
  typedef typename Mover::Parameters Parameters;

  constexpr static auto const className =
      my_static_string("adaptive_stretch_mover_") +
      my_trait<Model>::className;

  typedef adaptive_stretch_mover self_type;
  static auto get_constructor_fields() {
    return std::make_tuple(
        grammar::field(C<self_type>{}, "target", &self_type::target),
        grammar::field(C<self_type>{}, "alfa_map", &self_type::alfa_map),
        grammar::field(C<self_type>{}, "alfa", &self_type::alfa));
  }
  stretch_move_Distribution sample(std::mt19937_64 &mt) const {
    return stretch_move_Distribution(alfa_.sample(mt));
  }

  virtual ~adaptive_stretch_mover() {}
  virtual std::ostream &put(std::ostream &os) const { return os << *this; }

  void push_outcome(const stretch_move_Distribution &d, bool accept) {
    if (accept)
      alfa_map_[d.alpha()].push_accept();
    else
      alfa_map_[d.alpha()].push_reject();
  }

  void actualize() { alfa_ = alfa_map_.Distribute_on_p(target_); }
  adaptive_stretch_mover(double target, const std::vector<double> &alfas)
      : target_{target}, alfa_map_{Beta_map<double>::UniformPrior(alfas)},
        alfa_{} {
    actualize();
  }
  adaptive_stretch_mover() = default;

  adaptive_stretch_mover(double target, const Beta_map<double> &alfa_map,
                         const Probability_map<double> &alfa)
      : target_{target}, alfa_map_{alfa_map}, alfa_{alfa} {}

  double target() const { return target_; }
  Beta_map<double> const &alfa_map() const { return alfa_map_; }
  Probability_map<double> const &alfa() const { return alfa_; }

  adaptive_stretch_mover(const adaptive_stretch_mover &other)
      : target_{other.target_}, alfa_map_{other.alfa_map_},
        alfa_{other.alfa_} {}
  adaptive_stretch_mover(adaptive_stretch_mover &&other)
      : target_{other.target_}, alfa_map_{std::move(other.alfa_map_)},
        alfa_{std::move(other.alfa_)} {}

  adaptive_stretch_mover &operator=(const adaptive_stretch_mover &other) {
    adaptive_stretch_mover tmp(other);
    *this = std::move(tmp);
    return *this;
  }
  adaptive_stretch_mover &operator=(adaptive_stretch_mover &&other) {
    target_ = other.target_;
    alfa_map_ = std::move(other.alfa_map_);
    alfa_ = std::move(other.alfa_);
  }

  static Data_Index_scheme data_index() {
    Data_Index_scheme out;
    out.push_back("target", {"const"});
    out = concatenate(
        std::move(out),
        Beta_map<double>::data_index("stretch_alfa", "stretch_alfa"),
        Probability_map<double>::data_index("stretch_alfa", "stretch_alfa"));

    return out;
  }

  static auto get_data_index_static() {
    return Concatenate_tuple_static(
        std::make_tuple(make_data_static(
            std::make_tuple(I_s(i_stretch_alpha(),
                [](const self_type &self) {
                  return self.alfa_map().x().size();
                },
                0)),
            std::make_tuple(F_s(s_stretch_alpha{},
                                [](const self_type &s, std::size_t i_a) {
                                  return s.alfa().x()[i_a];
                                }),
                            F_s(CT_s<s_stretch_alpha, s_use, s_probability>{},
                                [](const self_type &s, std::size_t i_a) {
                                  return s.alfa().p().at(s.alfa().x()[i_a]);
                                })))),
        Compose_static(
            std::make_tuple(I_s(i_stretch_alpha(),
                [](const self_type &self) {
                  return self.alfa_map().x().size();
                },
                0)),
            F_s(CT_s<s_stretch_alpha, s_obs, s_probability>{},
                [](const self_type &self, std::size_t i_a) -> auto const & {
                  return self.alfa_map().get_Map().at(
                      self.alfa_map().x()[i_a]);
                }),
            Beta_Distribution::get_data_index_static()));
  }

private:
  double target_;
  Beta_map<double> alfa_map_;
  Probability_map<double> alfa_;
};


class Ensemble_Metropolis_Hastings {
private:
  std::size_t numWalkers_;
  std::size_t numTrials_;

public:
  typedef Ensemble_Metropolis_Hastings self_type;
  constexpr static auto const className =
      my_static_string("Ensemble_Metropolis_Hastings");


  std::size_t numWalkers() const { return numWalkers_; }
  std::size_t numTrials() const { return numTrials_; }

  Ensemble_Metropolis_Hastings(std::size_t numberWalkers, std::size_t num_tries)
      : numWalkers_{numberWalkers}, numTrials_{num_tries} {}
  Ensemble_Metropolis_Hastings() = default;

  template <class Random_Engine>
  static std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
  random_split(Random_Engine &mt, std::size_t n) {
    std::vector<std::size_t> index(n);
    for (std::size_t i = 0; i < n; ++i)
      index[i] = i;

    std::shuffle(index.begin(), index.end(), mt);
    std::size_t half = n / 2;

    std::vector<std::size_t> one(index.begin(), index.begin() + half);
    std::vector<std::size_t> two(index.begin() + half, index.end());
    return {one, two};
  }

  template <class Model, class Randome_Engine, class Adaptive_Mover,
            class Mover = typename Adaptive_Mover::Mover>
  myOptional_t<emcee_sample<Model, Adaptive_Mover>>
  start(const Model &model, Randome_Engine &mt,
        Adaptive_Mover &adaptive) const {
    typedef myOptional_t<emcee_sample<Model, Adaptive_Mover>> Op;
    auto w = emcee_sample<Model, Adaptive_Mover>::getWalkers(
        model, mt, adaptive, numWalkers(), numTrials());
    if (w)
      return Op(emcee_sample<Model, Adaptive_Mover>(std::move(w).value()));
    else {
      return Op(false, w.error());
    }
  }

  template <class Model, class Random_Engine,
            template <class> class Adaptive_Mover,
            class Mover = typename Adaptive_Mover<Model>::Mover>
  bool next_para(const Model &model, std::vector<Random_Engine> &mt,
                 adaptive_stretch_mover<Model> &adaptive,
                 emcee_sample<Model, stretch_move<Model>> &current) {

    //   typedef typename Adaptive_Mover<Model>::Mover stretch_move;
    auto tu = random_split(mt[0], current.numWalkers());
    auto one = std::move(std::get<0>(tu));
    auto two = std::move(std::get<1>(tu));
    //    auto [one, two] =tu;
#pragma omp parallel for
    for (std::size_t ii = 0; ii < one.size(); ++ii) {
      auto i = one[ii];
      current.Walker(i).set_g(adaptive.sample(mt[ii]));
      Mover::move(model, current.mt(i), current.Walker(i).g(), current, one,
                  current.Walker(i));
    }
    for (std::size_t ii = 0; ii < one.size(); ++ii) {
      auto i = one[ii];
      adaptive.push_outcome(current.Walker(i).g(), current.accept(i));
    }

#pragma omp parallel for
    for (std::size_t ii = 0; ii < two.size(); ++ii) {
      auto i = two[ii];
      current.Walker(i).g() = adaptive.sample(mt);
      stretch_move<Model>::move(model, current.mt(i), current.Walker(i).g(),
                                current, two, current.Walker(i));
    }
    for (std::size_t ii = 0; ii < two.size(); ++ii) {
      auto i = two[ii];
      adaptive.push_outcome(current.Walker(i).g(), current.accept(i));
    }
    adaptive.actualize();
    return current;
  }

  template <class Model, class Random_Engine, class Adaptive_Mover,
            class Sample>
  Op_void next(const Model &model, Random_Engine &mt, Adaptive_Mover &adaptive,
               Sample &current) const {

    typedef typename Adaptive_Mover::Mover Mover;
    auto tu = random_split(mt, current.numWalkers());
    auto one = std::move(std::get<0>(tu));
    auto two = std::move(std::get<1>(tu));
    //    auto [one, two] =tu;
    //#pragma omp parallel for
    for (std::size_t ii = 0; ii < one.size(); ++ii) {
      auto i = one[ii];
      current.Walker(i).set_g(adaptive.sample(mt));
      Mover::move(model, mt, current.Walker(i).g(), current, one,
                  current.Walker(i));
    }
    for (std::size_t ii = 0; ii < one.size(); ++ii) {
      auto i = one[ii];
      adaptive.push_outcome(current.Walker(i).g(), current.accept(i));
    }

    //#pragma omp parallel for
    for (std::size_t ii = 0; ii < two.size(); ++ii) {
      auto i = two[ii];
      current.Walker(i).set_g(adaptive.sample(mt));
      stretch_move<Model>::move(model, mt, current.Walker(i).g(), current, two,
                                current.Walker(i));
    }
    for (std::size_t ii = 0; ii < two.size(); ++ii) {
      auto i = two[ii];
      adaptive.push_outcome(current.Walker(i).g(), current.accept(i));
    }
    adaptive.actualize();
    return Op_void(true, "");
  }

  template <class Model, class mySample>
  auto compute_PartialDLikelihood(const Model &M, const mySample &current,
                                  std::ostream &os) const {
    std::vector<
        std::decay_t<decltype(M.compute_PartialDLikelihood(
                                   current.Walker(0).x(), current.Walker(0), os)
                                  .value())>>
        out(current.numWalkers());

    for (std::size_t i = 0; i < current.numWalkers(); ++i) {
      out[i] = M.compute_PartialDLikelihood(current.Walker(i).x(),
                                            current.Walker(i), os)
                   .value();
    }
    return out;
  }

  static std::vector<std::mt19937_64> mts(std::mt19937_64 &mt, std::size_t n) {
    std::uniform_int_distribution<typename std::mt19937_64::result_type> useed;
    std::vector<std::mt19937_64> out(n);
    for (std::size_t i = 0; i < n; ++i)
      out[i].seed(useed(mt));
    return out;
  }
};


class Ensemble_Parallel_Tempering {

public:
  typedef Ensemble_Parallel_Tempering self_type;
  constexpr static auto const className =
      my_static_string("Ensemble_Parallel_Tempering");

  static auto get_constructor_fields() {
    return std::make_tuple(
        grammar::field(C<self_type>{}, "Ensemble_Metropolis",
                       &self_type::get_Ensamble_Metropolis),
        grammar::field(C<self_type>{}, "pJump", &self_type::pJump));
  }

  template <class priorModel, class LikelihoodModel, class RandomEngine>
  static std::vector<RandomEngine> start_Random(
      const Thermodynamic_Model_Series<priorModel, LikelihoodModel> &model,
      RandomEngine &mt) {
    return mts(mt, model.size());
  }

  template <class Model, class Random_Engine, class AdaptiveMover>
  myOptional<Ensemble_samples<Model, AdaptiveMover>, reg_tag>
  start(const Model &model, std::vector<Random_Engine> &mt,
        std::vector<AdaptiveMover> adaptives) const {
    return Ensemble_samples<Model, AdaptiveMover>::evaluate(
        get_Ensamble_Metropolis(), model, mt, adaptives);
  }

  template <class Model, class Random_Engine, class AdaptiveMover>
  Op_void next(const Model &model, std::vector<Random_Engine> &mt,
               std::vector<AdaptiveMover> &adaptives,
               Ensemble_samples<Model, AdaptiveMover> &current) const {

    std::vector<Op_void> op(model.size());
#pragma omp parallel for
    for (std::size_t i = 0; i < model.size(); ++i) {
      op[i] = get_Ensamble_Metropolis().next(model.model(i), mt[i],
                                             adaptives[i], current.Scout(i));
    }

    auto res = consolidate(std::move(op));
    if (res) {
      std::uniform_real_distribution<> U;
      double r = U(mt[0]);

      if (r < P_jump_) {
        std::size_t j;
        if (U(mt[0]) < 0.5)
          j = 0;
        else
          j = 1;

#pragma omp parallel for
        for (std::size_t i = j; i < model.size() - 1; i += 2) {
          std::size_t j = std::uniform_int_distribution<std::size_t>(
              0, current.Scout(i).numWalkers() - 1)(mt[i]);
          std::size_t k = std::uniform_int_distribution<std::size_t>(
              0, current.Scout(i + 1).numWalkers() - 1)(mt[i]);
          double A = current.Accept(i, j, k, model);
          double r = std::uniform_real_distribution<>(0, 1)(mt[i]);
          if (r < A)
            current.swap(i, j, k, model);
        }
      }
    }
    return res;
  }

  Ensemble_Metropolis_Hastings const &get_Ensamble_Metropolis() const {
    return emh_;
  }
  Ensemble_Parallel_Tempering(Ensemble_Metropolis_Hastings ensamble_mh,
                              double probability_jump)
      : emh_{ensamble_mh}, P_jump_{probability_jump} {}

  double pJump() const { return P_jump_; }
  Ensemble_Parallel_Tempering() = default;
  template <class Model, class mySample>
  auto compute_PartialDLikelihood(const Model &M, const mySample &current,
                                  std::ostream &os) const {
    std::vector<decltype(get_Ensamble_Metropolis().compute_PartialDLikelihood(
        M.model(0), current.Scout(0), os))>
        out(M.size());

#pragma omp parallel for
    for (std::size_t i = 0; i < M.size(); ++i) {
      out[i] = get_Ensamble_Metropolis().compute_PartialDLikelihood(
          M.model(i), current.Scout(i), os);
    }
    return out;
  }

private:
  Ensemble_Metropolis_Hastings emh_;
  double P_jump_;
};



template <class Metropolis_algorithm, class ModelSeries,
          class Distribution_Generator, class OutputGenerator>
auto run_Montecarlo_Markov_Chain(const std::string idname,
                                    const std::string &info,
                                    const Metropolis_algorithm &mcmc,
                                    std::mt19937_64 &mt,  ModelSeries &M,
                                    Distribution_Generator &G,
                                    std::size_t nsamples, OutputGenerator &O) {

  O.save(idname, info, mcmc, M, nsamples);

  assert(([&O, &idname, &info, &M, &mcmc ]() -> bool {
    std::string info_;
    Metropolis_algorithm mcmc_;
    ModelSeries M_;
    std::size_t nsamples_;
    double eps = std::numeric_limits<double>::epsilon() * 10;
    O.load(idname, info_, mcmc_, M_, nsamples_);
    bool out = true;
    if (!(info_ == info)) {
      std::cerr << " info \n original: " << info << "\n loaded: " << info_;
      out = false;
    }
    if (!are_Equal_v(M_, M, eps, eps, std::cerr))
      out = false;
    if (!are_Equal_v(mcmc_, mcmc, eps, eps, std::cerr))
      out = false;
    return out;
  }()));

  auto mts = mcmc.start_Random(M, mt);
  auto state = mcmc.start(M, mts, G);
  if (!state) {
    std::cerr << state.error();
    return Op_void(false, " does not start at all " + state.error());
  }

  std::size_t isample = 0;

  typedef std::decay_t<decltype(
      mcmc.compute_PartialDLikelihood(M, state.value(), std::cerr))>
      AnaState;

  auto state_data =
      compose_state_model_static<std::decay_t<decltype(state.value())>,
                                 ModelSeries>();
  auto gen_data =
      compose_adg_model_static<Distribution_Generator, ModelSeries>();
  auto ana_data =
      compose_ana_state_model_static(Cs<AnaState>(), Cs<ModelSeries>());

  return continue_Montecarlo_Markov_Chain(idname, isample, state.value(), mts,
                                          mcmc, M, G, nsamples, state_data,
                                          gen_data, ana_data, O);
}



template <class PriorModel, class Likelihood_Model, class OutputGenerator>
auto run_Thermo_emcee(const std::string id, const std::string info,
                         const PriorModel &prior, const Likelihood_Model &lik,
                         std::mt19937_64 &mt, const std::vector<double> &betas,
                         std::size_t numWalkers,
                         const std::vector<double> &alfas, double pjump,
                         double target_prob, std::size_t nSamples,
                         std::size_t ntrials, OutputGenerator &output) {

  typedef Thermodynamic_Model<PriorModel, Likelihood_Model> Model;
  typedef
      typename Ensemble_Metropolis_Hastings::adaptive_stretch_mover<
          Model>
          Adaptive;
  // typedef  Thermodynamic_Model_Series<PriorModel, Likelihood_Model> Models;
  // typedef Ensemble_Parallel_Tempering MCMC;
  Thermodynamic_Model_Series<PriorModel, Likelihood_Model> model(prior, lik,
                                                                 betas);

  Ensemble_Parallel_Tempering ET(
      Ensemble_Metropolis_Hastings(numWalkers, ntrials), pjump);
  std::vector<Adaptive> alfa_adap(betas.size(), Adaptive(target_prob, alfas));

  return run_Montecarlo_Markov_Chain(id, info, ET, mt, model, alfa_adap,
                                     nSamples, output);
}






#endif // QM_EVIDENCE_H
