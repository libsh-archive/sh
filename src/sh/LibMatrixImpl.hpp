// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLIBMATRIXIMPL_HPP
#define SHLIBMATRIXIMPL_HPP

#include "LibMatrix.hpp"

namespace SH {

#define MATRIX_MSVC_OPERATOR_HACK(op, semantic, N, NminusOne) \
template<int M, BindingType Binding1, BindingType Binding2, typename T1, typename T2, bool swizzled> \
Attrib<NminusOne, SH_TEMP, CT1T2, semantic> \
op (const Matrix<M, N, Binding1, T1>& a, const Attrib<NminusOne, Binding2, T2, semantic, swizzled>& b) \
{ \
  return op<M, N, Binding1, Binding2, T1, T2, swizzled>(a, b); \
}

MATRIX_MSVC_OPERATOR_HACK(operator|, SH_VECTOR, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_VECTOR, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_VECTOR, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_VECTOR, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_PLANE, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_PLANE, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_PLANE, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_PLANE, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_POINT, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_POINT, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_POINT, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_POINT, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_TEXCOORD, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_TEXCOORD, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_TEXCOORD, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_TEXCOORD, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_VECTOR, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator|, SH_VECTOR, 3, 2)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_VECTOR, 4, 3)
MATRIX_MSVC_OPERATOR_HACK(operator*, SH_VECTOR, 3, 2)

template<int M, int N, BindingType Binding, BindingType Binding2, typename T1, typename T2>
Matrix<N, M, SH_TEMP, CT1T2>
operator+(const Matrix<N, M, Binding, T1>& a, 
	  const Matrix<N, M, Binding2, T2>& b)
{
  Matrix<N, M, SH_TEMP, CT1T2> r(a);
  r += b;
  return r;
}

template<int M, int N, BindingType Binding, BindingType Binding2, typename T1, typename T2>
Matrix<N, M, SH_TEMP, CT1T2>
operator-(const Matrix<N, M, Binding, T1>& a, 
	  const Matrix<N, M, Binding2, T2>& b)
{
  Matrix<N, M, SH_TEMP, CT1T2> r(a);
  r -= b;
  return r;
}

template<int M, int N, BindingType Binding, BindingType Binding2, typename T1, typename T2>
Matrix<N, M, SH_TEMP, CT1T2>
operator/(const Matrix<N, M, Binding, T1>& a, 
	  const Matrix<N, M, Binding2, T2>& b)
{
  Matrix<N, M, SH_TEMP, CT1T2> r(a);
  r /= b;
  return r;
}

template<int M, int N1, int N2, int P, BindingType Binding, BindingType Binding2, typename T1, typename T2>
Matrix<M, P, SH_TEMP, CT1T2>
operator|(const Matrix<M, N1, Binding, T1>& a,
          const Matrix<N2, P, Binding2, T2>& b)
{
  Matrix<P, N2, SH_TEMP, T2> tb = transpose(b);
  Matrix<M, P, SH_TEMP, CT1T2> result;

  if (N1 > N2) {
    // Matrix b must be expanded
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < P; j++) {
	Attrib<1, SH_TEMP, CT1T2> sum = a[i][0] * tb[j][0];
	for (int k = 1; k < N2; k++) {
	  sum += a[i][k] * tb[j][k];
	}
	for (int k = N2; k < N1; k++) {
	  if (k == j) sum += a[i][k];
	}
	result[i][j] = sum;
      }
    }
  } else if (N2 > N1) {
    // Matrix a must be expanded
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < P; j++) {
	Attrib<1, SH_TEMP, CT1T2> sum = a[i][0] * tb[j][0];
	for (int k = 1; k < N1; k++) {
	  sum += a[i][k] * tb[j][k];
	}
	for (int k = N1; k < N2; k++) {
	  if (k == i) sum += tb[j][k];
	}
	result[i][j] = sum;
      }
    }
  } else {
    // Sizes are compatible, no expansion
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < P; j++) {
	result[i][j] = dot(a[i], tb[j]);
      }
    }
  }
  return result;
}

