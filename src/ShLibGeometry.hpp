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

/** Normalize an n-tuple to unit length.
 * Divides an n-tuple by its Euclidean length.   
 */
template<int N, typename T>
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var);

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
