#ifndef SHLIBMISCIMPL_HPP
#define SHLIBMISCIMPL_HPP

#include "ShLibMisc.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int M, int N, typename T> 
inline
ShGeneric<M, T> cast(const ShGeneric<N, T>& a)
{
  int copySize = std::min(M, N);
  ShAttrib<M, SH_TEMP, T> result;

  int indices[copySize];
  for(int i = 0; i < copySize; ++i) indices[i] = i;
  if(M < N) {
    result = a.template swiz<M>(indices);
  } else if( M > N ) {
    result.template swiz<N>(indices) = a;
  } else { // M == N
    shASN(result, a);
  }
  return result;
}

template<int M> 
inline
ShGeneric<M, float> cast(float a)
{
  return cast<M>(ShAttrib<1, SH_CONST, float>(a));
}

template<int M, int N, typename T> 
inline
ShGeneric<M, T> fillcast(const ShGeneric<N, T>& a)
{
  if( M <= N ) return cast<M>(a);
  int indices[M];
  for(int i = 0; i < M; ++i) indices[i] = i >= N ? N - 1 : i;
  return a.template swiz<M>(indices);
}

template<int M> 
inline
ShGeneric<M, float> fillcast(float a)
{
  return fillcast<M>(ShAttrib<1, SH_CONST, float>(a));
}

template<int N, typename T>
inline
void kill(const ShGeneric<N, T>& c)
{
  shKIL(c);
}

}

#endif
