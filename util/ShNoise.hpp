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
#include "ShTypeInfo.hpp"

// @todo type remove these later when noise becomes part of standard library 
#define CV1V2 ShCommonType<V1, V2>::valueType

namespace ShUtil {


using namespace SH;

/** \file ShNoise.hpp
 * This is an implementation of Perlin noise.
 */

/** \brief A Perlin noise/turbulence generator.
 * M = dimensions of the result (1 <= M <= 4 currently)
 * P = period of the noise texture
 */
template<int M, ShValueType V, int P = 16>
class ShNoise 
{
  public:
    /** \brief Generates a single octave Perlin noise with frequency 1 in each
     * of K dimensions.
     * If useTexture is on, then the pattern repeats at every P cells.
     */
    template<int K>
    static ShGeneric<M, V> perlin(const ShGeneric<K, V> &p, bool useTexture);

    /** \brief Generates a cell noise value using unit cube cells  */
    template<int K>
    static ShGeneric<M, V> cellnoise(const ShGeneric<K, V> &p, bool useTexture);

  private:
    static ShAttrib<1, SH_CONST, V> constP, invP;
    static bool m_init;
    static ShArray3D<ShColor<M, SH_TEMP, V> > noiseTex; ///< pseudorandom 2D perlin noise texture 

    static void init();
};

// Returns summed octaves of Perlin improved noise
// @{
template<int N, int M, ShValueType V>
ShGeneric<N, V> perlin(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> perlin(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, bool useTexture = true);
// @}

// Returns summed octaves of signed Perlin improved noise
// @{
template<int N, int M, ShValueType V>
ShGeneric<N, V> sperlin(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> sperlin(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, bool useTexture = true);
// @}

// Returns summed octaves of cell noise 
// @{
template<int N, int M, ShValueType V>
ShGeneric<N, V> cellnoise(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> cellnoise(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, 
    bool useTexture = true);
// @}

// Returns summed octaves of signed cell noise 
// @{
template<int N, int M, ShValueType V>
ShGeneric<N, V> scellnoise(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> scellnoise(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp,
    bool useTexture = true); 
// @}

// Returns M octaves of turbulence in N-dimensional space (currently 1 <= N <= 4,
// 1 <= M <= 2, 1 <= K <= 4 is supported)
// The ith octave has double the frequency of the (i-1)th octave and 
// is weighted by amp(i).
// 
// The maximum M size supported is determined by the color depth of the noise texture.
// The reason it is set at 2 right now is  
//
// useTexture determines whether procedureal hash is used or texture lookup
// @{
template<int N, int M, ShValueType V>
ShGeneric<N, V> turbulence(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> turbulence(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, 
    bool useTexture = true);
// @}

/** \brief Turbulence functions sum several octaves of Perlin noise. 
 * @{
 */
template<int N, int M, ShValueType V>
ShGeneric<N, V> sturbulence(const ShGeneric<M, V> &p, bool useTexture = true);

template<int N, int M, int K, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> sturbulence(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, 
    bool useTexture = true);
// @}

}
#include "ShNoiseImpl.hpp" 

//@todo type remove these later
#undef CV1V2
#endif
