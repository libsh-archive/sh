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
template<int N, typename T>
ShGeneric<N, T> lo(const ShGeneric<N, ShInterval<T> >& var);

 /** upper bound
 */
template<int N, typename T>
ShGeneric<N, T> hi(const ShGeneric<N, ShInterval<T> >& var);

/** Sets lower bound with a regular tuple and returns updated var */ 
template<int N, typename T1, typename T2>
ShGeneric<N, ShInterval<T1> >& 
setlo(ShGeneric<N, ShInterval<T1> >& var, const ShGeneric<N, T2> &lo);

/** Sets upper bound with a regular tuple and returns updated var */ 
template<int N, typename T1, typename T2>
ShGeneric<N, ShInterval<T1> >& 
sethi(ShGeneric<N, ShInterval<T1> >& var, const ShGeneric<N, T2> &hi);

/** Creates an interval from two tuples 
 */
template<int N, typename T1, typename T2>
ShGeneric<N, ShInterval<CT1T2> > 
makeInterval(const ShGeneric<N, T1>& lo, const ShGeneric<N, T2>& hi);

/*@}*/

}

#include "ShLibIntervalImpl.hpp"

#endif
