#ifndef QM_SUMMARIZE_H
#define QM_SUMMARIZE_H
#include <qm_tensor_model.h>



template<class Id,class G, class Input, class Index>
class  S;

template<class Id,class G, class... Input, class... Index>
class  S<Id,G,Cs<Input...>,Cs<Index...>>

{
private:
  G g_;
public:
  typedef   Id myId;
  auto &operator[](Id)const {return *this;}

  template<class... Param>
  auto operator()(const Param&... par)const
  {
    //    using test=typename Id::myID;
    //    using test2=typename Cs<Xs...>::myXs;

    auto p=Position<Index...>{}
    do
          {
    auto  out=apply(g_,get_from(Input{},par...)(p)...);

    return x_i(Id{},std::move(out));
  }
  S(Id ,G&& g, Cs<Input...>,Cs<Index...>):g_{std::move(g)}{}
};





#endif // QM_SUMMARIZE_H
