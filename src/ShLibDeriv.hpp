#ifndef SHLIBDERIV_HPP
#define SHLIBDERIV_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"
#include "ShMatrix.hpp"

namespace SH {

/** \defgroup lib_deriv Derivatives
 * @ingroup library
 * Operations related to derivatives of values.
 * @{
 */

/** Screen-space x derivatives
 */
template<int N, typename T>
ShGeneric<N, T> dx(const ShGeneric<N, T>& var);

/** Screen-space y derivatives
 */
template<int N, typename T>
ShGeneric<N, T> dy(const ShGeneric<N, T>& var);

/** Maximum value of absolute derivatives
 */
template<int N, typename T>
ShGeneric<N, T> fwidth(const ShGeneric<N, T>& var);

/** Pair of screen-space derivatives
 */
template<typename T>
ShGeneric<2, T> gradient(const ShGeneric<1, T>& var);

/** Jacobian matrix
 */
template<int N, typename T>
ShMatrix<2, N, SH_TEMP, T> jacobian(const ShGeneric<N, T>& var);

/*@}*/

}

#include "ShLibDerivImpl.hpp"

#endif
