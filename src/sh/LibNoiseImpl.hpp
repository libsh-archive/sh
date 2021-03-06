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
#ifndef SHLIBNOISEIMPL_HPP
#define SHLIBNOISEIMPL_HPP

#include "LibNoise.hpp"
#include "Array.hpp"
#include "Image3D.hpp"

namespace SH {

static const int MRG_REPS = 2; // total instructions for hashmrg will be MRG_REPS * N * 2 + 2 

template<int N, int M, typename T>
Generic<N, T> hash(const Generic<M, T>& p)
{
  Attrib<N, SH_TEMP, T> result = cast<N>(frac(p * 0.01));
  Generic<N, T> a = fillcast<N>(ConstAttrib4f(M_PI * M_PI * M_PI * M_PI, 
                                                  std::exp(4.0), 
                                                  std::pow(13.0, M_PI / 2.0), 
                                                  std::sqrt(1997.0)));

  for(int i = 0; i < MRG_REPS; ++i) {
    for(int j = 0; j < N; ++j) { 
      result(j) = frac(dot(result, a));
    }
  }
  return result;
}

template<int N, int M, typename T>
Generic<N, T> texhash(const Generic<M, T>& p)
{
  return hash<N>(p);
}

/** \brief A Perlin noise/turbulence generator.
 * M = dimensions of the result (1 <= M <= 4 currently)
 * P = period of the noise texture
 */
template<int M, typename T, int P = 16>
class Noise 
{
public:
  /** \brief Generates a single octave Perlin noise with frequency 1
   * in each of K dimensions.  
   * If useTexture is on, then the pattern * repeats at every P cells.
   */
  template<int K>
  static Generic<M, T> noise(const Generic<K, T> &p, bool useTexture);

  /** \brief Generates a single octave Perlin improved noise with
   * frequency 1 in each of K dimensions.  
   * If useTexture is on, then the pattern repeats at every P cells.
   */
  template<int K>
  static Generic<M, T> perlin(const Generic<K, T> &p, bool useTexture);
  
  /** \brief Generates a cell noise value using unit cube cells  */
  template<int K>
  static Generic<M, T> cellnoise(const Generic<K, T> &p, bool useTexture);
  
  /** \brief Generates a single octave of bilinearly interpolated cellnoise */
  template<int K>
  static Generic<M, T> linnoise(const Generic<K, T> &p, bool useTexture);

private:
  static Attrib<1, SH_CONST, T> constP, invP;
  static bool m_init;
  static Array3D<Attrib<M, SH_TEMP, T, SH_COLOR> > noiseTex; ///< pseudorandom 2D perlin noise texture 
  
  static void init();
};

template<int M, typename T, int P>
Array3D<Attrib<M, SH_TEMP, T, SH_COLOR> > Noise<M, T, P>::noiseTex(P, P, P); // pseudorandom 3D noise texture

template<int M, typename T, int P>
bool Noise<M, T, P>::m_init = false; // whether Perlin is initialized. 

template<int M, typename T, int P>
Attrib<1, SH_CONST, T> Noise<M, T, P>::constP(P);

template<int M, typename T, int P>
Attrib<1, SH_CONST, T> Noise<M, T, P>::invP(1.0 / P);

template<int M, typename T, int P>
void Noise<M, T, P>::init() {
  if(m_init) return;

  int i, j, k, l;

  // generate pseudorand noise noiseImage[x + y * P][z] holds the four
  // 1D gradient components for lattice points (x, y, z), (x, y, z + 1), (x, y + 1, z),
  // and (x, y + 1, z + 1)
#ifdef _WIN32
  srand(13);
#else
  srand48(13);
#endif
  Image3D noiseImage(P, P, P, M);
  for(k = 0; k < P; ++k) {
   for(i = 0; i < P; ++i) for(j = 0; j < P; ++j) for(l = 0; l < M; ++l) {
#ifdef _WIN32
     noiseImage(i, j, k, l) = ((float)rand())/(RAND_MAX+1.0);
#else
     noiseImage(i, j, k, l) = drand48();
#endif
    }
  }
  noiseTex.memory(noiseImage.memory());
}

template<int M, typename T>
Generic<M, T> _psmootht(const Generic<M, T> &t) 
{
  return t * t * t * mad(t, mad(t, 6.0f, fillcast<M>(-15.0f)), fillcast<M>(10.0f)); 
}

template<int M, typename T, int P>
template<int K>
Generic<M, T> Noise<M, T, P>::noise(const Generic<K, T> &p, bool useTexture) 
{
  // TODO: implement the real algorithm
  return perlin(p, useTexture);
}

template<int M, typename T, int P>
template<int K>
Generic<M, T> Noise<M, T, P>::perlin(const Generic<K, T> &p, bool useTexture) 
{
  init();
  int i, j;
  typedef Attrib<K, SH_TEMP, T> TempType;
  typedef Attrib<M, SH_TEMP, T> ResultType;
  typedef Attrib<K, SH_CONST, T> ConstTempType;
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

  typename TempType::host_type flip[K];
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
      grad[i] = hash<M>(intLatticePoint); 
    }
  }

  TempType t = _psmootht(rp); //Noise's improved polynomial interpolant 
  for(i = K - 1; i >= 0; --i) {
    int offset = 1 << i; 
    for(j = 0; j < offset; ++j) {
      grad[j] = lerp(t(i), grad[j+offset], grad[j]); 
    }
  }

  return grad[0];
}

