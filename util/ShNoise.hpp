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
#ifndef SHUTIL_NOISE_HPP 
#define SHUTIL_NOISE_HPP 

#include "ShAttrib.hpp"
#include "ShArray.hpp"

namespace ShUtil {

using namespace SH;

/** \file ShNoise.hpp
 * This is an implementation of Perlin noise.
 */

/** \brief A Perlin noise/turbulence generator.
 * M = dimensions of the result (1 <= M <= 4 currently)
 * P = period of the noise texture
 */
template<int M, typename T, int P = 16>
class ShNoise 
{
  public:
    /** \brief Generates a single octave Perlin noise with frequency 1 in each
     * of K dimensions.
     * If useTexture is on, then the pattern repeats at every P cells.
     */
    template<int K>
    static ShGeneric<M, T> noise(const ShGeneric<K, T> &p, bool useTexture);

    /** \brief Generates a cell noise value using unit cube cells 
     * intPoint can be set to true if p is already an integer lattice point */ 
    template<int K>
    static ShGeneric<M, T> cellnoise(const ShGeneric<K, T> &p, bool useTexture,
        bool intPoint);

  private:
    static bool m_init;
    static ShArray3D<ShColor<M, SH_TEMP, T> > noiseTex; ///< pseudorandom 2D perlin noise texture 

    static void init();
};

// Returns one octave of perlin noise
template<int M, int K, typename T>
ShGeneric<M, T> noise(const ShGeneric<K, T> &p, bool useTexture = true);

// Returns one octave of signed perlin noise
template<int M, int K, typename T>
ShGeneric<M, T> snoise(const ShGeneric<K, T> &p, bool useTexture = true);

// Returns one octave of cell noise 
template<int M, int K, typename T>
ShGeneric<M, T> cellnoise(const ShGeneric<K, T> &p, bool useTexture = true, bool intPoint = false);

// Returns one octave of signed cell noise 
template<int M, int K, typename T>
ShGeneric<M, T> scellnoise(const ShGeneric<K, T> &p, bool useTexture = true, bool intPoint = false);

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
ShGeneric<M, T> turbulence(const ShGeneric<N, T> &amp, 
    const ShGeneric<K, T> &p, bool useTexture = true);

/** \brief Turbulence functions sum several octaves of Perlin noise. 
 */
template<int M, int N, int K, typename T>
ShGeneric<M, T> sturbulence(const ShGeneric<N, T> &amp, 
    const ShGeneric<K, T> &p, bool useTexture = true);
}
#include "ShNoiseImpl.hpp" 

#endif
