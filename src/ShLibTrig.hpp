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
#ifndef SHLIBTRIG_HPP
#define SHLIBTRIG_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"


namespace SH {

/** \defgroup lib_trig Trigonometric functions
 * @ingroup library
 * @todo tan, atan, atan2, hyperbolic functions, secant, cosecant, etc.
 * @{
 */

/** Arccosine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [0, pi].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, ShValueType V>
ShGeneric<N, V> acos(const ShGeneric<N, V>& var);

/** Arcsine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [-pi/2, pi/2].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, ShValueType V>
ShGeneric<N, V> asin(const ShGeneric<N, V>& var);

/** Arctangent. 
 * Operates componentwise on tuples.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, ShValueType V>
ShGeneric<N, V> atan(const ShGeneric<N, V>& var);

/** Arctangent of two variables. 
 * Operates componentwise on tuples of y/x.
 * Gives a result in [-pi/2, pi/2].
 */
template<int N, ShValueType V>
ShGeneric<N, V> atan2(const ShGeneric<N, V>& y, const ShGeneric<N, V>& x);

/** Cosine.
 * Operates componentwise on tuples.
 * Returns the cosine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, ShValueType V>
ShGeneric<N, V> cos(const ShGeneric<N, V>& var);

/** Sine.
 * Operates componentwise on tuples.
 * Returns the sine of x.   Any value of x gives a result
 * in the range [-1,1].
 */
template<int N, ShValueType V>
ShGeneric<N, V> sin(const ShGeneric<N, V>& var);

/** Tangent.
 * Operates componentwise on tuples.
 * Returns the tangent of x.   Equivalent to sin(x)/cos(x).
 */
template<int N, ShValueType V>
ShGeneric<N, V> tan(const ShGeneric<N, V>& var);

/*@}*/

}

#include "ShLibTrigImpl.hpp"

#endif
