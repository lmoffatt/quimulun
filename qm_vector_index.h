#ifndef QM_VECTOR_INDEX_H
#define QM_VECTOR_INDEX_H


#include "qm_vector_device.h"
#include "qm_vector_basis.h"
#include "qm_vector_position.h"

template<class... Id>
class offset
{
private:
    std::size_t n_;

public:
    __host__ __device__
    constexpr offset(std::size_t n):n_{n}{}
    __host__ __device__
    constexpr auto operator()()const {return n_;}
    template<class... Id1>
    __host__ __device__
    friend constexpr offset<Id...,Id1...> operator*(offset one,offset<Id1...> two) {return one()*two();}
};





class ind_size
{
private:
    std::size_t n_;
public:

    inline static constexpr auto ind_name=my_static_string("ind_size");

    inline static constexpr auto name=ind_name;

    __host__ __device__   constexpr explicit ind_size(std::size_t n):n_{n}{}

    ind_size()=default;

    __host__ __device__  constexpr auto& size()const  {return n_;}

    __host__ __device__  constexpr auto& size()  {return n_;}

};


template <class , class > class ind;

template<class Id>
class ind<Id,ind_size> : public ind_size
{
public:

    inline static constexpr auto ind_name=my_static_string("ind(");

    inline static constexpr auto name=ind_name+Id::name+my_static_string(")");

    __host__ __device__   constexpr explicit ind(Id, std::size_t n):ind_size{n}{}

    __host__ __device__  constexpr auto& operator[](Id)const  {return *this;}

    __host__ __device__  constexpr auto& operator[](Id) {return *this;}


    constexpr ind()=default;
    using ind_size::size;

    template<class index_table, class Pos>
    __host__ __device__ constexpr auto operator()(const std::pair<const index_table&, const Pos&>& p)const {
        return std::pair<offset<Id>, const std::pair<const index_table&, const Pos&>&>(p[Id{}](),p);
    }

    template<class index_table,class... Id0,class Pos>
    __host__ __device__   constexpr auto operator()(std::pair<offset<Id0...>, const std::pair<const index_table&, const Pos&>&> p)const
    {
        return std::pair<offset<Id0...,Id>, const std::pair<const index_table&, const Pos&>&>(
                    p.first()*size()+ p.second.second[Id{}](),
                p.second);
    }
    friend std::ostream& operator<<(std::ostream& os, const ind& me){os<<me.size(); return os;}
    friend std::istream& operator>>(std::istream& is, ind& me){is>>me.size(); return is;}


};


template<class...>
class index_prod;

template<>
class index_prod<>
{
public:
    inline static constexpr auto name=my_static_string("span()");
    template<class... Idx2>
    friend  __host__ __device__ constexpr auto operator+(index_prod, index_prod<Idx2...>)
    {
        return index_prod<Idx2...>{};
    }

};

template<class Idx,class... Idxs>
class index_prod<Idx,Idxs...>
{


public:
    inline static constexpr auto span_name=my_static_string("span");

    inline static constexpr auto name=span_name+obrk_s+(Idx::name+...+(comma_s+Idxs::name))+my_static_string(")");

    __host__ __device__    constexpr index_prod(Idx, Idxs...){}

    __host__ __device__    constexpr index_prod()=default;

    template< class index_table>
    __host__ __device__    constexpr auto size(const index_table& itable)const {

        if constexpr ((!std::is_same_v<decltype (itable[Idx{}]),Nothing>&&...&& !std::is_same_v<decltype (itable[Idxs{}]),Nothing>))
                return (itable[Idx{}].size()*...*itable[Idxs{}].size());
        else
        return Nothing{};
    }

    template<class index_table,class Pos>
    __host__ __device__    constexpr auto operator()(const std::pair<index_table&,Pos&>& p)const {
        return compose(p,p.first[Idx{}],p.first[Idxs{}]...).first;
    }



    template<class... Idxs2>
    static __host__ __device__ constexpr auto sum_impl(index_prod<>, index_prod<Idxs2...>)
    {
        return index_prod<Idxs2...>{};
    }

    template<class Idx0, class... Idxs0>
    static __host__ __device__ constexpr auto sum_impl(index_prod<Idx0,Idxs0...>, index_prod<>)
    {
        return index_prod<Idx0,Idxs0...>{};
    }
    template<class... Idxs0,class Idx2, class... Idxs2>
    static __host__ __device__ constexpr auto sum_impl(index_prod<Idxs0...>, index_prod<Idx2,Idxs2...>)
    {
        if constexpr ((std::is_same_v<Idxs0,Idx2 >||...))
                return sum_impl(index_prod<Idxs0...>{},index_prod<Idxs2...>{});
        else
               return sum_impl(index_prod<Idxs0...,Idx2>{},index_prod<Idxs2...>{});

}


template<class... Idx2>
friend  __host__ __device__ constexpr auto operator+(index_prod, index_prod<Idx2...>)
{
    return sum_impl(index_prod{},index_prod<Idx2...>{});
}



};


