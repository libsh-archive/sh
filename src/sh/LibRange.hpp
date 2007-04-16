#ifndef LIBRANGE_HPP
#define LIBRANGE_HPP

#include "Generic.hpp"
#include "Interval.hpp"
#include "Lib.hpp"

#define REGULARTYPE(T) typename StorageTypeInfo<T>::RegularType
#define IA_TYPE(T) typename StorageTypeInfo<T>::IntervalType

#ifndef WIN32
namespace SH {

/** \defgroup lib_range Range Arithmetic  
 * @ingroup library
 * Range arithmetic operations. 
 * @{
 */

/** lower bound 
 */
template<int N, typename T>
Generic<N, REGULARTYPE(T)> range_lo(const Generic<N, T>& var);

 /** upper bound
 */
template<int N, typename T>
Generic<N, REGULARTYPE(T)> range_hi(const Generic<N, T>& var);

 /** width of a range type 
  * @todo range could extend this to give 0 for non-range types */ 
template<int N, typename T>
Generic<N, REGULARTYPE(T)> range_width(const Generic<N, T>& var);

 /** radius of a range type 
  * @todo range could extend this to give 0 for non-range types */ 
template<int N, typename T>
Generic<N, REGULARTYPE(T)> range_radius(const Generic<N, T>& var);

/** center of a range type 
 * @todo range could extend this to give var for non-range types*/
template<int N, typename T>
Generic<N, REGULARTYPE(T)> range_center(const Generic<N, T>& var);

/** Makes an interval out of regular typed lo and hi 
 */
template<int N, typename T1, typename T2>
Generic<N, IA_TYPE(CT1T2)> 
make_interval(const Generic<N, T1>& lo, const Generic<N, T2>& hi);
SHLIB_CONST_SCALAR_OP_DECL(make_interval);
SHLIB_CONST_N_OP_BOTH_DECL(make_interval);

// @todo range make isct/union work for invalid ranges

/** Takes a union of a and b to form a new interval 
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
range_union(const Generic<N, T1>& a, const Generic<N, T2>& b);
SHLIB_CONST_SCALAR_OP_DECL(range_union);
SHLIB_CONST_N_OP_BOTH_DECL(range_union);

/** INtersects two intervals 
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
range_isct(const Generic<N, T1>& a, const Generic<N, T2>& b);
SHLIB_CONST_SCALAR_OP_DECL(range_isct);
SHLIB_CONST_N_OP_BOTH_DECL(range_isct);

/** Returns whether a range a contains another range b 
 */
template<int N, typename T1, typename T2>
Generic<N, CT1T2> 
range_contains(const Generic<N, T1>& a, const Generic<N, T2>& b);
SHLIB_CONST_SCALAR_OP_DECL(range_contains);
SHLIB_CONST_N_OP_BOTH_DECL(range_contains);

/*@}*/

/** Returns the error symbols from a due to b
 * @todo range this may not be very useful... */
template<int N1, typename T1, int N2, typename T2>
Generic<N1, T1>
affine_errfrom(const Generic<N1, T1>& a, const Generic<N2, T2>& b);

/** Returns the error in a due to last error symbol in b (useful for both getting errors from inputs and stmts) */ 
template<int N1, typename T1, int N2, typename T2>
Generic<N1, REGULARTYPE(T1)>
affine_lasterr(const Generic<N1, T1>& a, const Generic<N2, T2>& b);

}
#endif

#include "LibRangeImpl.hpp"

#undef REGULARTYPE
#undef IA_TYPE

#endif
