#ifndef QM_MAPU_H
#define QM_MAPU_H

#include "qm_vector_index.h"
#include "qm_vector_space.h"
#include "qm_vector_field.h"
#include "qm_base.h"

template<class...> class mapu;




template<class... Indexes>
class mapu<index_table<Indexes...>>
{
private:
    index_table<Indexes...> i_;

public:

    __host__ __device__ constexpr auto at(...)const
    {
        return Nothing{};
    }



    template<class Id>
    __host__ __device__ constexpr auto operator[](Id)const{
        return Nothing{};
    }


    __host__ __device__ constexpr auto& index()const{ return i_;}

    __host__ __device__ constexpr auto& index(){ return i_;}

    __host__ __device__ constexpr mapu(index_table<Indexes...>&& i ): i_{std::move(i)}{}


    constexpr mapu()=default;


    template<class newdata>
    friend auto operator+(mapu&& m,newdata&& d)
    {
        return mapu<index_table<Indexes...>,newdata>(std::move(m.index()),std::move(d));
    }




    constexpr auto slice(...)const {return Nothing{};}









};



template<class... Indexes, class ...data>
class mapu<index_table<Indexes...>,data...>: public data...
{
private:
    index_table<Indexes...> i_;

public:
    template<class Id>
    __host__ __device__ constexpr auto  at_impl(Id,Nothing)const {return Nothing{};}

    template<class Id,class Value>
    __host__ __device__ constexpr auto&  at_impl(Id,const x_i<Id,Value>& x)const {return x;}

    template<class Id,class Value>
    __host__ __device__ constexpr auto&  at_impl(Id,x_i<Id,Value>& x){return x;}

    template<class Id,class Value_type,class...  Idxs>
    __host__ __device__ constexpr auto  at_impl(Id,const v_f<index_prod<Idxs...>,Value_type>& x)const
    {
        return x_i_view(Id{}, index(),x);
    };


    template<class Id,class Value_type,class...  Idxs>
    __host__ __device__ constexpr auto  at_impl(Id, v_f<index_prod<Idxs...>,Value_type>& x)
    {
        return x_i_view(Id{}, index(),x);
    };

    template<class Id, class...Xs>
    __host__ __device__ constexpr auto  at_impl(Id,const Position<Xs...>& ,Nothing)const {return Nothing{};}

    template<class Id, class...Xs,class Value>
    __host__ __device__ constexpr auto&  at_impl(Id,const Position<Xs...>& ,const x_i<Id,Value>& x)const {return x;}

    template<class Id, class...Xs,class Value>
    __host__ __device__ constexpr auto&  at_impl(Id,const Position<Xs...>& , x_i<Id,Value>& x) {return x;}

    template<class Id, class...Xs,class Value_type,class...  Idxs>
    __host__ __device__ constexpr auto&  at_impl(Id,const Position<Xs...>& pos,const v_f<index_prod<Idxs...>,Value_type>& x)const
    {
        return x.value()[x.index()(index(),pos)()][Id{}];
}


template<class Id, class...Xs,class Value_type,class...  Idxs>
__host__ __device__ constexpr auto&  at_impl(Id,const Position<Xs...>& pos, v_f<index_prod<Idxs...>,Value_type>& x)
{
    return x.value()[x.index()(index(),pos)()][Id{}];
}




template<class Id>
__host__ __device__ constexpr decltype(auto) operator[](Id)const{
    return at_impl(Id{},join(data::operator[](Id{})...));
}

template<class Id>
__host__ __device__ constexpr decltype(auto) operator[](Id){
    return at_impl(Id{},join(data::operator[](Id{})...));
}

template<class Id, class...Xs>
__host__ __device__ constexpr decltype(auto) operator()(Id, const Position<Xs...>& pos)const{
    return at_impl(Id{},pos,join(data::operator[](Id{})...));
}

template<class Id, class...Xs>
__host__ __device__ constexpr decltype(auto) operator()(Id, const Position<Xs...>& pos){
    return at_impl(Id{},pos,join(data::operator[](Id{})...));
}


__host__ __device__ constexpr auto& index()const{ return i_;}

__host__ __device__ constexpr auto& index(){ return i_;}

__host__ __device__ constexpr mapu(index_table<Indexes...>&& i ,data&& ...d): data{std::move(d)}...,i_{std::move(i)}{}

__host__ __device__ constexpr mapu(index_table<Indexes...> const& i ,data&& ...d): data{std::move(d)}...,i_{i}{}

constexpr mapu()=default;


template<class newdata>
friend auto operator+(mapu&& m,newdata&& d)
{
    return mapu<index_table<Indexes...>,data...,newdata>(std::move(m.index()),static_cast<data&&>(m)...,std::move(d));
}



template<class indextable,class... newdata>
friend auto join(mapu&& m,mapu<indextable,newdata...>&& d)
{
    using newTable=decltype (join(std::move(m.index()),std::move(d.index())));
    return mapu<newTable,data...,newdata...>(join(std::move(m.index()),std::move(d.index())),
                                                         static_cast<data&&>(m)...,static_cast<newdata&&>(d)...);
}

};

template<class... Indexes, class ...data>
mapu(index_table<Indexes...>,data...)->mapu<index_table<Indexes...>,data...>;




#endif // QM_MAPU_H
