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
#include "Lib.hpp" // LibGeometry needs to be included from within Lib

#ifndef SHLIBGEOMETRY_HPP
#define SHLIBGEOMETRY_HPP

#include "Generic.hpp"

#ifndef _WIN32
namespace SH {

/** \defgroup lib_geometry Geometric operations.
 * @ingroup library
 * @todo sqdistance, rlength, rsqlength, sqlength, perp,
 * perpdot, projnorm should be implemented too.
 * @{
 */

/** Take the cross product of two 3-tuples.
 * Note that this operation is not limited to vectors.
 */
template<typename T1, typename T2>
Generic<3, CT1T2> 
cross(const Generic<3, T1>& left, const Generic<3, T2>& right);

/// Equivalent to cross(left, right)
template<int N, typename T1, typename T2>
Generic<3, CT1T2> 
operator^(const Generic<3, T1>& left, const Generic<3, T2>& right);

/** Normalize an n-tuple to unit length.
 * Divides an n-tuple by its Euclidean length.   
 */
template<int N, typename T>
Generic<N, T> normalize(const Generic<N, T>& var);

/** Compute reflection vector.
 * Reflect vector a about normal n.
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
reflect(const Generic<N, T1>& a, const Generic<N, T2>& b);

/** Compute refraction vector.
 * @param normal Outward facing normal vector
 * @param view Outward facing view vector
 * @param eta Ratio of IOR of second surface divided by IOR of first
 */
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> 
refract(const Generic<N, T1>& view, const Generic<N, T2>& normal, const Generic<1, T3>& eta);

/** Compute refraction vector and determine whether or not total internal reflection takes place.
 * @param normal Outward facing normal vector
 * @param view Outward facing view vector
 * @param eta Ratio of IOR of second surface divided by IOR of first
 * @tir eta Set to 1 if TIR happens, 0 otherwise.
 */
template<int N, typename T1, typename T2, typename T3, typename T4>
Generic<N, CT1T2T3> refract_tir(const Generic<N, T1>& v, const Generic<N, T2>& n,
                                const Generic<1, T3>& eta, Generic<1, T4>& tir);

/** Make a vector face the same way as another
 * Negates b if it does not face the same way as a (i.e. the dot
 * product between a and b is negative).
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
faceforward(const Generic<N, T1>& a, const Generic<N, T2>& b);

/** Compute lighting coefficents.
 * returns a 4-vector consisting of:
 *  - 1
 *  - max(a,0)
 *  - b^c if (a > 0 and b < 0), 0 otherwise
 *  - 1
 */
template<typename T1, typename T2, typename T3>
Generic<4, CT1T2T3> 
lit(const Generic<1, T1>& a,
    const Generic<1, T2>& b,
    const Generic<1, T3>& c);

/** Inner (dot) product.
 */
template<int N1, int N2, typename T1, typename T2>
Generic<1, CT1T2> 
dot(const Generic<N1, T1>& left, const Generic<N2, T2>& right);

template<int N, typename T1, typename T2>
Generic<1, CT1T2> 
operator|(const Generic<N, T1>& left, const Generic<N, T2>& right);

SHLIB_CONST_N_OP_RETSIZE_BOTH_DECL(dot, 1);

/*@}*/

/** Euclidean Distance
 * Computes the Euclidean distance between the two points a and b.
 */
template<int N, typename T>
Generic<1, T> distance(const Generic<N, T>& a, const Generic<N, T>& b);

/** Manhattan Distance
 * Computes the Manhattan distance between the two points a and b.
 */
template<int N, typename T>
Generic<1, T> distance_1(const Generic<N, T>& a, const Generic<N, T>& b);

/** Inf Distance
 * Computes the Inf norm distance between the two points a and b.
 */
template<int N, typename T>
Generic<1, T> distance_inf(const Generic<N, T>& a, const Generic<N, T>& b);

/** Euclidean Length
 * Computes the euclidean distance between the zero vector and a.
 */
template<int N, typename T>
Generic<1, T> length(const Generic<N, T>& a);

/** Manhattan Length
 * Computes the Manhattan distance between the zero vector and a.
 */
template<int N, typename T>
Generic<1, T> length_1(const Generic<N, T>& a);

/** Inf Length
 * Computes the Inf norm distance between the zero vector and a.
 */
template<int N, typename T>
Generic<1, T> length_inf(const Generic<N, T>& a);

}
#endif

#include "LibGeometryImpl.hpp"

#endif
