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
#ifndef SHUTIL_PERLIN_HPP 
#define SHUTIL_PERLIN_HPP 

#include "ShAttrib.hpp"
#include "ShTexture.hpp"

namespace ShUtil {

using namespace SH;

/** \file ShPerlin.hpp
 * This is an implementation of Perlin noise.
 */

/** \brief A Perlin noise/turbulence generator.
 * M = dimensions of the result (1 <= M <= 4 currently)
 * P = period of the noise texture
 */
template<int M, int P = 32>
class ShPerlin
{
  public:
    /** \brief Generates a single octave Perlin noise.
     */
    template<int K, typename T> 
    static ShVariableN<M, T> noise(const ShVariableN<K, T> &p, bool useTexture);

  private:
    static bool m_init;
    static ShTexture3D<ShColor<M, SH_VAR_TEMP> > noiseTex; ///< pseudorandom 3D perlin noise texture 

    static void init();
};

// Returns one octave of perlin noise
template<int M, int K, typename T>
ShVariableN<M, T> noise(const ShVariableN<K, T> &p, bool useTexture = true);

// Returns one octave of signed perlin noise
template<int M, int K, typename T>
ShVariableN<M, T> snoise(const ShVariableN<K, T> &p, bool useTexture = true);

// Returns M octaves of turbulence in N-dimensional space (currently 1 <= N <= 4,
// 1 <= M <= 2, 1 <= K <= 4 is supported)
// The ith octave has double the frequency of the (i-1)th octave and 
// is weighted by amp(i).
// 
// The maximum M size supported is determined by the color depth of the noise texture.
// The reason it is set at 2 right now is  
//
// useTexture determines whether procedureal hash is used or texture lookup
template<int M, int N, int K, typename T>
ShVariableN<M, T> turbulence(const ShVariableN<N, T> &amp, 
    const ShVariableN<K, T> &p, bool useTexture = true);

/** \brief Turbulence functions sum several octaves of Perlin noise. 
 */
template<int M, int N, int K, typename T>
ShVariableN<M, T> sturbulence(const ShVariableN<N, T> &amp, 
    const ShVariableN<K, T> &p, bool useTexture = true);
}
#include "ShPerlinImpl.hpp" 

#endif
