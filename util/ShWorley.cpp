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
#include "ShWorley.hpp"
#include "ShUtilLib.hpp"


namespace ShUtil {

using namespace SH;

static const int DX[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static const int DY[] = { 0, 1, 1, 1, 0, -1, -1, -1 }; 


ShWorley::ShWorley( int frequency, bool useTexture ): useTexture(useTexture), freq( frequency ),
  dfreq( freq ), cellPosTex(frequency, frequency)
{
  int i, j, k;
  srand(13);

  // generate cell positions 
  float** posx; 
  float** posy;
  posx = new float*[freq];
  posy = new float*[freq];
  for(i = 0; i < freq; ++i) {
    posx[i] = new float[freq];
    posy[i] = new float[freq];
  }
  for(i = 0; i < freq; ++i) for(j = 0; j < freq; ++j) for(k = 0; k < 2; ++k) {
    posx[i][j] = rand() / (float) RAND_MAX;
    posy[i][j] = rand() / (float) RAND_MAX;
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
      float px = posx[ ( i + DX[k] + freq ) % freq ][ ( j + DY[k] + freq ) % freq ]; 
      float py = posy[ ( i + DX[k] + freq ) % freq ][ ( j + DY[k] + freq ) % freq ]; 
      adjImage[k](i, j, 0) = (px + DX[k] + 1) / 3; 
      adjImage[k](i, j, 1) = (py + DY[k] + 1) / 3; 

      px = posx[ ( i + DX[k + 4] + freq ) % freq ][ ( j + DY[k + 4] + freq ) % freq ]; 
      py = posy[ ( i + DX[k + 4] + freq ) % freq ][ ( j + DY[k + 4] + freq ) % freq ]; 
      adjImage[k](i, j, 2) = (px + DX[k + 4] + 1) / 3; 
      adjImage[k](i, j, 3) = (py + DY[k + 4] + 1) / 3; 
    }
  }
  for(i = 0; i < 4; ++i) adjTex[i]->load(adjImage[i]);
  cellPosTex.load(cellPosImage); 

  for(i = 0; i < freq; ++i) {
    delete[] posx[i];
    delete[] posy[i];
  }
  delete[] posx;
  delete[] posy;
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

ShAttrib3f ShWorley::worley(ShAttrib2f p, ShAttrib4f c, ShWorleyMetric m ) {
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
  ShAttrib4f gradAdj[4];
  ShAttrib2f gradCell;
  dcell = dist( cellPoint, fp );
  gradCell = normalize(cellPoint - fp);
  for(i = 0; i < 4; ++i) {
    dadj[0](i) = dist( adjPoints[i](0,1), fp );
    dadj[1](i) = dist( adjPoints[i](2,3), fp );
    gradAdj[i](0,1) = normalize(adjPoints[i](0,1) - fp);
    gradAdj[i](2,3) = normalize(adjPoints[i](2,3) - fp);
  }

  // TODO find faster method to do k-selection for k = { 1, 2, 3, 4 }
  // Currently does an even-odd transposition sort
  ShAttrib4f even, odd;
  ShAttrib1f last;
  even = dadj[0];
  odd = dadj[1];
  last = dcell;
  ShAttrib4f et, ot, t;
  for(i = 0; i < 5; ++i ) {
    et = min( even, odd );
    ot = max( even, odd );

    even(1,2,3) = max( et(1,2,3), ot(0,1,2) );
    even(0) = et(0);
    odd(0,1,2) = min( et(1,2,3), ot(0,1,2) );
    odd(3) = min( ot(3), last );
    last = max( ot(3), last );
  }
  ShAttrib4f resultVec;
  resultVec(0,2) = even(0,1);
  resultVec(1,3) = odd(0,1);

  //TODO integrate this gradient selection into the above sort.
  //This is a really really UGLY hack
  ShAttrib2f grads[4];
  for (i=0; i<4; i++)
  {
    grads[i] = (resultVec(i)==dcell)*gradCell;
    //grads[i] = cond((resultVec(i)==dcell), gradCell, ShAttrib2f(0,0));
    for (j=0; j<4; j++)
    {
      grads[i]+= (resultVec(i)==dadj[0](j))*gradAdj[j](0,1);
      grads[i]+= (resultVec(i)==dadj[1](j))*gradAdj[j](2,3);
      //grads[i] = cond((resultVec(i)==dadj[0](j)), gradAdj[j](0,1), grads[i]);
      //grads[i] = cond((resultVec(i)==dadj[1](j)), gradAdj[j](2,3), grads[i]);
    }
  }

  result(0,1) = c(0)*grads[0] + 
	  	c(1)*grads[1] +
		c(2)*grads[2] +
		c(3)*grads[3];
  result(2) = dot(resultVec, c);
  return result;
}

void ShWorley::useNoiseTexture(bool useNoiseTex) {
  useTexture = useNoiseTex;
}

}
