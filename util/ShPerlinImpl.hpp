// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef SHUTIL_PERLINIMPL_HPP
#define SHUTIL_PERLINIMPL_HPP

#include <cstdlib>
#include "ShDebug.hpp"
#include "ShPerlin.hpp"
#include "ShUtilLib.hpp"

namespace ShUtil {

using namespace SH;

#define PERM_SIZE 32
#define DPERM_SIZE ( ( double ) PERM_SIZE )
#define INV_PERM_SIZE ( 1.0 / ( double )PERM_SIZE )

template<int M>
ShTexture3D<ShColor<M, SH_VAR_TEMP> > ShPerlin<M>::noiseTex( PERM_SIZE, PERM_SIZE, PERM_SIZE ); // pseudorandom 3D noise texture

template<int M>
bool ShPerlin<M>::m_init = false; // whether Perlin is initialized. 

template<int M>
void ShPerlin<M>::init() {
  if( m_init ) return;


  int i, j, k, l;
  srand48( 13 );

  // generate pseudorandom noise noiseImage[x + y * PERM_SIZE][z] holds the four
  // 1D gradient components for lattice points (x, y, z), (x, y, z + 1), (x, y + 1, z),
  // and (x, y + 1, z + 1 )
  double grads[ PERM_SIZE ][ PERM_SIZE ][ PERM_SIZE ][M];
  for( i = 0; i < PERM_SIZE; ++i ) for( j = 0; j < PERM_SIZE; ++j ) for( k = 0; k < PERM_SIZE; ++k ) 
  for( l = 0; l < M ; ++l ) {
    grads[i][j][k][l] = drand48();
  }

  // load one slice at a time
  ShImage noiseImage( PERM_SIZE, PERM_SIZE, M );
  for( k = 0; k < PERM_SIZE; ++k ) for( i = 0; i < PERM_SIZE; ++i ) for( j = 0; j < PERM_SIZE; ++j ) {
    for( l = 0; l < M; ++l ) {
      noiseImage( i, j, l ) = grads[i][j][k][l]; 
    }
    noiseTex.load( noiseImage, k ); 
  }
}

// runs x * d + e, d may be an Attrib or a float 
template<int M, typename T>
ShVariableN<M, T> _pmad(const ShVariableN<M, T> &x, T d, T e) {
  T vals[M];
  for( int i = 0; i < M; ++i ) vals[i] = e;
  ShConstant<M, T> ec( vals );
  return mad( x, d, ec);
}

template<int M, typename T>
ShVariableN<M, T> _pmad(const ShVariableN<M, T> &x, const ShVariableN<M, T> &d, T e) {
  T vals[M];
  for( int i = 0; i < M; ++i ) vals[i] = e;
  ShConstant<M, T> ec( vals );
  return mad( x, d, ec);
}

template<int M, typename T>
ShVariableN<M, T> _psmootht(const ShVariableN<M, T> &t ) 
{
  return t * t * t * _pmad( t, _pmad( t, 6.0, -15.0 ), 10.0); 
}

// adds x to a float d componentwise
template<int M, typename T>
ShVariableN<M, T> _padd(const ShVariableN<M, T> &x, T d) {
  T vals[M];
  for( int i = 0; i < M; ++i ) vals[i] = d;
  ShConstant<M, T> c( vals );
  return x + c;
}

/* casts into the result type using a very simple algorithm
 * that maintains the continuity of the noise.
 * padds with zeros for K > M, takes first M elements if M < K
 */
template<int M, int K, typename T>
ShVariableN<M, T> _pcast(const ShVariableN<K, T> &x) {
  ShAttrib<M, SH_VAR_TEMP, T> result;

  if( M == K ) {
    ShStatement stmt(result, SH_OP_ASN, x );
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  } else if( M < K ) {
    SH_DEBUG_PRINT( "Warning: casting down" );
    for( int i = 0; i < M; ++i ) result(i) = x(i);
    // TODO hope arbitrary swizzling works soon
  } else if( M > K ) {
    SH_DEBUG_PRINT( "Warning: casting up" );
    for( int i = 0; i < K; ++i ) result(i) = x(i); 
    // TODO hope arbitrary swizzling works soon
  } 
  return result;
}

template<int M>
template<int K, typename T>
ShVariableN<M, T> ShPerlin<M>::noise( const ShVariableN<K, T> &p, bool useTexture ) 
{
  init();
  std::cout << "noise M: " << M << " K: " << K << "T: " << std::endl;
  int i, j, k;
  typedef ShAttrib<K, SH_VAR_TEMP, T> TempType;
  typedef ShAttrib<M, SH_VAR_TEMP, T> ResultType;
  typedef ShConstant<K, T> ConstTempType;
  static const int NUM_SAMPLES = 1 << K;

  TempType rp = frac( p ); // offset from integer lattice point 
  TempType p0 = frac( p * INV_PERM_SIZE ) * DPERM_SIZE; // positive coordinates in [0, PERM_SIZE)
  TempType p1 = frac( _pmad(p, INV_PERM_SIZE, INV_PERM_SIZE )) * DPERM_SIZE; // positive coordinates in [0, PERM_SIZE)

  // get integer lattice point, and scale [0, PERM_SIZE]^3 space to [0,1]^3 for permutation by the texture
  TempType ip0 = _padd( floor( p0 ), 0.5 ) * INV_PERM_SIZE; 
  TempType ip1 = _padd( floor( p1 ), 0.5 ) * INV_PERM_SIZE; 


  // find gradients at the NUM_SAMPLES adjacent grid points (NUM_SAMPLES = 2^K for dimension K lookup)
  ResultType grad[NUM_SAMPLES]; 

  T flip[K];
  for( i = 0; i < NUM_SAMPLES; ++i ) {
    for( j = 0; j < K; ++j ) {
      if( j == 0 ) flip[j] = i;
      else flip[j] = ( i >> j ) & 1;
    }
    ConstTempType offsets(flip);
    TempType intLatticePoint = cond( offsets, ip1, ip0 );
    if( useTexture ) {
      // TODO calculate texture coordinates
      grad[i] = noiseTex( _pcast<3>(intLatticePoint)); // lookup 3D texture
    } else {
      grad[i] = _pcast<M>(hashmrg( intLatticePoint )); 
    }
  }
  TempType t = _psmootht( rp ); //ShPerlin's improved polynomial interpolant 
  for( i = K - 1; i >= 0; --i ) {
    int offset = 1 << i; 
    for( j = 0; j < offset; ++j ) {
      grad[j] = lerp( t(i), grad[j+offset], grad[j] ); 
    }
  }

  //result is in grad[0], cast into the result size 
  return grad[0];
}

template<int M, int N, int K, typename T>
ShVariableN<M, T> turbulence(const ShVariableN<N, T> &amp,
      const ShVariableN<K, T> &p, bool useTexture) {
  std::cout << "sturbulence M: " << M << " N: " << N << " K: " << K << std::endl;

  ShAttrib<M, SH_VAR_TEMP, T> result;
  T freq = 1.0;
  for( int i = 0; i < N; ++i, freq *= 2.0 ) {
    result = mad( ShPerlin<M>::noise(p * freq, useTexture), amp(i), result );
  }
  return result;
}

template<int M, int N, int K, typename T>
ShVariableN<M, T> sturbulence(const ShVariableN<N, T> &amp,
      const ShVariableN<K, T> &p, bool useTexture) {
  std::cout << "sturbulence M: " << M << " N: " << N << " K: " << K << std::endl;
  ShAttrib<M, SH_VAR_TEMP, T> result;
  result = turbulence<M>(amp, p, useTexture);
  return _pmad( result, 2.0, - 1.0 ); 
}

} // namespace ShUtil

#endif
