#ifndef SHLIBRANGE_HPP
#define SHLIBRANGE_HPP

#include "ShGeneric.hpp"
#include "ShInterval.hpp"
#include "ShLib.hpp"

#define SH_REGULARTYPE(T) typename ShStorageTypeInfo<T>::RegularType
#define SH_IA_TYPE(T) typename ShStorageTypeInfo<T>::IntervalType

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
ShGeneric<N, SH_REGULARTYPE(T)> range_lo(const ShGeneric<N, T>& var);

 /** upper bound
 */
template<int N, typename T>
ShGeneric<N, SH_REGULARTYPE(T)> range_hi(const ShGeneric<N, T>& var);

 /** width of a range type 
  * @todo range could extend this to give 0 for non-range types */ 
template<int N, typename T>
ShGeneric<N, SH_REGULARTYPE(T)> range_width(const ShGeneric<N, T>& var);

 /** radius of a range type 
  * @todo range could extend this to give 0 for non-range types */ 
template<int N, typename T>
ShGeneric<N, SH_REGULARTYPE(T)> range_radius(const ShGeneric<N, T>& var);

/** center of a range type 
 * @todo range could extend this to give var for non-range types*/
template<int N, typename T>
ShGeneric<N, SH_REGULARTYPE(T)> range_center(const ShGeneric<N, T>& var);

/** Makes an interval out of regular typed lo and hi 
 */
template<int N, typename T1, typename T2>
ShGeneric<N, SH_IA_TYPE(CT1T2)> 
make_interval(const ShGeneric<N, T1>& lo, const ShGeneric<N, T2>& hi);
SH_SHLIB_CONST_SCALAR_OP_DECL(make_interval);
SH_SHLIB_CONST_N_OP_BOTH_DECL(make_interval);

// @todo range make isct/union work for invalid ranges

/** Takes a union of a and b to form a new interval 
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> 
range_union(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);
SH_SHLIB_CONST_SCALAR_OP_DECL(range_union);
SH_SHLIB_CONST_N_OP_BOTH_DECL(range_union);

/** INtersects two intervals 
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> 
range_isct(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);
SH_SHLIB_CONST_SCALAR_OP_DECL(range_isct);
SH_SHLIB_CONST_N_OP_BOTH_DECL(range_isct);

/** Returns whether a range a contains another range b 
 */
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> 
range_contains(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b);
SH_SHLIB_CONST_SCALAR_OP_DECL(range_contains);
SH_SHLIB_CONST_N_OP_BOTH_DECL(range_contains);

/*@}*/

/** Returns the error symbols from a due to b
 * @todo range this may not be very useful... */
template<int N1, typename T1, int N2, typename T2>
ShGeneric<N1, T1>
affine_errfrom(const ShGeneric<N1, T1>& a, const ShGeneric<N2, T2>& b);

/** Returns the error in a due to last error symbol in b (useful for both getting errors from inputs and stmts) */ 
template<int N1, typename T1, int N2, typename T2>
ShGeneric<N1, SH_REGULARTYPE(T1)>
affine_lasterr(const ShGeneric<N1, T1>& a, const ShGeneric<N2, T2>& b);

}
#endif

#include "ShLibRangeImpl.hpp"

#undef SH_REGULARTYPE
#undef SH_IA_TYPE

#endif
