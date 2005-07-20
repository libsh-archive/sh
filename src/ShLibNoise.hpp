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
#include "ShLib.hpp" // ShLibNoise needs to be included from within ShLib

#ifndef SHLIBNOISE_HPP
#define SHLIBNOISE_HPP

#ifndef WIN32
namespace SH {

/** \defgroup lib_noise Noise functions
 * @ingroup library
 * @{
 */

// Returns summed octaves of Perlin improved noise
// @{
template<int N, int M, typename T>
ShGeneric<N, T> perlin(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> perlin(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp, bool useTexture = true);
// @}

// Returns summed octaves of signed Perlin improved noise
// @{
template<int N, int M, typename T>
ShGeneric<N, T> sperlin(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> sperlin(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp, bool useTexture = true);
// @}

// Returns summed octaves of cell noise 
// @{
template<int N, int M, typename T>
ShGeneric<N, T> cellnoise(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> cellnoise(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp, 
    bool useTexture = true);
// @}

// Returns summed octaves of signed cell noise 
// @{
template<int N, int M, typename T>
ShGeneric<N, T> scellnoise(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> scellnoise(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp,
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
ShGeneric<N, T> turbulence(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> turbulence(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp, 
    bool useTexture = true);
// @}

/** \brief Turbulence functions sum several octaves of Perlin noise. 
 * @{
 */
template<int N, int M, typename T>
ShGeneric<N, T> sturbulence(const ShGeneric<M, T> &p, bool useTexture = true);

template<int N, int M, int K, typename T1, typename T2>
ShGeneric<N, CT1T2> sturbulence(const ShGeneric<M, T1> &p, const ShGeneric<K, T2> &amp, 
    bool useTexture = true);
// @}

// @}

} // namespace SH
#endif // WIN32

#include "ShLibNoiseImpl.hpp"

#endif // SHLIBNOISE_HPP
