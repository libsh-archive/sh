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
#ifndef SHLIBMATRIX_HPP
#define SHLIBMATRIX_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"
#include "ShMatrix.hpp"

namespace SH {

/** \defgroup lib_matrix Matrix operations
 * @ingroup library
 * @{
 */

/** Matrix multiplication.
 * Only works on matrices of compatible sizes.
 */
template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T>
ShMatrix<M, P, SH_TEMP, T>
operator|(const ShMatrix<M, N, Binding, T>& a,
          const ShMatrix<N, P, Binding2, T>& b);
template<int M, int N, int P, ShBindingType Binding, ShBindingType Binding2, typename T>
ShMatrix<M, P, SH_TEMP, T>
operator*(const ShMatrix<M, N, Binding, T>& a,
          const ShMatrix<N, P, Binding2, T>& b);

/** Matrix-tuple multiplication.
 * Treats the tuple as a column vector.
 */
template<int M, int N, ShBindingType Binding, typename T>
ShGeneric<M, T> operator|(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<N, T>& b);
template<int M, int N, ShBindingType Binding, typename T>
ShGeneric<M, T> operator*(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<N, T>& b);

/** Tuple-matrix multiplication.
 * Treats the tuple as a row vector.
 */
template<int M, int N, ShBindingType Binding, typename T>
ShGeneric<N, T> operator|(const ShGeneric<M, T>& a, const ShMatrix<M, N, Binding, T>& b);
template<int M, int N, ShBindingType Binding, typename T>
ShGeneric<N, T> operator*(const ShGeneric<M, T>& a, const ShMatrix<M, N, Binding, T>& b);

/// Matrix-scalar multiplication
template<int M, int N, ShBindingType Binding, typename T>
ShMatrix<M, N, SH_TEMP, T>
operator*(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<1, T>& b);
template<int M, ShBindingType Binding, typename T>
ShMatrix<M, 1, SH_TEMP, T>
operator*(const ShMatrix<M, 1, Binding, T>& a, const ShGeneric<1, T>& b);
/// Scalar-matrix multiplication
template<int M, int N, ShBindingType Binding, typename T>
ShMatrix<M, N, SH_TEMP, T>
operator*(const ShGeneric<1, T>& a, const ShMatrix<M, N, Binding, T>& b);
template<int N, ShBindingType Binding, typename T>
ShMatrix<1, N, SH_TEMP, T>
operator*(const ShGeneric<1, T>& a, const ShMatrix<1, N, Binding, T>& b);
/// Matrix-scalar division
template<int M, int N, ShBindingType Binding, typename T>
ShMatrix<1, N, SH_TEMP, T>
operator/(const ShMatrix<M, N, Binding, T>& a, const ShGeneric<1, T>& b);

/** \brief Returns the determinant for the matrix in parameter
 *
 * Returns a ShAttrib1f containing the value of the determinant of the
 * matrix. NOTE: This method uses an naive recursive algorithm in
 * O(n!) which could take a long time with large matrices
 */
template<ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<1, 1, Binding2, T2>& matrix);

template<ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<2, 2, Binding2, T2>& matrix);
    
template<int RowsCols, ShBindingType Binding2, typename T2>
ShAttrib1f det(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);


/** \brief Returns the matrix of cofactors for the matrix in parameter
 *
 * Returns a ShMatrix containing the same amount of Rows/Colsvalue as
 * the matrix in parameter
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols, RowsCols, SH_TEMP, T2>
cofactors(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);
    
/** \brief Returns the transposed matrix for the matrix in parameter
 *
 * Returns a ShMatrix that is the transposed of the one in parameters
 */
template<int M, int N, ShBindingType Binding2, typename T2>
ShMatrix<N, M, SH_TEMP, T2>
transpose(const ShMatrix<M, N, Binding2, T2>& matrix);
  
/** \brief Returns the adjacent matrix for the matrix in parameter
 *
 * Returns a ShMatrix that is the adjacent of the one in parameters
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols, RowsCols, SH_TEMP, T2>
adjoint(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);

/** \brief Returns the inverse matrix for the matrix in parameter
 *
 * Returns a ShMatrix that is the inverse of the one in parameters
 * (NOTE: There is no error handling if the determinant is 0, NOTE #2:
 * We might want to rewrite this method if we have the * operator with
 * a ShVariableN)
 */
template<int RowsCols, ShBindingType Binding2, typename T2>
ShMatrix<RowsCols,RowsCols, SH_TEMP, T2>
inverse(const ShMatrix<RowsCols, RowsCols, Binding2, T2>& matrix);

template<int N, typename T>
ShMatrix<1, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0);

template<int N, typename T>
ShMatrix<2, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1);

template<int N, typename T>
ShMatrix<3, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2);

template<int N, typename T>
ShMatrix<4, N, SH_TEMP, T>
rowmat(const ShGeneric<N, T>& s0,
       const ShGeneric<N, T>& s1,
       const ShGeneric<N, T>& s2,
       const ShGeneric<N, T>& s3);

template<int N, typename T>
ShMatrix<N, N, SH_TEMP, T>
diag(const ShGeneric<N, T>& a);

/*@}*/

}

#include "ShLibMatrixImpl.hpp"

#endif
