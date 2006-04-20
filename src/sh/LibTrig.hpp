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
#include "Lib.hpp" // LibTrig needs to be included from within Lib

#ifndef SHLIBTRIG_HPP
#define SHLIBTRIG_HPP

#include "Generic.hpp"

#ifndef _WIN32
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
Generic<N, T> acos(const Generic<N, T>& var);

/** Arcsine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [-pi/2, pi/2].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
Generic<N, T> asin(const Generic<N, T>& var);

/** Arctangent. 
 * Operates componentwise on tuples.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, typename T>
Generic<N, T> atan(const Generic<N, T>& var);

/** Arctangent of two variables. 
 * Operates componentwise on tuples of y/x.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, typename T>
Generic<N, T> atan2(const Generic<N, T>& y, const Generic<N, T>& x);

/** Cosine.
 * Operates componentwise on tuples.
 * Returns the cosine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
Generic<N, T> cos(const Generic<N, T>& var);

/** Sine.
 * Operates componentwise on tuples.
 * Returns the sine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, typename T>
Generic<N, T> sin(const Generic<N, T>& var);

/** Tangent.
 * Operates componentwise on tuples.
 * Returns the tangent of x.   Equivalent to sin(x)/cos(x).
 */
template<int N, typename T>
Generic<N, T> tan(const Generic<N, T>& var);

/** Hyperbolic cosine.
 * Operates componentwise on tuples.
 * Returns the hypebolic cosine of x.
 */
template<int N, typename T>
Generic<N, T> cosh(const Generic<N, T>& var);

/** Hyperbolic sine.
 * Operates componentwise on tuples.
 * Returns the hyperbolic sine of x.
 */
template<int N, typename T>
Generic<N, T> sinh(const Generic<N, T>& var);

/** Hyperbolic tangent.
 * Operates componentwise on tuples.
 * Returns the hyperbolic tangent of x.   Equivalent to sinh(x)/cosh(x).
 */
template<int N, typename T>
Generic<N, T> tanh(const Generic<N, T>& var);

/** Hyperbolic arccosine.
 * Operates componentwise on tuples.
 * Returns the hypebolic arccosine of x.
 */
template<int N, typename T>
Generic<N, T> acosh(const Generic<N, T>& var);

/** Hyperbolic arcsine.
 * Operates componentwise on tuples.
 * Returns the hyperbolic arcsine of x.
 */
template<int N, typename T>
Generic<N, T> asinh(const Generic<N, T>& var);

/** Hyperbolic arctangent.
 * Operates componentwise on tuples.
 * Returns the hyperbolic arctangent of x.
 */
template<int N, typename T>
Generic<N, T> atanh(const Generic<N, T>& var);
/*@}*/

}
#endif

#include "LibTrigImpl.hpp"

#endif
