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
#ifndef SHUTIL_SHUTILLIB_HPP 
#define SHUTIL_SHUTILLIB_HPP 

#include <cmath>
#include "ShAttrib.hpp"
#include "ShConstant.hpp"
#include "ShSwizzle.hpp" 
#include "ShVariable.hpp"

namespace ShUtil {

using namespace SH;

//TODO extend to vector versions

/** \file ShUtilLib.hpp
 * \brief A set of small Sh utility functions.  
 */

/** \brief Clamps value between a and b. 
 * a must be <= b.
 * Returns a if x < a,
 * b if b < x,
 * x otherwise
 */
//@{
template<typename T>
ShVariableN<1, T> clamp(const ShVariableN<1, T>& a, const ShVariableN<1, T>& b,
    const ShVariableN<1, T>& x) {
  return min(max(x, a), b); 
}

template<typename T>
ShVariableN<1, T> clamp(double a, double b, const ShVariableN<1, T>& x) {
  return min(max(x, ShConstant1f(a)), ShConstant1f(b)); 
}
//@}


/** \brief Cubic interpolated step between 0 and 1. 
 * Returns 0 if x < a,
 * 1 if x > b, 
 * cubic interpolation between 0 and 1 otherwise
 */
template<typename T>
ShVariableN<1, T> smoothstep(const ShVariableN<1, T>& a, const ShVariableN<1, T>& b,
    const ShVariableN<1, T> x) {
  ShVariableN<1, T> t = (x - a) / (b - a);
  t = clamp(0.0, 1.0, t); 
  return t * t * mad(-2.0, t, ShConstant1f(3.0));
}

/** \brief Euclidean distance between two points.
 */
template<int N, typename T>
ShVariableN<1, T> distance(const ShVariableN<N, T>& a, const ShVariableN<N, T>& b) {
  return sqrt(dot(a, b));
}

/** \brief L1 distance between two points
 * The L1 distance is a sum of the absolute component-wise differences.
 */
template<int N, typename T>
ShVariableN<1, T> lOneDistance(const ShVariableN<N, T>& a, const ShVariableN<N, T>& b) {
  //TODO should use dot product with vector 1
  ShVariableN<N, T> diff = abs(a - b);
  return dot(1.0, diff); 
}

/** \brief Linfinity distance between two points 
 * Linfinity distance is the maximum absolute component-wise difference.
 */
template<int N, typename T>
ShVariableN<1, T> lInfDistance(const ShVariableN<N, T>& a, const ShVariableN<N, T>& b) {
  ShVariableN<N, T> diff = abs(a - b);
  ShVariableN<1, T> result = max(diff(0), diff(1));
  for(int i = 2; i < N; ++i) result = max(result, diff(i));
  return result;
}

// TODO analyze quality of random numbers and pick some better values
// These were just picked out of a hat, and may give very bad behaviour...
static ShAttrib4f hasha(97.409091034002437236440332688705, 
						std::exp(4.0), 
                        std::pow(13.0, 1.5707963267948966192313216916398), 
						std::sqrt(1997.0));
static ShAttrib4f hashm(std::sqrt(2.0), 
						0.31830988618379067153776752674503, 
						std::sqrt(3.0), 
                        std::exp(-1.0));

static const int LCG_REPS = 5; // total instructions for hashlcg will be LCG_REPS * 2 + 2
/** \brief Parallel linear congruential generator
 * 
 * This does not work very well right now.  Use hashmrg instead.
 *
 * \sa  template<int N, typename T> ShVariableN<N, T> hashmrg(const ShVariableN<N, T>& p)
 */
// TODO: may not work as intended on 24-bit floats
// since there may not be enough precision 
template<int N, typename T>
ShVariableN<N, T> hashlcg(const ShVariableN<N, T>& p) {
  ShAttrib<N, SH_VAR_TEMP, T> result = frac(p * 0.01);

  ShVariableN<N, T> a(hasha.node(), ShSwizzle(N), false);
  ShVariableN<N, T> m(hashm.node(), ShSwizzle(N), false);

  for(int i = 0; i < LCG_REPS; ++i) {
    result = frac(mad(result, a, m)); 
  }
  return result;
}


static const int MRG_REPS = 2; // total instructions for hashmrg will be MRG_REPS * N * 2 + 2 
/** \brief MRG style pseudorandom vector generator
 *
 * Generates a random vector using a multiple-recursive generator style. (LCG on steroids)
 * Treat x,y,z,w as seeds a0, a1, a2, a3
 * and repeatedly apply an = b * (an-1, an-2, an-3, an-4), where b is a vector
 * Take as output (an, an-1, an-2, an3) after suitable number of iterations.
 *
 * This appears to reduce correlation in the output components when input components are 
 * similar, but the behaviour needs to be studied further.
 *
 * \sa template<int N, typename T> ShVariableN<N, T> hashlcg(const ShVariableN<N, T>& p)
 */
template<int N, typename T>
ShVariableN<N, T> hashmrg(const ShVariableN<N, T>& p) {
  ShAttrib<N, SH_VAR_TEMP, T> result = frac(p * 0.01);
  ShVariableN<N, T> a(hasha.node(), ShSwizzle(N), false); // use only first N elements of hasha

  for(int i = 0; i < MRG_REPS; ++i) {
    for(int j = 0; j < N; ++j) { 
      result(j) = frac(dot(result, a));
    }
  }
  return result;
}

}

#endif // SHUTIL_SHUTILLIB_HPP 
