// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Bryan Chan, Michael D. McCool
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
#ifndef SHUTIL_SHUTILIMPL_HPP 
#define SHUTIL_SHUTILIMPL_HPP 

#include <cmath>
#include <numeric>
#include "ShAttrib.hpp"
#include "ShConstant.hpp"
#include "ShSwizzle.hpp" 
#include "ShVariable.hpp"
#include "ShUtil.hpp"

namespace ShUtil {

using namespace SH;

template<typename T>
ShVariableN<1, T> clamp(const ShVariableN<1, T>& a, const ShVariableN<1, T>& b,
    const ShVariableN<1, T>& x) {
  return min(max(x, a), b); 
}

template<typename T>
ShVariableN<1, T> clamp(T a, T b, const ShVariableN<1, T>& x) {
  return min(max(x, ShConstant<1,T>(a)), ShConstant<1,T>(b)); 
}

template<int N, typename T>
ShVariableN<N, T> smoothstep(const ShVariableN<N, T>& a, const ShVariableN<N, T>& b,
    const ShVariableN<N, T> x) {
  ShVariableN<N, T> t = (x - a) / (b - a);
  // TODO fix this for other types
  t = clamp(0.0f, 1.0f, t); 
  return t * t * mad(-2.0f, t, ShConstant1f(3.0f));
}

/** \brief Euclidean distance between two points.
 */
template<int N, typename T>
ShVariableN<1, T> distance(const ShVariableN<N, T>& a, const ShVariableN<N, T>& b) {
  ShVariableN<N, T> diff = abs(a - b);
  return sqrt(dot(diff, diff));
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
  ShAttrib<N, SH_TEMP, T> result = frac(p * 0.01);

  // TODO fix this for long tuples
  ShVariableN<N, T> a = fillcast<N>(
      ShConstant4f(M_PI * M_PI * M_PI * M_PI, std::exp(4.0), 
          std::pow(13.0, M_PI / 2.0), std::sqrt(1997.0)));
  ShVariableN<N, T> m = fillcast<N>(
      ShConstant4f(std::sqrt(2.0), 1.0 / M_PI, std::sqrt(3.0), 
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
 * \sa template<int N, typename T> ShVariableN<N, T> hashlcg(const ShVariableN<N, T>& p)
 */
template<int N, typename T>
ShVariableN<N, T> hashmrg(const ShVariableN<N, T>& p) {
  ShAttrib<N, SH_TEMP, T> result = frac(p * 0.01);
  ShVariableN<N, T> a = fillcast<N>(
      ShConstant4f(M_PI * M_PI * M_PI * M_PI, std::exp(4.0), 
          std::pow(13.0, M_PI / 2.0), std::sqrt(1997.0)));

  for(int i = 0; i < MRG_REPS; ++i) {
    for(int j = 0; j < N; ++j) { 
      result(j) = frac(dot(result, a));
    }
  }
  return result;
}

template<int N, int Kind, typename T>
ShAttrib<N, Kind, T> evenOddSort(const ShAttrib<N, Kind, T>& v) {
  ShAttrib<N, Kind, T> result = v;
  groupEvenOddSort<1>(&result);
  return result;
}

/** \brief Sorts groups of components v[i](j), 0 <= i < S
 *  * by the components in v[0](j) 0 <= j < N.
 *   * This also uses an even-odd transposition sort.
 *    */
template<int S, int N, int Kind, typename T>
void groupEvenOddSort(ShAttrib<N, Kind, T> v[]) {
  const int NE = (N + 1) / 2; // number of even elements
  const int NO = N / 2; // number of odd elements
  const int NU = NO; // number of components to compare for (2i, 2i+1) comparisons
  const int ND = NE - 1; // number of componnets to compare for (2i, 2i-1) comparisons

  int i, j;
  // hold even/odd temps and condition code for (2i, 2i+1) "up" and (2i, 2i-1) "down" comparisons 
  ShAttrib<NU, SH_TEMP, T> eu, ou, ccu; 
  ShAttrib<ND, SH_TEMP, T> ed, od, ccd; 

  // even and odd swizzle (elms 0..NE-1 are the "even" subsequence, NE..N-1 "odd")
  int eswiz[NE], oswiz[NO]; 
  for(i = 0; i < NE; ++i) eswiz[i] = i;
  for(i = 0; i < NO; ++i) oswiz[i] = NE + i;

  for(i = 0; i < NE; ++i) { 
    // TODO note the interesting syntax (does appear to be C++ standard) 
    // that's required so that the gcc parser does 
    // not crap out on the template swiz code
    //
    // if this doesn't work on other platforms, we may have to
    // rewrite the swiz function

    // compare 2i, 2i+1
    eu = v[0].template swiz<NU>(eswiz);
    ou = v[0].template swiz<NU>(oswiz);
    if (S > 1) ccu = eu < ou; 
    v[0].template swiz<NU>(eswiz) = min(eu, ou); 
    v[0].template swiz<NU>(oswiz) = max(eu, ou); 

    for(j = 1; j < S; ++j) {
      eu = v[j].template swiz<NU>(eswiz);
      ou = v[j].template swiz<NU>(oswiz);
      v[j].template swiz<NU>(eswiz) = cond(ccu, eu, ou); 
      v[j].template swiz<NU>(oswiz) = cond(ccu, ou, eu); 
    }

    // compare 2i, 2i-1
    ed = v[0].template swiz<ND>(eswiz + 1);
    od = v[0].template swiz<ND>(oswiz);
    if (S > 1) ccd = ed > od; 
    v[0].template swiz<ND>(eswiz + 1) = max(ed, od);
    v[0].template swiz<ND>(oswiz) = min(ed, od);

    for(j = 1; j < S; ++j) {
      ed = v[j].template swiz<ND>(eswiz + 1);
      od = v[j].template swiz<ND>(oswiz);
      v[j].template swiz<ND>(eswiz + 1) = cond(ccd, ed, od); 
      v[j].template swiz<ND>(oswiz) = cond(ccd, od, ed); 
    }
  }

  // reswizzle "even" to 0, 2, 4,... "odd" to 1, 3, 5, ..
  int resultEswiz[NE], resultOswiz[NO]; 
  for(i = 0; i < NE; ++i) resultEswiz[i] = i * 2;
  for(i = 0; i < NO; ++i) resultOswiz[i] = i * 2 + 1; 
  for(i = 0; i < S; ++i) {
    ShAttrib<NE, SH_TEMP, T> evens = v[i].template swiz<NE>(eswiz);
    v[i].template swiz<NO>(resultOswiz) = v[i].template swiz<NO>(oswiz);
    v[i].template swiz<NE>(resultEswiz) = evens;
  }
}


}

#endif // SHUTIL_SHUTILLIB_HPP 