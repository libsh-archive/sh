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
#include <cstdlib>
#include "ShSyntax.hpp"
#include "ShWorley.hpp"
#include "ShUtilLib.hpp"


namespace ShUtil {

using namespace SH;

static const int DX[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static const int DY[] = { 0, 1, 1, 1, 0, -1, -1, -1 }; 


ShWorley::ShWorley( int frequency, bool useTexture ): useTexture(useTexture), freq( frequency ),
  dfreq( freq ), cellPosTex(frequency, frequency)
{
  int i, j, k, l;
  srand48(13);

  // generate cell positions 
  double* posx[freq];
  double* posy[freq];
  for(i = 0; i < freq; ++i) {
    posx[i] = new double[freq];
    posy[i] = new double[freq];
  }
  for(i = 0; i < freq; ++i) for(j = 0; j < freq; ++j) for(k = 0; k < 2; ++k) {
    posx[i][j] = drand48();
    posy[i][j] = drand48();
  }

  ShImage adjImage[4];
  ShImage cellPosImage(freq, freq, 4);
  for(i = 0; i < 4; ++i) {
    adjImage[i] = ShImage(freq, freq, 4);
    adjTex[i] = new ShTexture2D< ShColor4f >(freq, freq);
  }

  for(i = 0; i < freq; ++i) for(j = 0; j < freq; ++j){ 
    cellPosImage(i, j, 0) = posx[i][j];
    cellPosImage(i, j, 1) = posy[i][j];
    for(k = 0; k < 4; ++k) {
      //TODO until we have floating point textures, we need to map the adjacent points
      // which are in [-1,2]^2 to [0,1]^2
      double px = posx[ ( i + DX[k] + freq ) % freq ][ ( j + DY[k] + freq ) % freq ]; 
      double py = posy[ ( i + DX[k] + freq ) % freq ][ ( j + DY[k] + freq ) % freq ]; 
      adjImage[k](i, j, 0) = (px + DX[k] + 1.0) / 3.0; 
      adjImage[k](i, j, 1) = (py + DY[k] + 1.0) / 3.0; 

      px = posx[ ( i + DX[k + 4] + freq ) % freq ][ ( j + DY[k + 4] + freq ) % freq ]; 
      py = posy[ ( i + DX[k + 4] + freq ) % freq ][ ( j + DY[k + 4] + freq ) % freq ]; 
      adjImage[k](i, j, 2) = (px + DX[k + 4] + 1.0) / 3.0; 
      adjImage[k](i, j, 3) = (py + DY[k + 4] + 1.0) / 3.0; 
    }
  }
  for(i = 0; i < 4; ++i) adjTex[i]->load(adjImage[i]);
  cellPosTex.load(cellPosImage); 

  for(i = 0; i < freq; ++i) {
    delete[] posx[i];
    delete[] posy[i];
  }
}

ShWorley::~ShWorley()
{
  int i;
  for(i = 0; i < 4; ++i) delete adjTex[i];
}

class Metric {
  private:
    ShWorleyMetric m;
  public:
    Metric( ShWorleyMetric m ): m( m ) {}
    ShAttrib1f operator()( ShAttrib2f a, ShAttrib2f b ) {
      switch( m ) {
        case L1: return lOneDistance( a, b );
        case L2: return distance( a, b );
        case L2_SQ: 
        {
          ShAttrib2f temp = a - b;
          return dot( temp, temp );
        }
        case LINF: return lInfDistance( a, b );
      }
    }

};

// hardcoded sort for 9 structs (each struct has 3 scalar elements)
// distance (in array[0]), x gradient (array[1]), y gradient (array[2])
//
// Data is sorted by the distance (comparisons are only done on element[0] each ShAttrib array) 
//
// Result is sorted so that
// even[0](0) <= odd[0](0) <= even[0](1) <= odd[0](1) <= etc. <= last[0]
// (so the smallest 4 elements are in even[](0), even[](1) and odd[](0), odd[](1)
//
void EvenOddSort( ShAttrib4f even[], ShAttrib4f odd[], ShAttrib3f &last ) {
  ShAttrib4f c; // condition

  // temporaries for holding even values when swapping elements between even/odd 
  static const int numChannels = 3; // one for position + two for gradients
  ShAttrib4f et[numChannels]; 
  int i, j;
  for(i = 0; i < 5; ++i ) { // must run ceil( 9 / 2 ) iterations to ensure sorted
    // hand optimizations that maybe the compiler should be doing later:
    //  -packing scalar ops into vector ops
    //  -not using temporary to save odd vals by always assigning to odd after reading from odd
    //  -reusing c(1) as a scalar temporary for saving last(j)
    // Most of this will not be possible until optimization can be done on individual vector components.
    c = even[0] < odd[0];
    for(j = 0; j < numChannels; ++j) { 
      et[j] = cond(c, even[j], odd[j] );
      odd[j] = cond(c, odd[j], even[j] );
    }

    c(0,1,2) = et[0](1,2,3) > odd[0](0,1,2); 
    for(j = 0; j < numChannels; ++j ) {
      even[j](1,2,3) = cond(c(0,1,2), et[j](1,2,3), odd[j](0,1,2));
      even[j](0) = et[j](0);
      odd[j](0,1,2) = cond(c(0,1,2), odd[j](0,1,2), et[j](1,2,3));
    }

    c(0) = odd[0](3) < last(0);
    for( j = 0; j < numChannels; ++j ) {
      c(1) = last(j);
      last(j) = cond(c(0),last(j), odd[j](3));
      odd[j](3) = cond(c(0), odd[j](3), c(1));
    }
  }
}


void ShWorley::doWorley(ShAttrib2f p, ShAttrib4f c, ShWorleyMetric m, ShAttrib1f &scalarResult, ShAttrib2f &gradientResult ) {
  ShAttrib3f result;
  Metric dist(m);

  int i, j;
  //TODO: handle p(i) values outside of [0,1]

  // get integer lattice point & offset from integer lattice point
  ShPoint2f pp = p * dfreq;
  ShPoint2f fp = frac(pp);


  // get points from adjacent cells (one per cell)
  ShAttrib2f cellPoint; 
  ShAttrib4f adjPoints[4];
  if(useTexture) {
    pp = ( floor(pp) + ShConstant2f( 0.5, 0.5 ) ) * ( 1.0 / dfreq ); 
    cellPoint = cellPosTex(pp)(0,1);
    // unmap adjacent points from [0,1]^2 back to [-1,2]^2
    for(i = 0; i < 4; ++i) adjPoints[i] = 
      mad(3.0, (*(adjTex[i]))(pp), ShConstant4f(-1, -1, -1, -1));
  } else {
    pp = floor( pp );
    cellPoint = hashmrg( pp );

    for( i = 0; i < 4; ++i ) { 
      ShAttrib2f adjp = pp + ShConstant2f( DX[i], DY[i] );
      adjPoints[i](0,1) = hashmrg( adjp ) + ShConstant2f( DX[i], DY[i] ); 

      adjp = pp + ShConstant2f( DX[i + 4], DY[i + 4] );
      adjPoints[i](2,3) = hashmrg( adjp ) + ShConstant2f( DX[i + 4], DY[i + 4] );
    }
  }

  // find distances & gradients to neighbours
  ShAttrib4f dadj[2];
  ShAttrib1f dcell;
  ShAttrib4f gradAdjX[2], gradAdjY[2];
  ShAttrib2f gradCell;
  dcell = dist( cellPoint, fp );
  gradCell = normalize(cellPoint - fp);
  for(i = 0; i < 4; ++i) {
    dadj[0](i) = dist( adjPoints[i](0,1), fp );
    dadj[1](i) = dist( adjPoints[i](2,3), fp );
    ShAttrib2f temp = normalize(adjPoints[i](0,1) - fp); 
    gradAdjX[0](i) = temp(0); 
    gradAdjY[0](i) = temp(1); 
    temp = normalize(adjPoints[i](2,3) - fp); 
    gradAdjX[1](i) = temp(0); 
    gradAdjY[1](i) = temp(1); 
  }

  // TODO find faster method to do k-selection for k = { 1, 2, 3, 4 }
  // Currently does an even-odd transposition sort
  // TODO - When arbitrary length vectors implemented, make this a library function
  ShAttrib4f even[3], odd[3];
  ShAttrib3f last;
  even[0] = dadj[0];
  even[1] = gradAdjX[0];
  even[2] = gradAdjY[0];
  odd[0] = dadj[1];
  odd[1] = gradAdjX[1];
  odd[2] = gradAdjY[1];
  last(0) = dcell;
  last(1) = gradCell(0);
  last(2) = gradCell(1);
  EvenOddSort( even, odd, last );

  ShAttrib4f resultVec;
  resultVec(0,2) = even[0](0,1);
  resultVec(1,3) = odd[0](0,1);
  scalarResult = dot(resultVec, c);

  ShAttrib4f gradsX, gradsY; 
  gradsX(0,2) = even[1](0,1);
  gradsY(0,2) = even[2](0,1);
  gradsX(1,3) = odd[1](0,1);
  gradsY(1,3) = odd[2](0,1);
  gradientResult(0) = dot(gradsX,c);
  gradientResult(1) = dot(gradsY,c);
}

ShAttrib3f ShWorley::worley(ShAttrib2f p, ShAttrib4f c, ShWorleyMetric m ) {
  ShAttrib3f result;
  ShAttrib1f scalarResult;
  ShAttrib2f gradientResult;
  doWorley( p, c, m, scalarResult, gradientResult );
  result(0,1) = gradientResult;
  result(2) = scalarResult;
  return result;
}

ShAttrib1f ShWorley::worleyNoGradient( ShAttrib2f p, ShAttrib4f c, ShWorleyMetric m ) {
  ShAttrib1f scalarResult;
  ShAttrib2f gradientResult;
  doWorley( p, c, m, scalarResult, gradientResult );
  return scalarResult; 
}


ShProgram ShWorley::worleyProgram( ShWorleyMetric m ) {
  ShProgram program = SH_BEGIN_PROGRAM() {
    ShInputAttrib4f SH_DECL(coefficients);
    ShInputAttrib2f SH_DECL(texcoord);

    ShAttrib1f tempScalar;
    ShAttrib2f tempGradient;
    doWorley( texcoord, coefficients, m, tempScalar, tempGradient );

    ShOutputAttrib1f SH_DECL(scalar) = tempScalar;
    ShOutputAttrib2f SH_DECL(gradient) = tempGradient;
  } SH_END_PROGRAM;
  return program;
}

void ShWorley::useNoiseTexture(bool useNoiseTex) {
  useTexture = useNoiseTex;
}

}
