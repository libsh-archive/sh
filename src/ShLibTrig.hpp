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
template<int N, typename T>
ShGeneric<N, T> acos(const ShGeneric<N, T>& var);

/** Arcsine. 
 * Operates componentwise on tuples.
 * A value of x in [-1, 1] gives a result in [-pi/2, pi/2].
 * Input values outside the range [-1,1] will give undefined results.
 */
template<int N, typename T>
ShGeneric<N, T> asin(const ShGeneric<N, T>& var);

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

/*@}*/

}

#include "ShLibTrigImpl.hpp"

#endif
