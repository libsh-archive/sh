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
#include "Lib.hpp" // LibDeriv needs to be included from within Lib

#ifndef SHLIBDERIV_HPP
#define SHLIBDERIV_HPP

#include "Generic.hpp"
#include "Matrix.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_deriv Derivatives
 * @ingroup library
 * Operations related to derivatives of values.
 * @{
 */

/** Screen-space x derivatives
 */
template<int N, typename T>
Generic<N, T> dx(const Generic<N, T>& var);

/** Screen-space y derivatives
 */
template<int N, typename T>
Generic<N, T> dy(const Generic<N, T>& var);

/** Maximum value of absolute derivatives
 */
template<int N, typename T>
Generic<N, T> fwidth(const Generic<N, T>& var);

/** Pair of screen-space derivatives
 */
template<typename T>
Generic<2, T> gradient(const Generic<1, T>& var);

/** Jacobian matrix
 */
template<int N, typename T>
Matrix<2, N, TEMP, T> jacobian(const Generic<N, T>& var);

/*@}*/

}
#endif

#include "LibDerivImpl.hpp"

#endif
