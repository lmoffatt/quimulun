#ifndef QM_DISTRIBUTION_VARIABLES_H
#define QM_DISTRIBUTION_VARIABLES_H




template<class Id>  struct mean{
  inline constexpr static auto  name_impl=my_static_string("mean(");
  inline constexpr static auto  name=name_impl+Id::name+my_static_string(")");
};

template<class Id>  struct stddev{
  inline constexpr static auto  name_impl=my_static_string("stddev(");
  inline constexpr static auto  name=name_impl+Id::name+my_static_string(")");
};

template<class Id>  struct variance{
  inline constexpr static auto  name_impl=my_static_string("variance(");
  inline constexpr static auto  name=name_impl+Id::name+my_static_string(")");
};


#endif // QM_DISTRIBUTION_VARIABLES_H
