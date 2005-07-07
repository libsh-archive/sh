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
#ifndef SHLIBTRIG_HPP
#define SHLIBTRIG_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"


#ifndef WIN32
namespace SH {

/** \defgroup lib_trig Trigonometric functions
 * @ingroup library
 * @todo inverse hyperbolic functions
 * @{
 */

/** Arccosine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [0, pi].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
ShGeneric<N, T> acos(const ShGeneric<N, T>& var);

/** Arcsine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [-pi/2, pi/2].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
ShGeneric<N, T> asin(const ShGeneric<N, T>& var);

/** Arctangent. 
 * Operates componentwise on tuples.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, typename T>
ShGeneric<N, T> atan(const ShGeneric<N, T>& var);

/** Arctangent of two variables. 
 * Operates componentwise on tuples of y/x.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, typename T>
ShGeneric<N, T> atan2(const ShGeneric<N, T>& y, const ShGeneric<N, T>& x);

/** Cosine.
 * Operates componentwise on tuples.
 * Returns the cosine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
ShGeneric<N, T> cos(const ShGeneric<N, T>& var);

/** Sine.
 * Operates componentwise on tuples.
 * Returns the sine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
ShGeneric<N, T> sin(const ShGeneric<N, T>& var);

/** Tangent.
 * Operates componentwise on tuples.
 * Returns the tangent of x.   Equivalent to sin(x)/cos(x).
 */
template<int N, typename T>
ShGeneric<N, T> tan(const ShGeneric<N, T>& var);

/** Hyperbolic cosine.
 * Operates componentwise on tuples.
 * Returns the hypebolic cosine of x.
 */
template<int N, typename T>
ShGeneric<N, T> cosh(const ShGeneric<N, T>& var);

/** Hyperbolic sine.
 * Operates componentwise on tuples.
 * Returns the hyperbolic sine of x.
 */
template<int N, typename T>
ShGeneric<N, T> sinh(const ShGeneric<N, T>& var);

/** Hyperbolic tangent.
 * Operates componentwise on tuples.
 * Returns the hyperbolic tangent of x.   Equivalent to sinh(x)/cosh(x).
 */
template<int N, typename T>
ShGeneric<N, T> tanh(const ShGeneric<N, T>& var);

/*@}*/

}
#endif

#include "ShLibTrigImpl.hpp"

#endif
