// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
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
#ifndef SHUTIL_NOISEIMPL_HPP
#define SHUTIL_NOISEIMPL_HPP

#include <cstdlib>
#include "ShDebug.hpp"
#include "ShNoise.hpp"
#include "ShFunc.hpp"
#include "ShImage3D.hpp"
#include "ShLib.hpp"

namespace ShUtil {

using namespace SH;


template<int M, ShValueType V, int P>
ShArray3D<ShColor<M, SH_TEMP, V> > ShNoise<M, V, P>::noiseTex(P, P, P); // pseudorandom 3D noise texture

template<int M, ShValueType V, int P>
bool ShNoise<M, V, P>::m_init = false; // whether Perlin is initialized. 

template<int M, ShValueType V, int P>
ShAttrib<1, SH_CONST, V> ShNoise<M, V, P>::constP(P);

template<int M, ShValueType V, int P>
ShAttrib<1, SH_CONST, V> ShNoise<M, V, P>::invP(1.0 / P);

template<int M, ShValueType V, int P>
void ShNoise<M, V, P>::init() {
  if(m_init) return;

  int i, j, k, l;

  // generate pseudorand noise noiseImage[x + y * P][z] holds the four
  // 1D gradient components for lattice points (x, y, z), (x, y, z + 1), (x, y + 1, z),
  // and (x, y + 1, z + 1)
#ifdef WIN32
  srand(13);
#else
  srand48(13);
#endif
  ShImage3D noiseImage(P, P, P, M);
  for(k = 0; k < P; ++k) {
   for(i = 0; i < P; ++i) for(j = 0; j < P; ++j) for(l = 0; l < M; ++l) {
#ifdef WIN32
     noiseImage(i, j, k, l) = ((float)rand())/(RAND_MAX+1.0);
#else
     noiseImage(i, j, k, l) = drand48();
#endif
    }
  }
  noiseTex.memory(noiseImage.memory());
}

template<int M, ShValueType V>
ShGeneric<M, V> _psmootht(const ShGeneric<M, V> &t) 
{
  return t * t * t * mad(t, mad(t, 6.0f, fillcast<M>(-15.0f)), fillcast<M>(10.0f)); 
}

template<int M, ShValueType V, int P>
template<int K>
ShGeneric<M, V> ShNoise<M, V, P>::perlin(const ShGeneric<K, V> &p, bool useTexture) 
{
  init();
  int i, j;
  typedef ShAttrib<K, SH_TEMP, V> TempType;
  typedef ShAttrib<M, SH_TEMP, V> ResultType;
  typedef ShAttrib<K, SH_CONST, V> ConstTempType;
  static const int NUM_SAMPLES = 1 << K;

  TempType rp = frac(p); // offset from integer lattice point
  TempType p0, p1; // positive coordinates in [0, P)^3
  TempType ip0, ip1; // integer lattice point in [0,P)^3 for hash, [0,1)^3 for tex lookup

  p0 = frac(p * invP) * constP; 
  p1 = frac(mad(p, invP, fillcast<K>(invP))) * constP;
  ip0 = floor(p0);
  ip1 = floor(p1);
  if(useTexture) { // convert to tex coordiantes (TODO remove when we have RECT textures)
    ip0 = (ip0 + 0.5f) * invP; 
    ip1 = (ip1 + 0.5f) * invP; 
  } 

  // find gradients at the NUM_SAMPLES adjacent grid points (NUM_SAMPLES = 2^K for dimension K lookup)
  ResultType grad[NUM_SAMPLES]; 

  typename TempType::HostType flip[K];
  for(i = 0; i < NUM_SAMPLES; ++i) {
    for(j = 0; j < K; ++j) {
      if(j == 0) flip[j] = i & 1;
      else flip[j] = (i >> j) & 1;
    }
    ConstTempType offsets(flip);
    TempType intLatticePoint = lerp(offsets, ip1, ip0);
    if(useTexture) {
      grad[i] = noiseTex(fillcast<3>(intLatticePoint)); // lookup 3D texture
    } else {
      grad[i] = cast<M>(hashmrg(intLatticePoint)); 
    }
  }

  TempType t = _psmootht(rp); //ShNoise's improved polynomial interpolant 
  for(i = K - 1; i >= 0; --i) {
    int offset = 1 << i; 
    for(j = 0; j < offset; ++j) {
      grad[j] = lerp(t(i), grad[j+offset], grad[j]); 
    }
  }

  return grad[0];
}

template<int M, ShValueType V, int P>
template<int K>
ShGeneric<M, V> ShNoise<M, V, P>::cellnoise(const ShGeneric<K, V> &p, bool useTexture)
{
  init();
  ShAttrib<K, SH_TEMP, V> ip;

  ip = floor(p);

  if( useTexture ) {
    ip = frac(ip * invP);
    return noiseTex(fillcast<3>(ip));
  } 
  return fillcast<M>(hashmrg(ip));
}

#define SHNOISE_WITH_AMP(name) \
template<int N, int M, int K, ShValueType V1, ShValueType V2>\
  ShGeneric<N, CV1V2> name(const ShGeneric<M, V1> &p, const ShGeneric<K, V2> &amp, bool useTexture) {\
    ShAttrib<N, SH_TEMP, CV1V2> result; \
    int freq = 1;\
    result *= ShDataTypeInfo<CV1V2, SH_HOST>::Zero; \
    for(int i = 0; i < K; ++i, freq *= 2) {\
      result = mad(name<N>(p * freq, useTexture), amp(i), result);\
    }\
    return result;\
  }

template<int N, int M, ShValueType V>
ShGeneric<N, V> perlin(const ShGeneric<M, V> &p, bool useTexture) {
  return ShNoise<N, V>::perlin(p, useTexture);
}
SHNOISE_WITH_AMP(perlin);

template<int N, int M, ShValueType V>
ShGeneric<N, V> sperlin(const ShGeneric<M, V> &p, bool useTexture) {
  return mad( perlin<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
SHNOISE_WITH_AMP(sperlin);

template<int N, int M, ShValueType V>
ShGeneric<N, V> cellnoise(const ShGeneric<M, V> &p, bool useTexture) {
  return ShNoise<N, V>::cellnoise(p, useTexture);
}
SHNOISE_WITH_AMP(cellnoise);

template<int N, int M, ShValueType V>
ShGeneric<N, V> scellnoise(const ShGeneric<M, V> &p, bool useTexture) {
  return mad( cellnoise<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
SHNOISE_WITH_AMP(scellnoise);


template<int N, int M, ShValueType V>
ShGeneric<N, V> turbulence(const ShGeneric<M, V> &p, bool useTexture) {
  abs(sperlin<N>(p, useTexture));
}
SHNOISE_WITH_AMP(turbulence);

template<int N, int M, ShValueType V>
ShGeneric<N, V> sturbulence(const ShGeneric<M, V> &p, bool useTexture) {
  return mad(abs(sperlin<N>(p, useTexture)), 2.0f, fillcast<N>(-1.0f));
}
SHNOISE_WITH_AMP(sturbulence);

} // namespace ShUtil

#endif
