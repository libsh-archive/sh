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
#include <cstdlib>
#include "ShPerlin.hpp"
#include "ShUtilLib.hpp"

namespace ShUtil {

using namespace SH;

#define PERM_SIZE 32
#define DPERM_SIZE ( ( double ) PERM_SIZE )
#define INV_PERM_SIZE ( 1.0 / ( double )PERM_SIZE )

ShPerlin::ShPerlin( double persistence, int octaves ):
  persistence( persistence ), octaves( octaves ), useTexture( false ), noiseTex( PERM_SIZE, PERM_SIZE, PERM_SIZE )
{
  int i, j, k;
  srand( 13 );

  // generate pseudorandom noise noiseImage[x + y * PERM_SIZE][z] holds the four
  // 1D gradient components for lattice points (x, y, z), (x, y, z + 1), (x, y + 1, z),
  // and (x, y + 1, z + 1 )
  ShImage noiseImage( PERM_SIZE, PERM_SIZE, 4 );
  float grads[ PERM_SIZE ][ PERM_SIZE ][ PERM_SIZE ];
  for( i = 0; i < PERM_SIZE; ++i ) for( j = 0; j < PERM_SIZE; ++j ) for( k = 0; k < PERM_SIZE; ++k ) {
    grads[i][j][k] = rand() / (float) RAND_MAX;
  }
  for( k = 0; k < PERM_SIZE; ++k ) for( i = 0; i < PERM_SIZE; ++i ) for( j = 0; j < PERM_SIZE; ++j ) { 
    int index1 = i + j * PERM_SIZE;
    noiseImage( i, j, 0 ) = grads[i][j][k];
    noiseImage( i, j, 1 ) = grads[i][j][(k + 1) % PERM_SIZE]; 
    noiseImage( i, j, 2 ) = grads[i][(j + 1) % PERM_SIZE][k]; 
    noiseImage( i, j, 3 ) = grads[i][(j + 1) % PERM_SIZE][(k + 1) % PERM_SIZE]; 
    noiseTex.load( noiseImage, k ); 
  }
}

ShPerlin::~ShPerlin()
{
}


ShAttrib3f smootht( ShAttrib3f t ) 
{
  return t * t * t * 
    mad( t, mad( t, 6.0, ShConstant1f( -15.0 )( 0,0,0 ) ), ShConstant1f( 10.0 )( 0,0,0 ) ); 
}

ShAttrib1f ShPerlin::noise( ShAttrib3f p ) 
{
  int i, j, k;


  ShAttrib3f rp = frac( p ); // offset from integer lattice point 
  ShAttrib3f p0 = frac( p * INV_PERM_SIZE ) * DPERM_SIZE; // positive coordinates in [0, PERM_SIZE)

  // get integer lattice point, and scale [0, PERM_SIZE]^3 space to [0,1]^3 for permutation by the texture
  ShAttrib3f ip[2];
  ip[0] = ( floor( p0 ) + ShConstant3f( 0.5, 0.5, 0.5 ) ) * INV_PERM_SIZE; 
  ip[1] = frac( ip[0] + ShConstant3f( INV_PERM_SIZE, INV_PERM_SIZE, INV_PERM_SIZE ) ); 


  // x gradients at x = 0 and x = 1 ( xg[i] contains gradients for ( y,z ) = {( 0,0 ), ( 0,1 ), ( 1,0 ), ( 1,1 )} in order
  ShAttrib4f xg[2]; 

  if( !useTexture ) {
    ShAttrib3f dip = ip[1] - ip[0];

    // Compute Gradient adpating Perlin's method 
    // G = G[ ( ip.x + P[ ( ip.y + P[ip.z] ) mod PERM_SIZE ] ) mod PERM_SIZE ]
    for( i = 0; i < 2; ++i ) for( j = 0; j < 2; ++j ) for( k = 0; k <2; ++k ) {
      xg[i](j * 2 + k) = hashmrg( ip[0] + dip * ShConstant3f( i, j, k ) )(2); 
    }
  } else {
    ShAttrib3f tc = ip[0];
    xg[0] = noiseTex( tc ); 

    tc(0) = ip[1](0); 
    xg[1] = noiseTex( tc ); 
  }

  // interpolation in x, then y, then z direction
  ShAttrib3f t = smootht( rp ); //ShPerlin's improved polynomial interpolant 
  ShAttrib4f xgrad = lerp( t(0), xg[1], xg[0] ); 
  ShAttrib2f ygrad = lerp( t(1), xgrad(2,3), xgrad(0,1) );
  return lerp( t(2), ygrad(1), ygrad(0) ); 
}

void ShPerlin::useNoiseTexture( bool useNoiseTex ) {
  useTexture = useNoiseTex;
}

void ShPerlin::setTurbulenceParams( double persist, int numOctaves ) {
  persistence = persist;
  octaves = numOctaves;
}

ShAttrib1f ShPerlin::turbulence( ShAttrib3f p ) {
  ShAttrib1f result;
  double freq = 1.0;
  double amp = 1.0;
  double total = 0.0;

  for( int i = 0; i < octaves; ++i, amp *= persistence, freq *= 2.0 ) {
    result += noise( p * freq ) * amp;
    total += amp;
  }
  
  return result * ( 1.0 / total );
}

ShAttrib1f ShPerlin::sturbulence( ShAttrib3f p ) {
  return turbulence( p ) * 2.0 - 1.0;
}

} // namespace ShUtil