template<class... >
struct index_vector;

template<class... Ids>
struct index_vector<ind<Ids,ind_size>...>:public ind<Ids,ind_size>...
{
    template<class Id>     constexpr auto operator[](Id){return Nothing{};}

    template<class index,class... indexs>
    static std::ostream& put(std::ostream& os,const index_vector<index,indexs...>& x)
    {
        os<<static_cast<index const&>(x);
        ((os<<sep<<static_cast<indexs const&>(x)),...);
        return os;

    }
    template<class ind0,class... inds>
    static std::ostream& put_titles(std::ostream& os,ind0,inds... )
    {
        os<<ind0::name;
        ((os<<sep<<inds::name),...);
        return os;

    }

    template<class... indexs, std::enable_if_t<sizeof... (indexs)==0>>
    static std::ostream& put(std::ostream& os,const index_vector<indexs...>& x)
    {
        return os;

    }
    template<class... indexs, std::enable_if_t<sizeof... (indexs)==0>>
    static std::ostream& put(std::ostream& os, typename index_vector<indexs...>::Titles x)
    {
        return os;

    }


    template<class index,class... indexs>
    static std::istream& get(std::istream& is, index_vector<index,indexs...>& x)
    {
        is>>static_cast<index &>(x);
        ((is>>sep>>static_cast<indexs &>(x)),...);
        return is;

    }

    template<class index,class... indexs>
    static std::istream& get_titles(std::istream& is, index,indexs...  )
    {
        is>>index::name;
        ((is>>sep>>indexs::name),...);
        return is;

    }


public:
    using ind<Ids,ind_size>::operator[]...;

    template<class Id>
    constexpr auto slice_impl(Id)
    {

        if constexpr (!(std::is_same_v<Id,Ids>||...||false))
                return Nothing{};
        else

          return index_vector<ind<Id,ind_size>>((*this)[Id{}]);
    }


    template<class Id>
    friend  constexpr auto join(index_vector&& me, index_vector<ind<Id,ind_size>>&& other)
    {
        return index_vector<ind<Ids,ind_size>...,ind<Id,ind_size>>{static_cast<ind<Ids,ind_size>&&>(me)...,static_cast<ind<Id,ind_size>&&>(other)};

    }


    static inline constexpr auto const sep=my_static_string(" ");

    struct Titles{
        friend std::ostream& operator<<(std::ostream& os,  Titles ){ return index_vector::put_titles(os,ind<Ids,ind_size>{}...);}
        friend std::istream& operator>>(std::istream& is,  Titles ) { return get_titles(is,ind<Ids,ind_size>{}...);}
    };

    friend class index_vector::Titles;

    static constexpr auto titles(){return Titles{};}


    friend std::ostream& operator<<(std::ostream& os, const index_vector& x)
    {
        return put(os,x);
    }

    friend std::istream& operator>>(std::istream& is,  index_vector& x)
    {
        return get(is,x);
    }


};

template <class,class> class index_field;
template<class...>
class index_table;

template<class I,class... Idxs>
class index_field<index_prod<Idxs...>,I>
{

private:
    vector_device<I> offset_;




public:
    constexpr auto& value(){return offset_;}

    constexpr   auto& value()const {return offset_;}

    static inline constexpr const auto sep=vector_device<I>::sep;

    template<class Id>
    class at_Id
    {
    private:
        index_field* v_;
    public :
        using my_field=index_field;


        template<class index_table,class...Xs>
        __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)
        { return v_->value()[(*v_)(x,p)()][Id{}];}

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[(*v_)(x,p)()][Id{}];}

at_Id(Id,index_field& v):v_(&v){}

};

template<class Id>
class at_Id_const
{
    index_field const* v_;
public:
    using my_field=index_field;

    template<class index_table,class...Xs>
    __host__ __device__ constexpr    decltype(auto) operator()(const index_table& x,const Position<Xs...>& p)const
    { return v_->value()[(*v_)(x,p)()][Id{}];}

at_Id_const(Id,index_field const& v):v_(&v){}

};


