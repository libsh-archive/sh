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

template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
inline
ShMatrix<M, P, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, N, Binding, T1>& a,
          const ShMatrix<N, P, Binding2, T2>& b)
{
  return a | b;
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

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> operator|(const ShGeneric<M, T1>& a, const ShMatrix<M, N, Binding, T2>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2> ret;
  for (int i = 0; i < N; i++) {
    ret[i] = dot(a, b()(i));
  }
  return ret;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> operator*(const ShGeneric<M, T1>& a, const ShMatrix<M, N, Binding, T2>& b)
{
  return a | b;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
inline
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<N, T2>& b)
{
  return a | b;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<1, T2>& b)
{
  ShMatrix<M, N, SH_TEMP, CT1T2> r(a);
  r *= b;
  return r;
}

template<int M, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, 1, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, 1, Binding, T1>& a, const ShGeneric<1, T2>& b)
{
  ShMatrix<M, 1, SH_TEMP, CT1T2> r(a);
  r *= b;
  return r;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator*(const ShGeneric<1, T1>& a, const ShMatrix<M, N, Binding, T2>& b)
{
  ShMatrix<M, N, SH_TEMP, CT1T2> r(b);
  r *= a;
  return r;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<1, N, SH_TEMP, CT1T2>
operator*(const ShGeneric<1, T1>& a, const ShMatrix<1, N, Binding, T2>& b)
{
  ShMatrix<1, N, SH_TEMP, CT1T2> r(b);
  r *= a;
  return r;
}

template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator/(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<1, T2>& b)
{
  ShMatrix<M, N, SH_TEMP, CT1T2> r(a);
  r /= b;
  return r;
}



template<ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<1, 1, Binding, T>& matrix)
{
  return matrix[0][0];
}

  
template<ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<2, 2, Binding, T>& matrix)
{
  return (matrix[0][0]*matrix[1][1] - matrix[0][1] * matrix[1][0]);
}
 
template<int RowsCols, ShBindingType Binding, typename T>
ShAttrib1f
det(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  ShAttrib1f ret = 0.0;
  bool flip = (RowsCols % 2 == 0);

  for (int i = 0; i < RowsCols; i++) {
    if (flip) {
      ret -= matrix[RowsCols - 1][i] * det(matrix.subMatrix(RowsCols - 1, i));
    } else {
      ret += matrix[RowsCols - 1][i] * det(matrix.subMatrix(RowsCols - 1, i));
    }
    flip = !flip;
  }
  return ret;
}

//Matrix of Cofactors
template<ShBindingType Binding, typename T>
ShMatrix<1,1,SH_TEMP,T>
cofactors(const ShMatrix<1, 1, Binding, T>& matrix){
  return matrix;
}
    
template<ShBindingType Binding, typename T>
ShMatrix<2,2,SH_TEMP,T>
cofactors(const ShMatrix<2, 2, Binding, T>& matrix)
{
  ShMatrix<2,2,Binding,T> r;
  r.m_data[0][0]= matrix[1][1];
  r.m_data[1][0]=-matrix[0][1];
  r.m_data[0][1]=-matrix[1][0];
  r.m_data[1][1]= matrix[0][0];
    
  return r;
  //return matrix;
}
  
template<int RowsCols,ShBindingType Binding, typename T>
ShMatrix<RowsCols,RowsCols, SH_TEMP, T>
cofactors(const ShMatrix<RowsCols,RowsCols, Binding, T>& matrix)
{
  ShMatrix<RowsCols,RowsCols,Binding,T> r;

  for (int i = 0; i < RowsCols; i++) {
    for (int j = 0; j < RowsCols; j++) {
      if( (i+j)%2 ==0)	  
        r[i][j]= det(matrix.subMatrix(i,j));
      else
        r[i][j]=-det(matrix.subMatrix(i,j));
    }
  }
  return r;
}

//Transpose
template<int M, int N, ShBindingType Binding, typename T>
ShMatrix<N, M, SH_TEMP, T>
transpose(const ShMatrix<M, N, Binding, T>& matrix)
{    
  ShMatrix<N, M, SH_TEMP, T> r;
    
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {	  
      r[i][j]= matrix[j][i];	
    }
  }
    
  return r;
}

// Adjoint
template<int RowsCols, ShBindingType Binding, typename T>
ShMatrix<RowsCols, RowsCols, SH_TEMP, T>
adjoint(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  return transpose(cofactors(matrix));
}
  
//Inverse
template<int RowsCols, ShBindingType Binding, typename T>
ShMatrix<RowsCols,RowsCols, SH_TEMP, T>
inverse(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  return adjoint(matrix)/det(matrix);
}

template<int N, typename T>
ShMatrix<1, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0)
{
  ShMatrix<1, N, SH_TEMP, T> r;
  r[0] = s0;
  return r;
}

template<int N, typename T>
ShMatrix<2, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1)
{
  ShMatrix<2, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  return r;
}

template<int N, typename T>
ShMatrix<3, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2)
{
  ShMatrix<3, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  r[2] = s2;
  return r;
}


template<int N, typename T>
ShMatrix<4, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2,
       const ShGeneric<N, T>& s3)
{
  ShMatrix<4, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  r[2] = s2;
  r[3] = s3;
  return r;
}

template<int N, typename T>
ShMatrix<N, N, SH_TEMP, T>
diag(const ShGeneric<N, T>& a)
{
  ShMatrix<N, N, SH_TEMP, T> r;
  
  for (int i = 0; i < N; ++i) r[i][i] = a[i];
  return r;
}

}

#endif
