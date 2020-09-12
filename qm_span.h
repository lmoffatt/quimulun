#ifndef QM_SPAN_H
#define QM_SPAN_H

#include "qm_vector_index.h"
#include "qm_mapu.h"

template<class, class, class> struct d_i;


template <class Id, class T, class ...Idxs>
struct d_i<Id,index_prod<Idxs...>,T>
{
    inline static constexpr auto element_type_name=my_static_string("element_type");
    inline static constexpr auto span_name=my_static_string("element_type");

    inline static constexpr auto name=eol_s+Id::name+arrow_s+index_prod<Idxs...>::name+arrow_s+T::name;


    constexpr auto element_type(Id)const { return Type<T>{};}
    constexpr auto span(Id)const {return index_prod<Idxs...>{};}

    constexpr auto operator[](Id)const {return *this;}

};

template <class, class> struct field_domain;

template < class ...Idxs,class... Id, class... T>
struct field_domain<index_prod<Idxs...>,x_i<Id,T>...>
{
    inline static constexpr auto element_type_name=my_static_string("element_type");
    inline static constexpr auto span_name=my_static_string("element_type");

    inline static constexpr auto name=eol_s+index_prod<Idxs...>::name+arrow_s+((obrk_s+Id::name+comma_s+T::name+cbrk_s+comma_s)+...);



    constexpr auto operator[](index_prod<Idxs...>)const {return *this;}

};



template<class... ts>
struct Domain: public ts...
{
    static inline constexpr auto domain_name=my_static_string("\nDomain{");
    static inline constexpr auto name=(domain_name+...+ts::name)+my_static_string("\n}");

    static constexpr bool is_empty(){return sizeof... (ts)==0;}

    template<class... Ids>
    constexpr auto fullspan(index_prod<Ids...>)const
    {
        return join(fullspan(Ids{})...,index_prod<Ids...>{});
    }
    constexpr auto fullspan(Nothing)const { return Nothing{};}


public:
    using ts::element_type...;
    using ts::span...;



    template<class Id> constexpr auto element_type(Id)const {return Nothing{};}
    template<class Id> constexpr auto span(Id)const {return Nothing{};}

    template<class Id> constexpr auto fullspan(Id)const {return fullspan(span(Id{}));}



    template<class ...ts2>
    friend constexpr auto join(Domain, Domain<ts2...>){return Domain<ts...,ts2...>{};}
};

template<class Id, class Value>
constexpr auto get_domain(x_i<Id,Value>const &) {return Domain<d_i<Id,index_prod<>,Value>>{};}

template<class... Id, class... Value, class... Indxs>
constexpr auto get_domain(v_f<index_prod<Indxs...>,vs<x_i<Id,Value>...>> const &)
{return Domain<d_i<Id,index_prod<Indxs...>,Value>...>{};}

template<class... Id, class... Value>
constexpr auto get_domain(vs<x_i<Id,Value>...> const &)
{return Domain<d_i<Id,index_prod<>,Value>...>{};}


template<class Id>
constexpr auto get_domain(ind<Id,ind_size>const &) {return Domain<d_i<Id,index_prod<>,ind_size>>{};}

template<class... Id, class... Value, class... Indxs>
constexpr auto get_domain(index_field<index_prod<Indxs...>,index_vector<ind<Id,ind_size>...>> const &)
{return Domain<d_i<Id,index_prod<Indxs...>,ind_size>...>{};}


template<class... Id, class... Value>
constexpr auto get_domain(index_vector<ind<Id,ind_size>...> const &)
{return Domain<d_i<Id,index_prod<>,ind_size>...>{};}


template<class... free_indexes,class... dependent_indexes>
constexpr auto get_domain(index_table<index_vector<free_indexes...>,dependent_indexes...>const& i)
{
    return join(get_domain(static_cast<index_vector<free_indexes...>const &>(i)),get_domain(static_cast<dependent_indexes const&>(i))...);
}


template<class... Indexes, class ...data>
constexpr auto get_domain(const mapu<index_table<Indexes...>,data...>& d)
{
    return join(get_domain(d.index()),get_domain(static_cast<data const&>(d))...);
}




#endif // QM_SPAN_H
