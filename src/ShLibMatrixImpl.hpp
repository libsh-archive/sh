// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
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

template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T>
inline
ShMatrix<M, P, SH_TEMP, T>
operator|(const ShMatrix<M, N, Binding, T>& a,
          const ShMatrix<N, P, Binding2, T>& b)
{
  ShMatrix<P, N, SH_TEMP, T> tb = transpose(b);

  ShMatrix<M, P, SH_TEMP, T> result;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < P; j++) {
      result[i][j] = dot(a[i], tb[j]);
    }
  }
  return result;
}

template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T>
inline
ShMatrix<M, P, SH_TEMP, T>
operator*(const ShMatrix<M, N, Binding, T>& a,
          const ShMatrix<N, P, Binding2, T>& b)
{
  return a | b;
}

template<int M, int N, ShBindingType Binding, typename T>
inline
ShGeneric<M, T> operator|(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<N, T>& b)
{
  ShAttrib<M, SH_TEMP, T> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

template<int M, int N, ShBindingType Binding, typename T>
inline
ShGeneric<N, T> operator|(const ShGeneric<M, T>& a, const ShMatrix<M, N, Binding, T>& b)
{
  ShAttrib<N, SH_TEMP, T> ret;
  for (int i = 0; i < N; i++) {
    ret[i] = dot(a, b()(i));
  }
  return ret;
}

template<int M, int N, ShBindingType Binding, typename T>
inline
ShGeneric<N, T> operator*(const ShGeneric<M, T>& a, const ShMatrix<M, N, Binding, T>& b)
{
  return a | b;
}

template<int M, int N, ShBindingType Binding, typename T>
inline
ShGeneric<M, T> operator*(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<N, T>& b)
{
  return a | b;
}

}

#endif
