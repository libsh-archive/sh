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
#ifndef SHUTIL_WORLEY_HPP 
#define SHUTIL_WORLEY_HPP

#include "ShAttrib.hpp"
#include "ShColor.hpp"
#include "ShTexture.hpp"

namespace ShUtil {

using namespace SH;
/** \file ShWorley.hpp
 * This is an implementation of 2D ShWorley texture.
 */

/** Kinds of metrics used for finding nearest neighbours.
 */
enum ShWorleyMetric {
  L1, ///< Sum of absolute componentwise differences
  L2, ///< Euclidean metric
  L2_SQ,  ///< Euclidean metric squared
  LINF, ///< Maximum absolute difference
};

// Point Generator 

template<typename T>
class ShWorleyDefaultPointGen {
  public:
    ShWorleyDefaultPointGen() {}
    ShGeneric<2, T> operator()(const ShGeneric<2, T> &p, bool useTexture) const; 
};

template<typename T>
class ShWorleyLerpingPointGen {
  public:
    ShWorleyLerpingPointGen(const ShGeneric<1, T> &time);
    ShGeneric<2, T> operator()(const ShGeneric<2, T> &p, bool useTexture) const; 

  private:
    const ShGeneric<1, T> &m_time;
};

/** \brief Worley texture generator.
 * This finds the 1 <= N <= 9 nearest neighbours to the point p using metric m 
 * and returns the sum of the distances, weighted by c.
 *
 * The weighted sum of the gradients associated with nearest neighbour points
 * is returned in components (1,2) of the result.
 *
 * Note that for GPU implementation, the number of points found & sorted for
 * each lookup must be equal and relatively small.  So cellnoise is used
 * to generate one point in each grid cell, and only a cell and it's 8
 * adjacent 2D neighbours are checked.  
 *
 * Unfortunately, this means that the result may not even be C0 continuous
 * since the closest point may in fact not be in any of the cells searched;
 * however, the probability of this happening with uniformly distributed
 * points is small.  With higher values of N, the probability of 
 * discontinuity increases.
 * @{
 */
template<int N, typename T, typename PointGen> 
ShGeneric<3, T> worley(const ShGeneric<2, T> &p, 
    const ShGeneric<N, T> &c, ShWorleyMetric m, bool useTexture, 
    const PointGen &generator); 

template<int N, typename T>
ShGeneric<3, T> worley(const ShGeneric<2, T> &p, 
    const ShGeneric<N, T> &c, ShWorleyMetric m = L2_SQ, bool useTexture = true); 

template<int N, typename T, typename PointGen> 
ShGeneric<1, T> worleyNoGradient(const ShGeneric<2, T> &p, 
    const ShGeneric<N, T> &c, ShWorleyMetric m, bool useTexture,
    const PointGen &generator); 

template<int N, typename T>
ShGeneric<1, T> worleyNoGradient(const ShGeneric<2, T> &p, 
    const ShGeneric<N, T> &c, ShWorleyMetric m = L2_SQ, bool useTexture = true);
//@}
    
/** Makes a shader that takes 
 *  IN(0) ShAttrib<N,T> coefficients; // worley coefficients
 *  IN(1) ShTexCoord<2,T> texcoord; // texture lookup coordinates
 *
 *  OUT(0) ShAttrib<1,T> scalar 
 *  OUT(1) ShAttrib<2,T> gradient;
 * @{
 */
template<int N, typename T, typename PointGen> 
ShProgram worleyProgram(ShWorleyMetric m, bool useTexture,
    const PointGen &generator); 

template<int N, typename T>
ShProgram worleyProgram(ShWorleyMetric m = L2_SQ, bool useTexture = true);
//@}


} // namespace ShUtil

#endif