template<int M, typename T, int P>
template<int K>
Generic<M, T> Noise<M, T, P>::cellnoise(const Generic<K, T> &p, bool useTexture)
{
  init();
  Attrib<K, SH_TEMP, T> ip;

  ip = floor(p);

  if( useTexture ) {
    ip = frac(ip * invP);
    return noiseTex(fillcast<3>(ip));
  } 
  return hash<M>(ip);
}

template<int M, typename T, int P>
template<int K>
Generic<M, T> Noise<M, T, P>::linnoise(const Generic<K, T> &p, bool useTexture)
{
  // TODO: implement the real algorithm
  return cellnoise(p, useTexture);
}

#ifdef _WIN32
#define NOISE_WITH_AMP(name) \
template<int N, int M, int K, typename T1, typename T2>\
  Generic<N, CT1T2> name(const Generic<M, T1> &p, const Generic<K, T2> &amp, bool useTexture=true) {\
    Attrib<N, SH_TEMP, CT1T2> result; \
    int freq = 1;\
    result *= DataTypeInfo<CT1T2, HOST>::Zero; \
    for(int i = 0; i < K; ++i, freq *= 2) {\
      result = mad(name<N>(p * freq, useTexture), amp(i), result);\
    }\
    return result;\
  }
#else
#define NOISE_WITH_AMP(name) \
template<int N, int M, int K, typename T1, typename T2>\
  Generic<N, CT1T2> name(const Generic<M, T1> &p, const Generic<K, T2> &amp, bool useTexture) {\
    Attrib<N, SH_TEMP, CT1T2> result; \
    int freq = 1;\
    result *= DataTypeInfo<CT1T2, HOST>::Zero; \
    for(int i = 0; i < K; ++i, freq *= 2) {\
      result = mad(name<N>(p * freq, useTexture), amp(i), result);\
    }\
    return result;\
  }
#endif

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> noise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> noise(const Generic<M, T> &p, bool useTexture) {
#endif
  return Noise<N, T>::noise(p, useTexture);
}
NOISE_WITH_AMP(noise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> snoise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> snoise(const Generic<M, T> &p, bool useTexture) {
#endif
  return mad( noise<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
NOISE_WITH_AMP(snoise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> perlin(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> perlin(const Generic<M, T> &p, bool useTexture) {
#endif
  return Noise<N, T>::perlin(p, useTexture);
}
NOISE_WITH_AMP(perlin);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> sperlin(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> sperlin(const Generic<M, T> &p, bool useTexture) {
#endif
  return mad( perlin<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
NOISE_WITH_AMP(sperlin);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> cellnoise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> cellnoise(const Generic<M, T> &p, bool useTexture) {
#endif
  return Noise<N, T>::cellnoise(p, useTexture);
}
NOISE_WITH_AMP(cellnoise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> scellnoise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> scellnoise(const Generic<M, T> &p, bool useTexture) {
#endif
  return mad( cellnoise<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
NOISE_WITH_AMP(scellnoise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> linnoise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> linnoise(const Generic<M, T> &p, bool useTexture) {
#endif
  return Noise<N, T>::linnoise(p, useTexture);
}
NOISE_WITH_AMP(linnoise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> slinnoise(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> slinnoise(const Generic<M, T> &p, bool useTexture) {
#endif
  return mad( linnoise<N>(p, useTexture), 2.0f, fillcast<N>(-1.0f));
}
NOISE_WITH_AMP(slinnoise);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> turbulence(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> turbulence(const Generic<M, T> &p, bool useTexture) {
#endif
  return abs(sperlin<N>(p, useTexture));
}
NOISE_WITH_AMP(turbulence);

template<int N, int M, typename T>
#ifdef _WIN32
Generic<N, T> sturbulence(const Generic<M, T> &p, bool useTexture=true) {
#else
Generic<N, T> sturbulence(const Generic<M, T> &p, bool useTexture) {
#endif
  return mad(abs(sperlin<N>(p, useTexture)), 2.0f, fillcast<N>(-1.0f));
}
NOISE_WITH_AMP(sturbulence);

} // namespace SH

#endif // SHLIBNOISEIMPL_HPP
