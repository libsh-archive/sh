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

#ifndef SHUTIL_WORLEYIMPL_HPP 
#define SHUTIL_WORLEYIMPL_HPP

#include <cstdlib>
#include "ShSyntax.hpp"
#include "ShWorley.hpp"
#include "ShNoise.hpp"
#include "ShUtil.hpp"
#include "ShImage.hpp"

namespace ShUtil {

using namespace SH;

static const int DX[] = { 0, 1, 1, 0, -1, -1, -1, 0, 1 };
static const int DY[] = { 0, 0, 1, 1, 1, 0, -1, -1, -1 }; 
static const int DSIZE = 9; 

class Metric {
  private:
    ShWorleyMetric m;
  public:
    Metric(ShWorleyMetric m): m(m) {}
    ShAttrib1f operator()(ShAttrib2f a, ShAttrib2f b) {
      switch(m) {
        case L1: return lOneDistance(a, b);
        case L2: return distance(a, b);
        case L2_SQ: 
        {
          ShAttrib2f temp = a - b;
          return dot(temp, temp);
        }
        case LINF: return lInfDistance(a, b);
      }
    }

};

// TODO split off the point generation section 
//   (can allow weird (i.e. fXXXing slow) methods
//   like interpolating between a sequence of points for animated worley,
//   or using textures for partial data and noise for the rest (i.e. make 
//   some worley cells spell out letters)
//
// Split off sorting section (can sort other data, like cellnoise colour
// geneated for each cell, etc.)
//

template<int N, int Kind1, int Kind2, bool Swizzled1, bool Swizzled2, typename T>
void doWorley(const ShVariableN<2, T> &p, const ShVariableN<N, T> &c, 
    ShWorleyMetric m, ShAttrib<1, Kind1, T, Swizzled1> &scalarResult, 
    ShAttrib<2, Kind2, T, Swizzled2> &gradientResult, bool useTexture) {
  int i;
  Metric dist(m);

  // get integer lattice point & offset from integer lattice point
  ShAttrib<2, SH_TEMP, T> ip = floor(p);
  ShAttrib<2, SH_TEMP, T> op = frac(p); 

  // find distances & gradients to neighbours
  // adj[0](i) stores distance to point i, adj[1](i), adj[2](i) hold gradient
  ShAttrib<2, SH_TEMP, T> point; 
  ShAttrib<DSIZE, SH_TEMP, T> adj[3]; 
  ShAttrib<2, SH_TEMP, T> gradTemp;
  
  for(i = 0; i < DSIZE; ++i) {
    ShConstant<2, T> offset(DX[i], DY[i]);
    point = cellnoise<2>(ip + offset, useTexture, true) + offset; 

    adj[0](i) = dist(point, op);
    gradTemp = normalize(point - op); 
    adj[1](i) = gradTemp(0);
    adj[2](i) = gradTemp(1);
  }

  // sort points & gradients by distance
  groupEvenOddSort<3>(adj); 

  // weighted sum of basis function values to get final result
  scalarResult = dot(fillcast<N>(adj[0]), c);
  gradientResult(0) = dot(fillcast<N>(adj[1]), c);
  gradientResult(1) = dot(fillcast<N>(adj[2]), c);
}

template<int N, typename T>
ShVariableN<3, T> worley(const ShVariableN<2, T> &p,
    const ShVariableN<N, T> &c, ShWorleyMetric m, bool useTexture) {
  ShAttrib<3, SH_TEMP, T> result;
  ShAttrib<1, SH_TEMP, T> scalar;
  ShAttrib<2, SH_TEMP, T> grad;

  doWorley(p, c, m, scalar, grad, useTexture);

  result(0) = scalar;
  result(1,2) = grad;
  return result;
}

template<int N, typename T>
ShVariableN<1, T> worleyNoGradient(const ShVariableN<2, T> &p,
    const ShVariableN<N, T> &c, ShWorleyMetric m, bool useTexture) {
  ShAttrib<1, SH_TEMP, T> result;
  ShAttrib<2, SH_TEMP, T> dummy;
  doWorley(p, c, m, result, dummy, useTexture); 
  return result;
}

/** Makes a shader that takes 
 *  IN(0) ShAttrib<N,T> coefficients; // worley coefficients
 *  IN(1) ShTexCoord<2,T> texcoord; // texture lookup coordinates
 *
 *  OUT(0) ShAttrib<1,T> scalar 
 *  OUT(1) ShAttrib<2,T> gradient;
 */
template<int N, typename T>
ShProgram worleyProgram(ShWorleyMetric m, bool useTexture) {
  ShProgram program = SH_BEGIN_PROGRAM() {
    ShAttrib<N, SH_INPUT, T> SH_DECL(coefficients);
    ShTexCoord<2, SH_INPUT, T> SH_DECL(texcoord);

    ShAttrib<1, SH_OUTPUT, T> SH_DECL(scalar); 
    ShAttrib<2, SH_OUTPUT, T> SH_DECL(gradient);
    doWorley(texcoord, coefficients, m, scalar, gradient, useTexture); 
  } SH_END_PROGRAM;
  return program;
}

}

#endif
