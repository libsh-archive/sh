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
template<typename T>
ShGeneric<3, T> cross(const ShGeneric<3, T>& left, const ShGeneric<3, T>& right);

/// Equivalent to cross(left, right)
template<int N, typename T>
ShGeneric<3, T> operator^(const ShGeneric<3, T>& left, const ShGeneric<3, T>& right);

/** Normalize an n-tuple to unit length.
 * Divides an n-tuple by its Euclidean length.   
 */
template<int N, typename T>
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var);

/** Compute reflection vector.
 * Reflect vector a about normal n.
 */
template<int N, typename T>
ShGeneric<N, T> reflect(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b);

/** Make a vector face the same way as another
 * Negates b if it does not face the same way as a (i.e. the dot
 * product between a and b is negative).
 */
template<int N, typename T>
ShGeneric<N, T> faceforward(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b);

/** Compute lighting coefficents.
 * returns a 4-vector consisting of:
 *  - 1
 *  - max(a,0)
 *  - b^c if (a > 0 and b < 0), 0 otherwise
 *  - 1
 */
template<typename T>
ShGeneric<4, T> lit(const ShGeneric<1, T>& a,
                    const ShGeneric<1, T>& b,
                    const ShGeneric<1, T>& c);

/** Inner (dot) product.
 */
template<int N, typename T>
ShGeneric<1,  T> dot(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);

template<int N, typename T>
ShGeneric<1,  T> operator|(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right);

SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

/*@}*/

}

#include "ShLibGeometryImpl.hpp"

#endif
