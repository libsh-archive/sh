// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "Lib.hpp" // LibNoise needs to be included from within Lib

#ifndef SHLIBNOISE_HPP
#define SHLIBNOISE_HPP

#ifndef _WIN32
namespace SH {

/** \defgroup lib_hash Hash functions
 * @ingroup library
 * @{
 */

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
 * \sa template<int N, int M, typename T> Generic<N, T> hashlcg(const Generic<M, T>& p)
 */
template<int N, int M, typename T>
Generic<N, T> hash(const Generic<M, T>& p); 

template<int N, int M, typename T>
Generic<N, T> texhash(const Generic<M, T>& p); 

// Returns summed octaves of cell noise 
// @{
template<int N, int M, typename T>
Generic<N, T> cellnoise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> cellnoise(const Generic<M, T1> &p, 
                              const Generic<K, T2> &amp, 
                              bool useTexture = true);
// @}

// Returns summed octaves of signed cell noise 
// @{
template<int N, int M, typename T>
Generic<N, T> scellnoise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> scellnoise(const Generic<M, T1> &p, 
                               const Generic<K, T2> &amp,
                               bool useTexture = true); 
// @}
// @}


/** \defgroup lib_noise Noise functions
 * @ingroup library
 * @{
 */

// Returns summed octaves of Perlin noise
// @{
template<int N, int M, typename T>
Generic<N, T> noise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> noise(const Generic<M, T1> &p,
                          const Generic<K, T2> &amp,
                          bool useTexture = true);
// @}

// Returns summed octaves of signed Perlin noise
// @{
template<int N, int M, typename T>
Generic<N, T> snoise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> snoise(const Generic<M, T1> &p,
                          const Generic<K, T2> &amp,
                          bool useTexture = true);
// @}

// Returns summed octaves of Perlin improved noise
// @{
template<int N, int M, typename T>
Generic<N, T> perlin(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> perlin(const Generic<M, T1> &p,
                           const Generic<K, T2> &amp,
                           bool useTexture = true);
// @}

// Returns summed octaves of signed Perlin improved noise
// @{
template<int N, int M, typename T>
Generic<N, T> sperlin(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> sperlin(const Generic<M, T1> &p, 
                            const Generic<K, T2> &amp, 
                            bool useTexture = true);
// @}

// Returns summed octaves of bilinearly interpolated cell noise 
// @{
template<int N, int M, typename T>
Generic<N, T> linnoise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> linnoise(const Generic<M, T1> &p, 
                             const Generic<K, T2> &amp, 
                             bool useTexture = true);
// @}

// Returns summed octaves of bilinearly interpolated signed cell noise 
// @{
template<int N, int M, typename T>
Generic<N, T> slinnoise(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> slinnoise(const Generic<M, T1> &p, 
                              const Generic<K, T2> &amp,
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
template<int N, int M, typename T>
Generic<N, T> turbulence(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> turbulence(const Generic<M, T1> &p, 
                               const Generic<K, T2> &amp, 
                               bool useTexture = true);
// @}

/** \brief Turbulence functions sum several octaves of Perlin noise. 
 * @{
 */
template<int N, int M, typename T>
Generic<N, T> sturbulence(const Generic<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
Generic<N, CT1T2> sturbulence(const Generic<M, T1> &p,
                                const Generic<K, T2> &amp, 
                                bool useTexture = true);
// @}

// @}

} // namespace SH
#endif // _WIN32

#include "LibNoiseImpl.hpp"

#endif // SHLIBNOISE_HPP
