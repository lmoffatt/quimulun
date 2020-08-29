#ifndef QM_FILE_IO_H
#define QM_FILE_IO_H

#include "qm_mapu.h"
#include "qm_vector_space.h"
#include "qm_vector_field.h"

#include <fstream>


struct input_mode{};
struct create_mode{};

struct append_mode{};


template <class> class file_stream;

template<>
class file_stream<append_mode>
{
private:
    std::string fname_;
    std::ofstream f_;
public:
    file_stream(std::string fname):fname_{std::move(fname)},f_{fname_,std::ios::app}{}
    auto& operator()(){return f_;}

};

template<>
class file_stream<create_mode>
{
private:
    std::string fname_;
    std::ofstream f_;
public:
    file_stream(std::string fname):fname_{std::move(fname)},f_{fname_}{}
    auto& operator()(){return f_;}

};

template<>
class file_stream<input_mode>
{
private:
    std::string fname_;
    std::ifstream f_;
public:
    file_stream(std::string fname):fname_{std::move(fname)},f_{fname_}{}
    auto& operator()(){return f_;}

};

template<class...> class file_IO;





template<class... Indexes>
class file_IO<index_vector<Indexes...>>
{
private:
    std::string fname_;

public:
    static constexpr auto titles(){return index_vector<Indexes...>::titles();}
    static constexpr inline auto const line_sep=my_static_string("\n");


    using type=index_vector<Indexes...>;

    file_IO(std::string fname): fname_{fname+index_vector<Indexes...>::name.c_str()}{
    }

    friend void operator<<(file_IO&& fo, const index_vector<Indexes...>& x)
    {
        std::ofstream f(fo.fname_);
        f<<titles()<<line_sep;
        f<<x<<line_sep;
    }

    friend auto extract(file_IO&& fo)
    {
        type x;
        std::ifstream f(fo.fname_);
        f>>titles()>>line_sep;
        f>>x>>line_sep;
        return x;
    }

};

template<class I,class... Idxs>
class file_IO<index_field<index_prod<Idxs...>,I>>
{
private:
    std::string fname_;

public:
    static constexpr auto titles(){return I::titles();}
    static constexpr inline auto const line_sep=my_static_string("\n");

    using type=index_field<index_prod<Idxs...>,I>;

    file_IO(std::string fname): fname_{fname+index_field<index_prod<Idxs...>,I>::name.c_str()}{
    }

    friend void operator<<(file_IO&& fo, const index_field<index_prod<Idxs...>,I>& x)
    {
        std::ofstream f(fo.fname_);
        f<<x;
    }

    template<class rootIndex,class... Indexes>
    friend auto extract(file_IO&& fo, index_table<rootIndex,Indexes...>&&t)
    {
        constexpr auto n=index_prod<Idxs...>{}.size(t);
        if constexpr (!is_Nothing(n))
        {
         std::ifstream f(fo.fname_);
         type x(n);
          f>>x;
          return std::move(t)+std::move(x);
        } else
        return Nothing{};
    }
};



template<class rootIndex,class... Indexes>
class file_IO<index_table<rootIndex,Indexes...>>: public file_IO<rootIndex>, public file_IO<Indexes>...
{
public:
    file_IO(std::string fname): file_IO<rootIndex>{fname},file_IO<Indexes>{fname}...{}

    friend void operator<<(file_IO&& fo, const index_table<rootIndex,Indexes...>& x)
    {
        static_cast<file_IO<rootIndex>&&>(fo)<<static_cast<rootIndex const&>(x);
        ((static_cast<file_IO<Indexes>&&>(fo)<<static_cast<Indexes const &>(x)),...);
    }

    friend auto extract(file_IO&& fo)
    {
        auto root=index_table<rootIndex>(extract(static_cast<file_IO<rootIndex>>(fo)));
        return compose(root,[&f=static_cast<file_IO<Indexes>&>(fo)](auto&& index){return extract(f,std::move(index));} ...);
    }

};



template<class... xs>
class file_IO<vs<xs...>>
{
private:
    std::string fname_;

public:
    static constexpr auto titles(){return type::titles();}
    static constexpr inline auto const line_sep=my_static_string("\n");


    using type=vs<xs...>;

    file_IO(std::string fname): fname_{fname+type::name.c_str()}{
    }

    friend void operator<<(file_IO&& fo, const type& x)
    {
        std::ofstream f(fo.fname_);
        f<<titles()<<line_sep;
        f<<x<<line_sep;
    }

    friend auto extract(file_IO&& fo)
    {
        type x;
        std::ifstream f(fo.fname_);
        f>>titles()>>line_sep;
        f>>x>>line_sep;
        return x;
    }


};




template<class Value_type,class...  Idxs>
class file_IO<v_f<index_prod<Idxs...>,Value_type>>
{
private:
    std::string fname_;

public:
    static constexpr auto titles(){return Value_type::titles();}
    static constexpr inline auto const line_sep=my_static_string("\n");


    using type=v_f<index_prod<Idxs...>,Value_type>;

    file_IO(std::string fname): fname_{fname+type::name_ext.c_str()}{
    }

    friend void operator<<(file_IO&& fo, const type& x)
    {
        std::ofstream f(fo.fname_);
        f<<titles()<<line_sep;
        f<<x.value()<<line_sep;
    }

    template<class index_table>
    friend auto extract(file_IO&& fo, const index_table& t)
    {
        type x(t);
        std::ifstream f(fo.fname_);
        f>>titles()>>line_sep;
        f>>x.value();
        return x;
    }


};




template<class... Indexes, class ...data>
class file_IO<mapu<index_table<Indexes...>,data...>>: public file_IO<index_table<Indexes...>>,public file_IO<data>...
{
public:
    using type=mapu<index_table<Indexes...>,data...>;

    file_IO(std::string fname): file_IO<index_table<Indexes...>>{fname},file_IO<data>{fname}...{}

    friend void operator<<(file_IO&& fo, const type& x)
    {
        static_cast<file_IO<index_table<Indexes...>>&&>(fo)<<x.index();
        ((static_cast<file_IO<data>&&>(fo)<<static_cast<data const &>(x)),...);
    }

    friend auto extract(file_IO&& fo)
    {

        auto ind=extract(static_cast<file_IO<index_table<Indexes...>>>(fo));

        return type(std::move(ind),extract(static_cast<file_IO<data>>(fo),ind)...);
    }

};




#endif // QM_FILE_IO_H
