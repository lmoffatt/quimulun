#ifndef QM_QUIMULUN_ENGINE_H
#define QM_QUIMULUN_ENGINE_H

#include "qm_quimulun.h"
#include "qm_function_engine.h"
#include "qm_mapu.h"

#include "qm_distribution_engine.h"
#include "qm_units_standard.h"
#include "qm_span.h"



template<class Id,  class... in_dm,class... out_dm>
constexpr auto compute_span(Id,Domain<in_dm...> in,Domain<out_dm...> out)
{

    return in.span(Id{})&&out.span(Id{});
}

template<class F, class...Args,  class... in_dm,class... out_dm>
constexpr auto compute_span(Op<F,Args...>,Domain<in_dm...> in,Domain<out_dm...> out)
{

  //  using test=typename Cs<Cs<Args,decltype (compute_span(Args{},in,out))>...>::esta;
    return (compute_span(Args{},in,out)+...+index_prod<>{});
}




template<class Id,  class... in_dm,class... out_dm>
constexpr auto compute_type(Id,Domain<in_dm...> in,Domain<out_dm...> out)
{

    return in.element_type(Id{})&&out.element_type(Id{});
}

template<long n,class... in_dm,class... out_dm>
constexpr auto compute_type(N<n>,Domain<in_dm...> ,Domain<out_dm...> )
{
    return Type<Op<N<n>>>{}();
}

template<class... in_dm,class... out_dm>
constexpr auto compute_type(Pi,Domain<in_dm...> ,Domain<out_dm...> )
{
    return Type<Op<Pi>>{}();
}

template<class... in_dm,class... out_dm>
constexpr auto compute_type(GHz_F,Domain<in_dm...> ,Domain<out_dm...> )
{
    return Type<Op<GHz_F>>{}();
}

template<class... in_dm,class... out_dm>
constexpr auto compute_type(ps_F,Domain<in_dm...> ,Domain<out_dm...> )
{
    return Type<Op<ps_F>>{}();
}


template<class... in_dm,class... out_dm>
constexpr auto compute_type(Sample,Domain<in_dm...> ,Domain<out_dm...> )
{
    return Type<Sample>{};
}


template<class... in_dm,class... out_dm>
constexpr auto compute_type(random_generator,Domain<in_dm...> ,Domain<out_dm...> )
{

    return Type<Op<random_generator>>{}();
}




template<class F, class...Args,  class... in_dm,class... out_dm>
constexpr auto compute_type(Op<F,Args...>,Domain<in_dm...> in,Domain<out_dm...> out)
{
    //using test=typename Cs<Cs<Args,decltype (compute_type(Args{},in,out))>...>::ja;
    return Type<Op<F>>{}(compute_type(Args{},in,out)...);
}



template<class Id0,class F, class...Args,  class... in_dm,class... out_dm>
auto compute_domain(Id0,Calculate,Op<F,Args...>,Domain<in_dm...> in,Domain<out_dm...> out)
{
    using ele_type=decltype(compute_type(Op<F,Args...>{}, in,out));
    using  s=decltype (compute_span(Op<F,Args...>{}, in,out));
    return Domain<d_i<Id0,s,typename ele_type::type>>{};
}




template<class Id0,class Dist, class... Id,  class... in_dm,class... out_dm>
auto compute_domain(Id0,Sample,Op<Dist,Id...>,Domain<in_dm...> in,Domain<out_dm...> out)
{
    using ele_type=decltype(compute_type(Op<Dist,Sample,random_generator,Id...>{}, in,out));
    using  s=decltype (compute_span(Op<Sample,Id...>{}, in,out));
    return Domain<d_i<Id0,s,typename ele_type::type>>{};
}


template<template<class...>class F, class Id,class G, class... in_dm,class... out_dm>
auto compute_domain(Calculate,F<Id,G>,Domain<in_dm...> in,Domain<out_dm...> out)
{
    auto un_resolved_arguments=collect_arguments(F<Id,G>{})-get_variables(in)-get_variables(out);

    // using test=typename decltype (un_resolved_arguments)::K;
    if constexpr (!std::is_same_v<decltype(un_resolved_arguments),arguments<>>)
            return Error<missing<Id,decltype(un_resolved_arguments)>>{};
    else
    return compute_domain(Id{},Calculate{},G{},in,out);
}

template<template<class...>class F, class Id,class G, class... in_dm,class... out_dm>
auto compute_domain(Sample,F<Id,G>,Domain<in_dm...> in,Domain<out_dm...> out)
{
    auto un_resolved_arguments=collect_arguments(F<Id,G>{})-get_variables(in)-get_variables(out);

    // using test=typename decltype (un_resolved_arguments)::K;
    if constexpr (!std::is_same_v<decltype(un_resolved_arguments),arguments<>>)
            return Error<missing<Id,decltype(un_resolved_arguments)>>{};
    else
          return compute_domain(Id{},Sample{},G{},in,out);


}



template<class... Fids, class... Dids,class ...Fs, class... in_dm,class... out_dm>
auto compute_domain( Sample,quimulun<Fs...> q, functions<Fids...> variables, distributions<Dids...> parameters,Domain<in_dm...> in,Domain<out_dm...> out)
{
    auto new_parameters=join(compute_domain(Sample{},q[Dids{}],in,out)...);
    auto new_variables=join(compute_domain(Calculate{},q[Fids{}],in,out)...);

    auto next_variables=variables-get_variables(new_variables);
    auto next_parameters=parameters-get_variables(new_parameters);


    auto new_out=join(new_parameters,new_variables);

    auto next_out=join(out,new_out);
    new_out.is_empty();
    if constexpr (std::is_same_v<decltype (out),decltype (next_out) >)
            return out;
    else
        return compute_domain(Sample{},q,next_variables,next_parameters,in,next_out);


}



template<class engine, class ...Fs, class TableIndex,class...Data>
auto execute(engine e,Sample,quimulun<Fs...> q, const mapu<TableIndex,Data...>& d)
{
    auto parameters=get_distributions(q)-get_variables(Type<mapu<TableIndex,Data...>>{});
    auto variables=get_functions(q)-get_variables(Type<mapu<TableIndex,Data...>>{});
    auto args=collect_arguments(q);

    return compute_domain(Sample{},q,variables,parameters,get_domain(d),Domain<>{});
}


template<class engine, class ...Fs, class TableIndex,class...Data>
auto execute(engine e,Replicate,quimulun<Fs...> q, const mapu<TableIndex,Data...>& d)
{
    auto parameters=get_distributions(q);
    auto variables=get_functions(q);//-get_variables(Type<mapu<TableIndex,Data...>>{});
    auto args=collect_arguments(q);

    return compute_domain(Sample{},q,variables,parameters,get_domain(d),Domain<>{});
}






#endif // QM_QUIMULUN_ENGINE_H