template<int M, int N1, int N2, int P, BindingType Binding, BindingType Binding2, typename T1, typename T2>
inline
Matrix<M, P, SH_TEMP, CT1T2>
operator*(const Matrix<M, N1, Binding, T1>& a,
          const Matrix<N2, P, Binding2, T2>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Generic<M, CT1T2> operator|(const Matrix<M, N, Binding, T1>& a, const Generic<N, T2>& b)
{
  Attrib<M, SH_TEMP, CT1T2> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Generic<N, CT1T2> operator|(const Generic<M, T1>& a, const Matrix<M, N, Binding, T2>& b)
{
  Attrib<N, SH_TEMP, CT1T2> ret;
  for (int i = 0; i < N; i++) {
    ret[i] = 0;
    for (int j=0; j < M; j++) {
      ret[i] += a[j] * b[j][i];
    }
  }
  return ret;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
inline
Generic<N, CT1T2> operator*(const Generic<M, T1>& a, const Matrix<M, N, Binding, T2>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
inline
Generic<M, CT1T2> operator*(const Matrix<M, N, Binding, T1>& a, const Generic<N, T2>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
Attrib<N-1, SH_TEMP, CT1T2, SH_VECTOR>
operator|(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b)
{
  Attrib<M, SH_TEMP, CT1T2, SH_VECTOR> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(cast<N-1>(a[i]), b);
  }
  return cast<N-1>(ret);
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
Attrib<N-1, SH_TEMP, CT1T2, SH_VECTOR>
operator|(const Matrix<M, N, Binding1, T1>& a, 
          const Attrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b)
{
  Attrib<M, SH_TEMP, CT1T2, SH_VECTOR> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(cast<N-1>(a[i]), b);
  }
  return cast<N-1>(ret);
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
inline
Attrib<N-1, SH_TEMP, CT1T2, SH_VECTOR>
operator*(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
inline
Attrib<N-1, SH_TEMP, CT1T2, SH_VECTOR>
operator*(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding1, BindingType Binding2,
         typename T1, typename T2, bool swizzled>
Attrib<N-1, SH_TEMP, CT1T2, SH_TEXCOORD>
operator|(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_TEXCOORD, swizzled>& b)
{
  Attrib<N, SH_TEMP, T2, SH_TEXCOORD, swizzled> tmp = cast<N>(b);
  tmp[N-1] = static_cast<T2>(1);
  return cast<N-1>(a | tmp);
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
Attrib<N-1, SH_TEMP, CT1T2, SH_POINT>
operator|(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_POINT, swizzled>& b)
{
  Attrib<N, SH_TEMP, T2, SH_POINT, swizzled> tmp = cast<N>(b);
  tmp[N-1] = static_cast<T2>(1);
  return cast<N-1>(a | tmp);
}

template<int M, int N, BindingType Binding1, BindingType Binding2,
         typename T1, typename T2, bool swizzled>
Attrib<N-1, SH_TEMP, CT1T2, SH_PLANE>
operator|(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_PLANE, swizzled>& b)
{
  Attrib<N, SH_TEMP, T2, SH_PLANE, swizzled> tmp = cast<N>(b);
  tmp[N-1] = static_cast<T2>(1);
  return cast<N-1>(a | tmp);
}

template<int M, int N, BindingType Binding1, BindingType Binding2,
         typename T1, typename T2, bool swizzled>
inline
Attrib<N-1, SH_TEMP, CT1T2, SH_TEXCOORD>
operator*(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_TEXCOORD, swizzled>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding1, BindingType Binding2, 
         typename T1, typename T2, bool swizzled>
inline
Attrib<N-1, SH_TEMP, CT1T2, SH_POINT>
operator*(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_POINT, swizzled>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding1, BindingType Binding2,
         typename T1, typename T2, bool swizzled>
inline
Attrib<N-1, SH_TEMP, CT1T2, SH_PLANE>
operator*(const Matrix<M, N, Binding1, T1>& a,
          const Attrib<N-1, Binding2, T2, SH_PLANE, swizzled>& b)
{
  return a | b;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Matrix<M, N, SH_TEMP, CT1T2>
operator*(const Matrix<M, N, Binding, T1>& a, const Generic<1, T2>& b)
{
  Matrix<M, N, SH_TEMP, CT1T2> r(a);
  r *= b;
  return r;
}

template<int M, BindingType Binding, typename T1, typename T2>
Matrix<M, 1, SH_TEMP, CT1T2>
operator*(const Matrix<M, 1, Binding, T1>& a, const Generic<1, T2>& b)
{
  Matrix<M, 1, SH_TEMP, CT1T2> r(a);
  r *= b;
  return r;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Matrix<M, N, SH_TEMP, CT1T2>
operator*(const Generic<1, T1>& a, const Matrix<M, N, Binding, T2>& b)
{
  Matrix<M, N, SH_TEMP, CT1T2> r(b);
  r *= a;
  return r;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Matrix<1, N, SH_TEMP, CT1T2>
operator*(const Generic<1, T1>& a, const Matrix<1, N, Binding, T2>& b)
{
  Matrix<1, N, SH_TEMP, CT1T2> r(b);
  r *= a;
  return r;
}

template<int M, int N, BindingType Binding, typename T1, typename T2>
Matrix<M, N, SH_TEMP, CT1T2>
operator/(const Matrix<M, N, Binding, T1>& a, const Generic<1, T2>& b)
{
  Matrix<M, N, SH_TEMP, CT1T2> r(a);
  r /= b;
  return r;
}



template<BindingType Binding, typename T>
inline
Attrib1f
det(const Matrix<1, 1, Binding, T>& matrix)
{
  return matrix[0][0];
}

  
template<BindingType Binding, typename T>
Attrib1f
det(const Matrix<2, 2, Binding, T>& matrix)
{
  return (matrix[0][0]*matrix[1][1] - matrix[0][1] * matrix[1][0]);
}

template<BindingType Binding, typename T>
Attrib1f
det(const Matrix<3, 3, Binding, T>& matrix)
{
  return ((matrix[0] * matrix[1](1,2,0)) | matrix[2](2,0,1)) - 
         ((matrix[0] * matrix[1](2,0,1)) | matrix[2](1,2,0));
}
 
template<int RowsCols, BindingType Binding, typename T>
Attrib1f
det(const Matrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  Attrib1f ret = 0.0;
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
template<BindingType Binding, typename T>
inline
Matrix<1, 1, SH_TEMP, T>
cofactors(const Matrix<1, 1, Binding, T>& matrix)
{
  return matrix;
}
    
template<BindingType Binding, typename T>
Matrix<2, 2, SH_TEMP, T>
cofactors(const Matrix<2, 2, Binding, T>& matrix)
{
  Matrix<2, 2, Binding, T> r;
  r[0][0] = matrix[1][1];
  r[1][0] = -matrix[0][1];
  r[0][1] = -matrix[1][0];
  r[1][1] = matrix[0][0];
  return r;
}
  
template<int RowsCols, BindingType Binding, typename T>
Matrix<RowsCols, RowsCols, SH_TEMP, T>
cofactors(const Matrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  Matrix<RowsCols, RowsCols, Binding, T> r;

  for (int i = 0; i < RowsCols; i++) {
    for (int j = 0; j < RowsCols; j++) {
      if((i+j) % 2 == 0) {
        r[i][j] = det(matrix.subMatrix(i,j));
      } else {
        r[i][j] = -det(matrix.subMatrix(i,j));
      }
    }
  }
  return r;
}

//Transpose
template<int M, int N, BindingType Binding, typename T>
Matrix<N, M, SH_TEMP, T>
transpose(const Matrix<M, N, Binding, T>& matrix)
{    
  Matrix<N, M, SH_TEMP, T> r;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {	  
      r[i][j] = matrix[j][i];	
    }
  }
    
  return r;
}

// Adjoint
template<int RowsCols, BindingType Binding, typename T>
Matrix<RowsCols, RowsCols, SH_TEMP, T>
adjoint(const Matrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  return transpose(cofactors(matrix));
}
  
//Inverse
template<int RowsCols, BindingType Binding, typename T>
Matrix<RowsCols, RowsCols, SH_TEMP, T>
inverse(const Matrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  return adjoint(matrix)/det(matrix);
}

template<int RowsCols, BindingType Binding, typename T>
Generic<1, T>
trace(const Matrix<RowsCols, RowsCols, Binding, T>& matrix)
{
  Attrib<1, SH_TEMP, T> r(matrix[0][0]);
  for (int i=1; i < RowsCols; i++) {
    r += matrix[i][i];
  }
  return r;
}

template<int N, typename T>
Matrix<1, N, SH_TEMP, T>
rowmat(const Generic<N, T>& s0)
{
  Matrix<1, N, SH_TEMP, T> r;
  r[0] = s0;
  return r;
}

template<int N, typename T>
Matrix<2, N, SH_TEMP, T>
rowmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1)
{
  Matrix<2, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  return r;
}

template<int N, typename T>
Matrix<3, N, SH_TEMP, T>
rowmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1,
       const Generic<N, T>& s2)
{
  Matrix<3, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  r[2] = s2;
  return r;
}


template<int N, typename T>
Matrix<4, N, SH_TEMP, T>
rowmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1,
       const Generic<N, T>& s2,
       const Generic<N, T>& s3)
{
  Matrix<4, N, SH_TEMP, T> r;
  r[0] = s0;
  r[1] = s1;
  r[2] = s2;
  r[3] = s3;
  return r;
}


template<int N, typename T>
Matrix<N, 1, SH_TEMP, T>
colmat(const Generic<N, T>& s0)
{
  Matrix<N, 1, SH_TEMP, T> r;
  for (int i = 0; i < N; ++i) {
    r[i][0] = s0[i];
  }
  return r;
}

template<int N, typename T>
Matrix<N, 2, SH_TEMP, T>
colmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1)
{
  Matrix<N, 2, SH_TEMP, T> r;
  for (int i = 0; i < N; ++i) {
    r[i][0] = s0[i];
    r[i][1] = s1[i];
  }
  return r;
}

template<int N, typename T>
Matrix<N, 3, SH_TEMP, T>
colmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1,
       const Generic<N, T>& s2)
{
  Matrix<N, 3, SH_TEMP, T> r;
  for (int i = 0; i < N; ++i) {
    r[i][0] = s0[i];
    r[i][1] = s1[i];
    r[i][2] = s2[i];
  }
  return r;
}


template<int N, typename T>
Matrix<N, 4, SH_TEMP, T>
colmat(const Generic<N, T>& s0,
       const Generic<N, T>& s1,
       const Generic<N, T>& s2,
       const Generic<N, T>& s3)
{
  Matrix<N, 4, SH_TEMP, T> r;
  for (int i = 0; i < N; ++i) {
    r[i][0] = s0[i];
    r[i][1] = s1[i];
    r[i][2] = s2[i];
    r[i][3] = s3[i];
  }
  return r;
}


template<int N, typename T>
Matrix<N, N, SH_TEMP, T>
diag(const Generic<N, T>& a)
{
  Matrix<N, N, SH_TEMP, T> r;
  
  for (int i = 0; i < N; ++i) r[i][i] = a[i];
  return r;
}

template<typename T>
Matrix<4, 4, SH_TEMP, T>
rotate(const Generic<3, T>& axis,
       const Generic<1, T>& angle)
{
  Generic<1, T> c = cos(angle);
  Generic<1, T> s = sin(angle);
  Generic<3, T> xyz = normalize(axis);

  Matrix<4, 4, SH_TEMP, T> result;

  result[0](0,1,2) = fillcast<3>((1.0 - c) * xyz(0));
  result[1](0,1,2) = fillcast<3>((1.0 - c) * xyz(1));
  result[2](0,1,2) = fillcast<3>((1.0 - c) * xyz(2));

  result[0](1,2) *= xyz(1,2);
  result[0][1] -= xyz(2) * s;
  result[0][2] += xyz(1) * s;

  result[1](0,2) *= xyz(0,2);
  result[1][0] += xyz(2) * s;
  result[1][2] -= xyz(0) * s;

  result[2](0,1) *= xyz(0,1);
  result[2][0] -= xyz(1) * s;
  result[2][1] += xyz(0) * s;
  
  result[0][0] *= xyz(0); result[0][0] += c;
  result[1][1] *= xyz(1); result[1][1] += c;
  result[2][2] *= xyz(2); result[2][2] += c;

  return result;
}

template<typename T>
Matrix<3, 3, SH_TEMP, T>
rotate(const Generic<1, T>& angle)
{
  Matrix<3, 3, SH_TEMP, T> result;
  
  Generic<1, T> c = cos(angle);
  Generic<1, T> s = sin(angle);
  result[0] = Attrib<3, SH_TEMP, T>(c, -s, 0.0f);
  result[1] = Attrib<3, SH_TEMP, T>(c, s, 0.0f);
  result[2] = Attrib<3, SH_TEMP, T>(0.0f, 0.0f, 1.0f);

  return result;
}

template<typename T>
Matrix<4, 4, SH_TEMP, T>
translate(const Generic<3, T>& a)
{
  Matrix<4, 4, SH_TEMP, T> result;

  for (int i = 0; i < 3; i++) {
    result[i][3] = a[i];
  }
  
  return result;
}

template<typename T>
Matrix<3, 3, SH_TEMP, T>
translate(const Generic<2, T>& a)
{
  Matrix<3, 3, SH_TEMP, T> result;

  result[0][2] = a[0];
  result[1][2] = a[1];
  
  return result;
}


template<typename T>
inline
Matrix<4, 4, SH_TEMP, T>
scale(const Generic<3, T>& a)
{
  return diag(join(a, ConstAttrib1f(1.0)));
}

template<typename T>
inline
Matrix<3, 3, SH_TEMP, T>
scale(const Generic<2, T>& a)
{
  return diag(join(a, ConstAttrib1f(1.0)));
}

template<int N, typename T>
inline
Matrix<N, N, SH_TEMP, T>
scale(const Generic<1, T>& a)
{
  return diag(join(fillcast<N - 1>(a), ConstAttrib1f(1.0)));
}

}

#endif
