#ifndef SHLIBGEOMETRY_HPP
#define SHLIBGEOMETRY_HPP

#include "ShGeneric.hpp"
#include "ShLib.hpp"

namespace SH {

/** \defgroup lib_geometry Geometric operations.
 * @ingroup library
 * @todo distance, sqdistance, length, rlength, rsqlength, sqlength, perp,
 * perpdot, projnorm should be implemented too.   Different distance functions
 * also possible: L1, L2, Linf.
 * @{
 */

/** Take the cross product of two 3-tuples.
 * Note that this operation is not limited to vectors.
 */
template<typename T1, typename T2>
ShGeneric<3, CT1T2> 
cross(const ShGeneric<3, T1>& left, const ShGeneric<3, T2>& right);

/// Equivalent to cross(left, right)
template<int N, typename T1, typename T2>
ShGeneric<3, CT1T2> 
operator^(const ShGeneric<3, T1>& left, const ShGeneric<3, T2>& right);

/** Normalize an n-tuple to unit length.
 * Divides an n-tuple by its Euclidean length.   
 */
template<int N, typename T>
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var);

/** Compute reflection vector.
 * Reflect vector a about normal n.
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> 
reflect(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);

/** Compute refraction vector.
 * Refract vector a about normal b using relative index of refraction c.
 */
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> 
refract(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b, const ShGeneric<1, T3>& c);

/** Make a vector face the same way as another
 * Negates b if it does not face the same way as a (i.e. the dot
 * product between a and b is negative).
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> 
faceforward(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);

/** Compute lighting coefficents.
 * returns a 4-vector consisting of:
 *  - 1
 *  - max(a,0)
 *  - b^c if (a > 0 and b < 0), 0 otherwise
 *  - 1
 */
template<typename T1, typename T2, typename T3>
ShGeneric<4, CT1T2T3> 
lit(const ShGeneric<1, T1>& a,
    const ShGeneric<1, T2>& b,
    const ShGeneric<1, T3>& c);

/** Inner (dot) product.
 */
template<int N, typename T1, typename T2>
ShGeneric<1, CT1T2> 
dot(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);

template<int N, typename T1, typename T2>
ShGeneric<1, CT1T2> 
operator|(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right);

SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

/*@}*/

}

#include "ShLibGeometryImpl.hpp"

#endif
