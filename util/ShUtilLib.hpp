// SH::Sh: A GPU metaprogramming language.
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

#include <sh/sh.hpp>
#include <sh/ShSwizzle.hpp>
#include <cmath>

namespace ShUtil {

//TODO extend to vector versions

/** \file SH::ShUtilLib.hpp
 * \brief A set of small SH::Sh utility functions.  
 */

/** \brief Clamps value between a and b. 
 * a must be <= b.
 * Returns a if x < a,
 * b if b < x,
 * x otherwise
 */
//@{
template<typename T>
SH::ShVariableN<1, T> clamp(const SH::ShVariableN<1, T>& a, const SH::ShVariableN<1, T>& b,
    const SH::ShVariableN<1, T>& x) {
  return min(max(x, a), b); 
}

template<typename T>
SH::ShVariableN<1, T> clamp(double a, double b, const SH::ShVariableN<1, T>& x) {
  return min(max(x, SH::ShConstant1f(a)), SH::ShConstant1f(b)); 
}
//@}


/** \brief Cubic interpolated step between 0 and 1. 
 * Returns 0 if x < a,
 * 1 if x > b, 
 * cubic interpolation between 0 and 1 otherwise
 */
template<typename T>
SH::ShVariableN<1, T> smoothstep(const SH::ShVariableN<1, T>& a, const SH::ShVariableN<1, T>& b,
    const SH::ShVariableN<1, T> x) {
  SH::ShVariableN<1, T> t = (x - a) / (b - a);
  t = clamp(0.0, 1.0, t); 
  return t * t * mad(-2.0, t, SH::ShConstant1f(3.0));
}

/** \brief Euclidean distance between two points.
 */
template<int N, typename T>
SH::ShVariableN<1, T> distance(const SH::ShVariableN<N, T>& a, const SH::ShVariableN<N, T>& b) {
  return sqrt(dot(a, b));
}

/** \brief L1 distance between two points
 * The L1 distance is a sum of the absolute component-wise differences.
 */
template<int N, typename T>
SH::ShVariableN<1, T> lOneDistance(const SH::ShVariableN<N, T>& a, const SH::ShVariableN<N, T>& b) {
  //TODO should use dot product with vector 1
  SH::ShVariableN<N, T> diff = abs(a - b);
  return dot(1.0, diff); 
}

/** \brief Linfinity distance between two points 
 * Linfinity distance is the maximum absolute component-wise difference.
 */
template<int N, typename T>
SH::ShVariableN<1, T> lInfDistance(const SH::ShVariableN<N, T>& a, const SH::ShVariableN<N, T>& b) {
  SH::ShVariableN<N, T> diff = abs(a - b);
  SH::ShVariableN<1, T> result = max(diff(0), diff(1));
  for(int i = 2; i < N; ++i) result = max(result, diff(i));
  return result;
}

static SH::ShAttrib4f hasha(M_PI * M_PI * M_PI * M_PI, std::exp(4.0), 
                        std::pow(13.0, M_PI / 2.0), std::sqrt(1997.0));
static SH::ShAttrib4f hashm(std::sqrt(2.0), 1.0 / M_PI, std::sqrt(3.0), 
                        std::exp(-1.0));

static const int LCG_REPS = 5; // total instructions for hashlcg will be LCG_REPS * 2 + 2
/** \brief Parallel linear congruential generator
 * 
 * This does not work very well right now.  Use hashmrg instead.
 *
 * \sa  template<int N, typename T> SH::ShVariableN<N, T> hashmrg(const SH::ShVariableN<N, T>& p)
 */
// TODO: may not work as intended on 24-bit floats
// since there may not be enough precision 
template<int N, typename T>
SH::ShVariableN<N, T> hashlcg(const SH::ShVariableN<N, T>& p) {
  SH::ShAttrib<N, SH_VAR_TEMP, T> result = frac(p * 0.01);

  SH::ShVariableN<N, T> a(hasha.node(), SH::ShSwizzle(N), false);
  SH::ShVariableN<N, T> m(hashm.node(), SH::ShSwizzle(N), false);

  for(int i = 0; i < LCG_REPS; ++i) {
    result = frac(mad(result, a, m)); 
  }
  return result;
}


static const int MRG_REPS = 2; // total instructions for hashmrg will be MRG_REPS * N * 2 + 2 
/** \brief MRG style pseudorandom vector generator
 * Generates a random vector using a multiple-recursive generator style. (LCG on steroids)
 * Treat x,y,z,w as seeds a0, a1, a2, a3
 * and repeatedly apply an = b * (an-1, an-2, an-3, an-4), where b is a vector
 * Take as output (an, an-1, an-2, an3) after suitable number of iterations.
 *
 * This appears to reduce correlation in the output components when input components are 
 * similar, but the behaviour needs to be studied further.
 *
 * \sa template<int N, typename T> SH::ShVariableN<N, T> hashlcg(const SH::ShVariableN<N, T>& p)
 */
template<int N, typename T>
SH::ShVariableN<N, T> hashmrg(const SH::ShVariableN<N, T>& p) {
  SH::ShAttrib<N, SH_VAR_TEMP, T> result = frac(p * 0.01);
  SH::ShVariableN<N, T> a(hasha.node(), SH::ShSwizzle(N), false); // use only first N elements of hasha

  for(int i = 0; i < MRG_REPS; ++i) {
    for(int j = 0; j < N; ++j) { 
      result(j) = frac(dot(result, a));
    }
  }
  return result;
}

}

#endif // SHUTIL_SHUTILLIB_HPP 
