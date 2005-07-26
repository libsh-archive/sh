// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShLib.hpp" // ShLibMatrix needs to be included from within ShLib

#ifndef SHLIBMATRIX_HPP
#define SHLIBMATRIX_HPP

#include "ShGeneric.hpp"
#include "ShMatrix.hpp"

#ifndef WIN32
namespace SH {

/** \defgroup lib_matrix Matrix operations
 * @ingroup library
 * @{
 */

/** Matrix addition.
 * Only works on matrices of same sizes.
 */
template<int M, int N, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
ShMatrix<N, M, SH_TEMP, CT1T2>
operator+(const ShMatrix<N, M, Binding, T1>& a, const ShMatrix<N, M, Binding2, T2>& b);

/** Matrix subtraction.
 * Only works on matrices of same sizes.
 */
template<int M, int N, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
ShMatrix<N, M, SH_TEMP, CT1T2>
operator-(const ShMatrix<N, M, Binding, T1>& a, const ShMatrix<N, M, Binding2, T2>& b);

/** Matrix division.
 * Only works on matrices of same sizes.
 */
template<int M, int N, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
ShMatrix<N, M, SH_TEMP, CT1T2>
operator/(const ShMatrix<N, M, Binding, T1>& a, const ShMatrix<N, M, Binding2, T2>& b);

/** Matrix multiplication.
 * Only works on matrices of compatible sizes or ones that can be expanded to be compatible.
 */
template<int M, int N1, int N2, int P, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
ShMatrix<M, P, SH_TEMP, CT1T2>
operator|(const ShMatrix<M, N1, Binding, T1>& a,
          const ShMatrix<N2, P, Binding2, T2>& b);
template<int M, int N1, int N2, int P, ShBindingType Binding, ShBindingType Binding2, typename T1, typename T2>
ShMatrix<M, P, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, N1, Binding, T1>& a,
          const ShMatrix<N2, P, Binding2, T2>& b);

/** Matrix-tuple multiplication.
 * Treats the tuple as a column vector.
 * @{ */
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<N, T2>& b);
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<N, T2>& b);
// @}

/** Matrix-tuple multiplication.
 * If the matrix on the left has one more column than the number of
 * elements in the tuple, the weight of the last column is assumed to
 * be ZERO. @{ */
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_NORMAL, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_VECTOR, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_NORMAL, swizzled>& b);
// @}

/** Matrix-tuple multiplication.
 * If the matrix on the left has one more column than the number of
 * elements in the tuple, the weight of the last column is assumed to
 * be ONE. @{ */
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_TEXCOORD, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_POINT, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator|(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_PLANE, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_TEXCOORD, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_POINT, swizzled>& b);
template<int M, int N, ShBindingType Binding1, ShBindingType Binding2, typename T1, typename T2, bool swizzled>
ShGeneric<M, CT1T2> operator*(const ShMatrix<M, N, Binding1, T1>& a, const ShAttrib<N-1, Binding2, T2, SH_PLANE, swizzled>& b);
// @}

/** Tuple-matrix multiplication.
 * Treats the tuple as a row vector.
 */
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShGeneric<N, CT1T2> operator|(const ShGeneric<M, T1>& a, const ShMatrix<M, N, Binding, T2>& b);
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShGeneric<N, CT1T2> operator*(const ShGeneric<M, T1>& a, const ShMatrix<M, N, Binding, T2>& b);

/// Matrix-scalar multiplication
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<1, T2>& b);

template<int M, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, 1, SH_TEMP, CT1T2>
operator*(const ShMatrix<M, 1, Binding, T1>& a, const ShGeneric<1, T2>& b);

/// Scalar-matrix multiplication
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator*(const ShGeneric<1, T1>& a, const ShMatrix<M, N, Binding, T2>& b);

template<int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<1, N, SH_TEMP, CT1T2>
operator*(const ShGeneric<1, T1>& a, const ShMatrix<1, N, Binding, T2>& b);

/// Matrix-scalar division
template<int M, int N, ShBindingType Binding, typename T1, typename T2>
ShMatrix<M, N, SH_TEMP, CT1T2>
operator/(const ShMatrix<M, N, Binding, T1>& a, const ShGeneric<1, T2>& b);

/** \brief Returns the determinant for the given matrix
 *
 */
template<ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<1, 1, Binding2, T2>& matrix);

template<ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<2, 2, Binding2, T2>& matrix);
    
template<int RowsCols, ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);


