#ifndef QM_UNITS_STANDARD_H
#define QM_UNITS_STANDARD_H

#include "qm_unit.h"
#include "qm_value.h"
#include "qm_operator.h"

struct m{ constexpr static auto  name=my_static_string("m");};
struct s{constexpr static auto  name=my_static_string("s");};
struct kg{constexpr static auto  name=my_static_string("kg");};
struct A{constexpr static auto  name=my_static_string("A");};

struct ps{constexpr static auto  name=my_static_string("ps");};
struct V{constexpr static auto  name=my_static_string("V");};
struct GHz{constexpr static auto  name=my_static_string("GHz");};


typedef p<u<ps,1>> ps_u;
typedef p<u<V,1>> V_u;
typedef p<u<GHz,1>> GHz_u;

constexpr const auto GHz_f=v(1E9,p_t<u<GHz,-1>,u<s,-1>>{});
constexpr const auto ps_f=v(1E-12,p_t<u<s,1>,u<ps,-1>>{});

constexpr const auto Gsad=v(1,GHz_u{});


struct GHz_F {
    friend constexpr auto collect_arguments(GHz_F){return arguments<>{};}
    inline static constexpr auto name=my_static_string("GHz_f");
};

struct ps_F {
    friend constexpr auto collect_arguments(ps_F){return arguments<>{};}
    inline static constexpr auto name=my_static_string("ps_f");
};

template<>
struct Op<GHz_F> {
    constexpr auto operator[] (GHz_F)const {return *this;}
    __host__ __device__ constexpr auto operator()()const { return GHz_f;}
};

template<>
struct Op<ps_F> {
    constexpr auto operator[] (ps_F)const {return *this;}
    __host__ __device__ constexpr auto operator()()const { return ps_f;}
};



#endif // QM_UNITS_STANDARD_H
