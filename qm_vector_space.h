#ifndef QM_VECTOR_SPACE_H
#define QM_VECTOR_SPACE_H

#include "qm_vector_basis.h"


template<class... xs>
struct vs :public xs...
{
private:
  template<class x0, class...x2>
  static std::ostream& put(std::ostream& os, const x0& x, const x2&...xss)
  {
    os<<x;
    ((os<<sep<<xss),...);
    return os;
  }
  template<class x0, class...x2>
  static std::istream& get(std::istream& is,  x0& x,  x2&...xss)
  {
    is>>x;
    ((is>>sep>>xss),...);
    return is;
  }

public:



    static inline constexpr const auto sep=my_static_string("\t");

    static constexpr auto titles(){return ((xs::title()+sep)+...);}

   using xs::operator[]...;

    __host__ __device__ constexpr explicit vs(xs&&... x):xs{std::move(x)}...{}

    __host__ __device__ constexpr vs()=default;


    friend std::ostream& operator<<(std::ostream& os, const vs& x)
    {
        return   put(os,static_cast<xs const&>(x)()...);
    }

    friend std::istream& operator>>(std::istream& is, vs& x)
    {
      return   get(is,static_cast<xs &>(x)()...);
    }



};


#endif // QM_VECTOR_SPACE_H
