#ifndef SHLIBMATRIXIMPL_HPP
#define SHLIBMATRIXIMPL_HPP

#include "ShLibMatrix.hpp"

namespace SH {

template<int Rows, int Cols, ShBindingType Binding, typename T>
inline
ShMatrix<Cols, Rows, SH_TEMP, T>
transpose(const ShMatrix<Rows, Cols, Binding, T>& m)
{
  ShMatrix<Cols, Rows, SH_TEMP, T> result;
  for (int i = 0; i < Cols; i++) {
    for (int j = 0; j < Rows; j++) {
      result[i][j] = m[j][i];
    }
  }
  return result;
}

template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, 
  typename T1, typename T2>
inline
ShMatrix<M, P, SH_TEMP, CT1T2>
operator|(const ShMatrix<M, N, Binding, T1>& a,
          const ShMatrix<N, P, Binding2, T2>& b)
{
  ShMatrix<P, N, SH_TEMP, T2> tb = transpose(b);

  ShMatrix<M, P, SH_TEMP, CT1T2> result;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < P; j++) {
      result[i][j] = dot(a[i], tb[j]);
    }
  }
  return result;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
inline
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding, T1>& a, 
    const ShGeneric<N, T2>& b)
{
  ShAttrib<M, SH_TEMP, CT1T2> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

}

#endif
