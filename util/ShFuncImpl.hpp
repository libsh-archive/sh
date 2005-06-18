// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHUTIL_FUNCIMPL_HPP 
#define SHUTIL_FUNCIMPL_HPP 

#include <cmath>
#include <numeric>
#include "ShAttrib.hpp"
#include "ShSwizzle.hpp" 
#include "ShVariable.hpp"
#include "ShFunc.hpp"
#include "ShLib.hpp"

namespace ShUtil {

using namespace SH;

template<int N, typename T>
ShGeneric<N, T> deprecated_smoothstep(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b,
    const ShGeneric<N, T> x) {
  ShGeneric<N, T> t = (x - a) / (b - a);
  // TODO fix this for other types
  t = clamp(t, 0.0f, 1.0f); 
  return t * t * mad(-2.0f, t, ShConstAttrib1f(3.0f));
}

static const int LCG_REPS = 5; // total instructions for hashlcg will be LCG_REPS * 2 + 2
/** \brief Parallel linear congruential generator
 * 
 * This does not work very well right now.  Use hashmrg instead.
 *
 * \sa  template<int N, typename T> ShGeneric<N, T> hashmrg(const ShGeneric<N, T>& p)
 */
// TODO: may not work as intended on 24-bit floats
// since there may not be enough precision 
template<int N, typename T>
ShGeneric<N, T> hashlcg(const ShGeneric<N, T>& p) {
  ShAttrib<N, SH_TEMP, T> result = frac(p * 0.01);

  // TODO fix this for long tuples
  ShGeneric<N, T> a = fillcast<N>(
      ShConstAttrib4f(M_PI * M_PI * M_PI * M_PI, std::exp(4.0), 
          std::pow(13.0, M_PI / 2.0), std::sqrt(1997.0)));
  ShGeneric<N, T> m = fillcast<N>(
      ShConstAttrib4f(std::sqrt(2.0), 1.0 / M_PI, std::sqrt(3.0), 
          std::exp(-1.0)));

  for(int i = 0; i < LCG_REPS; ++i) result = frac(mad(result, a, m)); 
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
 * \sa template<int N, typename T> ShGeneric<N, T> hashlcg(const ShGeneric<N, T>& p)
 */
template<int N, typename T>
ShGeneric<N, T> hashmrg(const ShGeneric<N, T>& p) {
  ShAttrib<N, SH_TEMP, T> result = frac(p * 0.01);
  ShGeneric<N, T> a = fillcast<N>(
      ShConstAttrib4f(M_PI * M_PI * M_PI * M_PI, std::exp(4.0), 
          std::pow(13.0, M_PI / 2.0), std::sqrt(1997.0)));

  for(int i = 0; i < MRG_REPS; ++i) {
    for(int j = 0; j < N; ++j) { 
      result(j) = frac(dot(result, a));
    }
  }
  return result;
}

/** \brief Given 3 orthonormal basis vectors b0, b1, b2, specified relative to a coordinate space C, 
 * this does a change of basis on a vector v in space C to the orthonormal basis
 */
template<typename T>
ShGeneric<3, T> changeBasis(const ShGeneric<3, T> &b0, 
    const ShGeneric<3, T> &b1, const ShGeneric<3, T> &b2, const ShGeneric<3, T> &v) {
  ShAttrib<3, SH_TEMP, T> result;
  result(0) = b0 | v;
  result(1) = b1 | v;
  result(2) = b2 | v;
  return result;
}

/** Evaluate cubic Bernstein (Bezier) basis functions.
 */
template <int N, typename T>
ShAttrib4f bernstein_basis_4(const ShGeneric<N, T>& t)
{
   ShAttrib4f r;
   ShAttrib<N,SH_TEMP, T> it = fillcast<N>(ShAttrib<1,SH_TEMP, T>(1.0f)) - t;
   r(0) = it*it*it;
   r(1) = 3.0*it*it*t;
   r(2) = 3.0*it*t*t;
   r(3) = t*t*t;
   return r;
}

/** Evaluate cubic Bezier spline.
 */
template <int N, typename T>
ShGeneric<N, T> bezier(const ShGeneric<N, T>& t, const ShAttrib4f& p)
{
   ShAttrib4f B = bernstein_basis_4(t);
   ShGeneric<N, T> r = B[0] * p[0];
   for (int i=1; i<4; i++) {
      r += B[i] * p[i];  
   }
   return r;
}

}

#endif // SHUTIL_FUNCIMPL_HPP 