/** \brief Returns the matrix of cofactors for the given matrix 
 *
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols, RowsCols, SH_TEMP, T2>
cofactors(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);
    
/** \brief Returns the transpose of the given matrix.
 *
 * The matrix is flipped around its diagonal.
 *
 */
template<int M, int N, ShBindingType Binding2, typename T2>
ShMatrix<N, M, SH_TEMP, T2>
transpose(const ShMatrix<M, N, Binding2, T2>& matrix);
  
/** \brief Returns the adjoint of the given matrix.
 *
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols, RowsCols, SH_TEMP, T2>
adjoint(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);

/** \brief Invert a matrix
 *
 * Results are undefined if the matrix is non-invertible.
 *
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols,RowsCols, SH_TEMP, T2>
inverse(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);

/** \brief Trace of a matrix.
 *
 * Returns the sum of the diagonal entries of a matrix.
 */
template<int RowsCols, ShBindingType Binding, typename T>
ShGeneric<1, T>
trace(const ShMatrix<RowsCols, RowsCols, Binding, T>& matrix);

template<int N, typename T>
ShMatrix<1, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0);

template<int N, typename T1, typename T2>
ShMatrix<2, N, SH_TEMP, CT1T2>
rowmat(const ShGeneric<N, T1>& s0,
       const ShGeneric<N, T2>& s1);

template<int N, typename T1, typename T2, typename T3>
ShMatrix<3, N, SH_TEMP, CT1T2T3>
rowmat(const ShGeneric<N, T1>& s0,
       const ShGeneric<N, T2>& s1,
       const ShGeneric<N, T3>& s2);

template<int N, typename T1, typename T2, typename T3, typename T4>
ShMatrix<4, N, SH_TEMP, CT1T2T3T4>
rowmat(const ShGeneric<N, T1>& s0,
       const ShGeneric<N, T2>& s1,
       const ShGeneric<N, T3>& s2,
       const ShGeneric<N, T4>& s3);

template<int N, typename T>
ShMatrix<N, 1, SH_TEMP, T>
colmat(const ShGeneric<N, T>& s0);

template<int N, typename T>
ShMatrix<N, 2, SH_TEMP, T>
colmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1);

template<int N, typename T>
ShMatrix<N, 3, SH_TEMP, T>
colmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2);

template<int N, typename T>
ShMatrix<N, 4, SH_TEMP, T>
colmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2,
       const ShGeneric<N, T>& s3);

template<int N, typename T>
ShMatrix<N, N, SH_TEMP, T>
diag(const ShGeneric<N, T>& a);

/*@}*/


/** \defgroup lib_matrix_trans Matrix transformations
 * @ingroup library
 * @{
 */

/** \brief 3D Rotation about the given axis by the given angle
 *
 * The angle is specified in radians. The result is an affine
 * transformation matrix.
 */
template<typename T>
ShMatrix<4, 4, SH_TEMP, T>
rotate(const ShGeneric<3, T>& axis, const ShGeneric<1, T>& angle);

/** \brief 2D Rotation about the given angle
 *
 * The angle is specified in radians. The result is an affine
 * transformation matrix.
 */
template<typename T>
ShMatrix<3, 3, SH_TEMP, T>
rotate(const ShGeneric<1, T>& angle);

/** \brief 3D Translation
 *
 * Returns an affine transformation matrix translating by the given
 * vector.
 */
template<typename T>
ShMatrix<4, 4, SH_TEMP, T>
translate(const ShGeneric<3, T>& a);

/** \brief 2D Translation
 *
 * Returns an affine transformation matrix translating by the given
 * vector.
 */
template<typename T>
ShMatrix<3, 3, SH_TEMP, T>
translate(const ShGeneric<2, T>& a);

/** \brief 3D Scale
 *
 * Returns an affine transformation matrix scaling by the given
 * nonuniform vector.
 */
template<typename T>
ShMatrix<4, 4, SH_TEMP, T>
scale(const ShGeneric<3, T>& a);

/** \brief 2D Scale
 *
 * Returns an affine transformation matrix scaling by the given
 * nonuniform vector.
 */
template<typename T>
ShMatrix<3, 3, SH_TEMP, T>
scale(const ShGeneric<2, T>& a);

/** \brief Uniform scale in N-1 dimensions
 *
 * Returns an NxN affine transformation matrix performing a uniform
 * scale of the given amount in (N-1) dimensions.
 */
template<int N, typename T>
ShMatrix<N, N, SH_TEMP, T>
scale(const ShGeneric<1, T>& a);

/*@}*/

}
#endif

#include "ShLibMatrixImpl.hpp"

#endif
