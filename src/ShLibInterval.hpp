#ifndef SHLIBINTERVAL_HPP
#define SHLIBINTERVAL_HPP

#include "ShGeneric.hpp"
#include "ShInterval.hpp"
#include "ShLib.hpp"

namespace SH {

/** \defgroup lib_interval Interval Arithmetic  
 * @ingroup library
 * Operations related to derivatives of values.
 * @{
 */

/** lower bound 
 */
template<int N, ShValueType V>
ShGeneric<N, V> lo(const ShGeneric<N, ShIntervalValueType<V>::type>& var);

 /** upper bound
 */
template<int N, ShValueType V>
ShGeneric<N, V> hi(const ShGeneric<N, ShIntervalValueType<V>::type>& var);

/** Sets lower bound with a regular tuple and returns updated var */ 
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, ShIntervalValueType<V1>::type>& 
setlo(ShGeneric<N, ShIntervalValueType<V1>::type>& var, const ShGeneric<N, V2> &lo);

/** Sets upper bound with a regular tuple and returns updated var */ 
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, ShIntervalValueType<V1>::type>& 
sethi(ShGeneric<N, ShIntervalValueType<V1>::type>& var, const ShGeneric<N, V2> &hi);

/** Creates an interval from two tuples 
 */
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, ShIntervalValueType<CV1V2>::type> 
makeInterval(const ShGeneric<N, V1>& lo, const ShGeneric<N, V2>& hi);

/*@}*/

}

#include "ShLibIntervalImpl.hpp"

#endif