constexpr auto with_index(index_prod<Idxs...>)const& {return *this;}
constexpr auto with_index(index_prod<Idxs...>)&& {return std::move(*this);}


static constexpr auto index(){return index_prod<Idxs...>{};}

template<class Id>
constexpr auto slice_impl(Id)
{
    using T=std::decay_t<decltype (std::declval<I>().slice(Id{}))>;
    if constexpr (std::is_same_v<T,Nothing >)
            return Nothing{};
    else
    {
        auto out=index_field<index_prod<Idxs...>,T>(size());
        for (std::size_t i=0; i<size(); ++i)
            out.value()[i][Id{}]=value()[i][Id{}];
        return out;

    }
}

template<class Id, typename=decltype (std::declval<I>()[Id{}])>
constexpr  auto at(Id)
{
    return at_Id<Id>(*this);
}

template<class Id, typename=decltype (std::declval<I>()[Id{}])>
constexpr auto at(Id) const
{
    return at_Id_const<Id>(*this);
}




inline static constexpr auto indname=my_static_string("index_fieldepends_on(");
inline static constexpr auto name=indname+index_prod<Idxs...>::name+my_static_string(")");

template<class index_table>
__host__ __device__ constexpr index_field(const index_table& m, index_prod<Idxs...> i):offset_{i.size(m)}{}


__host__ __device__ constexpr index_field(std::size_t n):offset_{n}{}


__host__ __device__  auto size()const{return offset_.size();}

template<class index_table>
__host__ __device__  static auto size(const index_table& t){return index().size(t);}


template<class index_table, class Pos>
__host__ __device__  auto& operator[](const std::pair<const index_table&,const Pos&>& p)const  {return offset_[index_prod<Idxs...>{}(p)()];}



template<class Id, class... Idxs2>
friend auto join_when_same_index (index_field&& x, index_field<index_prod<Idxs2...>,index_vector<ind<Id,ind_size>>>&& y)
{
    if constexpr (!std::is_same_v<index_prod<Idxs...>,index_prod<Idxs2... >>)
            return x;
    else {
        using T=std::decay_t<decltype (join(std::declval<I>(),std::declval<index_vector<ind<Id,ind_size>>>()))>;
        if constexpr (std::is_same_v<T,I >)
                return x;
        else
        {
            index_field<index_prod<Idxs...>,T> out(x.size());
            for (std::size_t i=0; i<x.size(); ++i)
                out[i]=join(std::move(x.value()[i]), std::move(y.value()[i]));
            return out;

        }
    }
}





friend std::ostream& operator<<(std::ostream& os, const index_field& me)
{
    os<<typename I::Title{}<<sep;
    os<<me.value();
    return os;
}

friend std::istream& operator>>(std::istream& is, index_field& me)
{
    is>>typename I::Title{}>>sep;
    is>>me.value();
    return is;
}


};


//template<class Id,class... Idxs>
//class ind<Id,index_field<index_prod<Idxs...>>>
//{

//private:
//  index_field<index_prod<Idxs...>> i_;

//  auto& index(){return i_;}
//  auto& index()const {return i_;}

//  template<class, class>
//  struct index_offset;

//  template<class index_table, class T>
//  using index_offset_t=typename index_offset<index_table,T>::type;

//  template<class index_table,class Id1>
//  struct index_offset<index_table,ind<Id1,ind_size>>
//  {
//    using type=offset<Id1>;
//  };

//  template<class index_table,class Id1, class... Idxts>
//  struct index_offset<index_table,ind<Id1,index_field<index_prod<Idxts...>>>>
//  {
//    using type=decltype((index_offset_t<index_table,std::decay_t<decltype(std::declval<index_table>()[Idxts{}])>>{}*...*offset<Id1>{}));
//  };


//public:

//  inline static constexpr auto indname=my_static_string("index(");
//  inline static constexpr auto name=indname+Id::name+my_static_string(")");

//  template<class index_table>
//  __host__ __device__ constexpr ind(const index_table& m, index_prod<Idxs...> i):i_{m,i}{}

//  __host__ __device__  auto size()const{return index().size();}




//  template< class Pos>
//  __host__ __device__  auto& operator[](const Pos& p) {return i_[p];}
//  template< class Pos>
//  __host__ __device__  auto& operator[](const Pos& p)const  {return i_[p];}

//  __host__ __device__  auto& operator[](Id)const  {return *this;}

//  __host__ __device__  auto& operator[](Id) {return *this;}


//  template<class index_table,class Pos>
//  __host__ __device__  constexpr auto operator()(std::pair<const index_table&,const Pos&>  p)const {
//    return std::pair<index_offset_t<index_table,ind>, std::pair<const index_table&,const Pos&>>(i_[p]+p.second[Id{}](),p);
//  }


