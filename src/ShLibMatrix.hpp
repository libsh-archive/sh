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
