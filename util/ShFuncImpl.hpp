// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

/** Evaluate cubic Bezier spline.
 */
template <int N, typename T>
ShGeneric<N, T> bezier(const ShGeneric<N, T>& t, const ShAttrib4f& p)
{
   ShAttrib4f B = bernstein<4>(t(0));
   ShGeneric<N, T> r = B[0] * p[0];
   for (int i=1; i<4; i++) {
      r += B[i] * p[i];  
   }
   return r;
}

}

#endif // SHUTIL_FUNCIMPL_HPP 