//  template<class... Id0,class index_table,class Pos>
//  __host__ __device__   constexpr auto operator()(std::pair<offset<Id0...>, std::pair<const index_table&,const Pos&>> p)const
//  {
//    return std::pair<decltype(offset<Id0...>{}*index_offset_t<index_table,ind>{}), std::pair<const index_table&,const Pos&>>(
//        p.first()*size()+(*this)(p.second),
//        p.second);
//  }

//};


template<class... free_indexes,class... dependent_indexes>
class index_table<index_vector<free_indexes...>,dependent_indexes...>
        : public index_vector<free_indexes...>, public dependent_indexes...
{
public:
    using free_ind=index_vector<free_indexes...>;

    using index_vector<free_indexes...>::operator[];
    using dependent_indexes::at...;
    using dependent_indexes::with_index...;
    template<class... Idxs>
    constexpr auto with_index(index_prod<Idxs...>){return Nothing{};}

    using index_vector<free_indexes...>::slice_impl;
    using dependent_indexes::slice_impl...;


    template<class Id>
    class at_Id
    {
    private:
        index_table * i_;

    public:
        at_Id(Id,const index_table& i):i_{&i}{}
        auto slice()const
        {

        }



    };




    template<class other_Index>
    constexpr auto at(other_Index)const {return Nothing{};}

    template<class an_Index>
    constexpr auto operator[](an_Index)const {
        using T=decltype (at(an_Index{}));
        if constexpr (std::is_same_v<T,Nothing >)
                return Nothing{};
        else if constexpr (std::is_same_v<T,ind<an_Index,ind_size> >)
                return at(an_Index{});
        else
        return at_Id(an_Index{},*this);
    }
    template<class otherId>
    constexpr auto slice_impl(otherId)const {return Nothing{};}



    template<class Id>
    constexpr auto slice(Id)const
    {
        return fill_slice(slice_impl(Id{}));
    }





    template<class Id>
    constexpr auto fill_slice(index_vector<ind<Id,ind_size>>&& x)const
    {
        return index_table<index_vector<ind<Id,ind_size>>>(std::move(x));
    }

    template<class ...Idx, class I>
    constexpr auto fill_slice(index_field<index_prod<Idx...>,I>&& x)const
    {
        return join(slice(Idx{})...,std::move(x));
    }

    constexpr auto fill_slice(Nothing)const
    {return Nothing{};}


    constexpr  index_table(index_vector<free_indexes...> f_i, dependent_indexes...di):index_vector<free_indexes...>{f_i},dependent_indexes{di}...{}
    constexpr  index_table()=default;

    template<class new_dependen_index>
    friend constexpr  auto operator +(index_table&& me, new_dependen_index&& other)
    {
        return index_table<index_vector<free_indexes...>,dependent_indexes...,new_dependen_index>(
                    static_cast<free_ind&&>(me),static_cast<dependent_indexes&&>(me)...,std::move(other));

    }

    template<class Id>
    friend constexpr auto join(index_table&& me, index_table<index_vector<ind<Id,ind_size>>>&& i)
    {
        using T=decltype (me.slice(Id{}));
        if constexpr (std::is_same_v<Nothing,T >)
        {
            return index_table<index_vector<free_indexes...,ind<Id,ind_size>>,dependent_indexes...>{
                join (static_cast<index_vector<free_indexes...>&&>(me),std::move(i)),
                        static_cast<dependent_indexes&&>(me)...
            };
        }
        else
        return me;
    }

    template<class ...Idx, class I>
    friend constexpr auto join(index_table&& x,index_field<index_prod<Idx...>,I>&& other)
    {
        using T=decltype ( x.with_index(index_prod<Idx...>{}));
        if constexpr (std::is_same_v<Nothing, T>)
                return index_table<index_vector<free_indexes...>,dependent_indexes...,index_field<index_prod<Idx...>,I>>(
                    static_cast<free_ind&&>(x),static_cast<dependent_indexes&&>(x)...,std::move(other));
        else
        return index_table<index_vector<free_indexes...>,
                decltype(join_when_same_index(std::declval<dependent_indexes&&>(),std::declval<index_field<index_prod<Idx...>,I>&&>()))
                ...>{
            static_cast<index_vector<free_indexes...>&&>(x),
                    join_when_same_index(static_cast<dependent_indexes&&>(x),std::move(other))...};

    }

};








#endif // QM_VECTOR_INDEX_H
