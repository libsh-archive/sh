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
 * @todo Move all the functions from ShMatrix.hpp here.
 * @{
 */

/** Transpose of a matrix.
 * @todo this implementation is not very smart.   Really, the compiler should
 * keep track of transpose state and swizzle references as needed, so that
 * a copy of the matrix does not need to be formed.   But this needs to be
 * overridden if the transpose of a matrix is taken and then modified
 * (which, however, does not happen very often).
 */
template<int Rows, int Cols, ShBindingType Binding, typename T>
ShMatrix<Cols, Rows, SH_TEMP, T>
transpose(const ShMatrix<Rows, Cols, Binding, T>& m);

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

/*@}*/

}

#include "ShLibMatrixImpl.hpp"

#endif
