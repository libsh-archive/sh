// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Bryan Chan 
//          
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

#ifndef SHLA_MATRIXIMPL_HPP
#define SHLA_MATRIXIMPL_HPP

#include <string>
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShException.hpp"
#include "ShlaMatrix.hpp"
#include "ShlaRenderGlobal.hpp"
//#include "Timer.hpp"

namespace Shla {

using namespace SH;

/*
ShlaMatrix<T, M, N>::ShlaMatrix() { 
}

ShlaMatrix<T, M, N>::~ShlaMatrix() {
}
*/

template< typename T, int M, int N >
ShlaDenseMatrix<T, M, N>::ShlaDenseMatrix() {
  if( m_column == 0 ) {
    ShError( ShException( "Could not allocate dense matrix." ) ); 
  }
}

template< typename T, int M, int N >
ShlaDenseMatrix<T, M, N>::~ShlaDenseMatrix() {
}

template< typename T, int M, int N >
template< typename VecT, ShlaVectorKind K >
ShlaVector<VecT, M, N> ShlaDenseMatrix<T, M, N>::operator|( const ShlaVector<VecT, M, N, K> &v ) const {

  // TODO this doesn't work at all
#if 0
  /* TODO these should be static */
  typedef ShlaRenderGlobal<VecT, M, N> vecGlobal;
  static ShTexture2D<T> &columnTex = ShlaRenderGlobal<T, M, N>::op1; 
  static ShTexture2D<VecT> &vecTex = vecGlobal::op2; 
  static ShTexture2D<VecT> &vecAccum = vecGlobal::accum; 
  static ShTexCoord2f colTexCoord;
  static ShProgram fsh; 
  if( !fsh ) {
    ShEnvironment::boundShader[0] = 0;
    ShEnvironment::boundShader[1] = 0;

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;
      typename vecGlobal::OutputColorType out;
      out = mad( columnTex(tc), vecTex(colTexCoord), vecAccum(tc) );
    } SH_END_PROGRAM;
  }
  /* end of static */

  if( M * N == 1 ) { // handle 1 dimensional matrix
    ShError( ShException( "Cannot handle dimension 1 matrices" ) ); 
  }
#endif

  ShlaVector<VecT, M, N> result;
#if 0

  vecGlobal::bindDefault( fsh );
  vecTex.attach( v.getMem() );
  vecGlobal::accumInit( result.getMem() );
  for( int i = 0; i < N; ++i ) for( int j = 0; j < M; ++j ) {
    int colIndex = i * M + j;
    if( colIndex == M * N - 1 ) {
      printf( "last loop\n" );
      vecGlobal::accumLastLoop( result.getMem() );
    } else if( i > 0 ) {
      printf( "loop\n" );
      vecGlobal::accumLoop();
    }
    // calculate the i'th column
    columnTex.attach( m_column[colIndex].getMem() );
    colTexCoord = ShConstant2f( ( j + 0.5 ) / M, ( i + 0.5 ) / N );
    printf( "Calculating Column %d texcoord( %lf, %lf )\n", colIndex,
        colTexCoord.node()->getValue(0), colTexCoord.node()->getValue(1) );

    vecGlobal::useRenderbuf();
    vecGlobal::drawQuad();
  }
  printf( "after loop\n" );
  vecGlobal::detachAll();
  columnTex.attach( 0 ); 
  //vecGlobal::detachAll();
  printf( "detach all\n" );

  shDrawBuffer( oldfb );
  printf( "draw oldfb\n" );
#endif
  return result;
}

template< typename T, int M, int N >
template< typename VecT, ShlaVectorKind K >
ShlaVector<VecT, M, N> ShlaBandedMatrix<T, M, N>::operator|( const ShlaVector<VecT, M, N, K> &v ) const {
  //ShTimer start = ShTimer::now();
  SH_DEBUG_PRINT( "Banded Matrix-Vector Multiplication" );
  /* only use static variables in the shader */
  typedef ShlaRenderGlobal<VecT, M, N> vecGlobal;
  static ShTexture2D<T> &diagTex = ShlaRenderGlobal<T, M, N>::op1; 
  static ShTexture2D<VecT> &vecTex = vecGlobal::op2; 
  static ShTexture2D<VecT> &vecAccum = vecGlobal::accum; 
  static ShUberbufferPtr zero = vecGlobal::getZeroMem();
  static ShAttrib2f diag;  // for current diagonal d, stores ( d % M, d / M );
  //static ShProgram fsh; 
  /*
  ShTexture2D<T> diagTex( M, N ); 
  ShTexture2D<VecT> &vecTex = vecGlobal::op2;
  ShTexture2D<VecT> &vecAccum = vecGlobal::accum; 
  ShAttrib2f diag;  // for current diagonal d, stores ( d % M, d / M );
  */
  // TODO figure out why this doesn't work when fsh is static
  // (i.e. on the second call to this operator, v gets corrupted)
  ShProgram fsh; 

  //if( !fsh ) {
    SH_DEBUG_PRINT( "Generating fragment shader for multiplying diagonals." );
    ShEnvironment::boundShader[0] = 0;
    ShEnvironment::boundShader[1] = 0;

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;

      ShTexCoord2f offsetTc = tc + diag;
      offsetTc(1) += ( offsetTc(0) > 1.0 ) * ShConstant1f( 1.0 / N );
      offsetTc = frac( offsetTc );

      typename vecGlobal::OutputColorType out;
      out = mad( diagTex(tc), vecTex(offsetTc), vecAccum(tc) ); 
    } SH_END_PROGRAM;
  //}
  /* end of static only */
  //std::cout << "Compile shader" << ( ShTimer::now() - start ).value() / 1000.0 << "s" << std::endl;
  //start = ShTimer::now();

  ShlaVector<VecT, M, N> result;

  vecGlobal::bindDefault( fsh );
  vecTex.attach( v.getMem() );

  vecGlobal::accumInit( zero ); 
  int i = 0;
  int diagSize = m_diagonal.size();
  for( typename DiagonalMap::const_iterator diagIt = m_diagonal.begin();
       diagIt != m_diagonal.end() ; ++diagIt, ++i ) {

    if( i == diagSize - 1 ) { 
      if( i == 0 ) {
        vecGlobal::renderbuf->bind( result.getMem() );
      } else {
        vecGlobal::accumLastLoop( result.getMem() );
      }
    } else if( i > 0 ) { 
      vecGlobal::accumLoop();
    }

    // calculate the i'th column
    SH_DEBUG_PRINT( "Attaching diagTex for diagonal " << diagIt->first );

    diagTex.attach( diagIt->second.getMem() );
    diag = ShConstant2f( (double)( diagIt->first % M ) / M, (double)( diagIt->first / M ) / N );
    SH_DEBUG_PRINT( "diag: (" << diag.node()->getValue(0) << ", " << diag.node()->getValue(1) << ")" );

    vecGlobal::useRenderbuf();
    vecGlobal::drawQuad();
    shDrawBuffer( 0 );
    diagTex.attach( 0 ); 
  }
  vecGlobal::detachAll();

  glFinish();
  //std::cout << "Done banded matrix multiply " << ( ShTimer::now() - start ).value() / 1000.0 << "s" << std::endl;
  
  return result;
}

template< typename T, int M, int N >
ShlaBandedMatrix<T, M, N> ShlaBandedMatrix<T, M, N>::operator*( const ShlaBandedMatrix<T, M, N> &m ) const {

  SH_DEBUG_PRINT( "Banded Matrix-Matrix Multiplication" );
  /* only use static variables in the shader */
  typedef ShlaRenderGlobal<T, M, N> global;
  static ShTexture2D<T> &diag1= global::op1; 
  static ShTexture2D<T> &diag2= global::op2; 
  static ShTexture2D<T> &accum = global::accum; 
  static ShAttrib2f diag;  // for current diagonal d, stores ( d % M, d / M );
  // TODO figure out why this doesn't work when fsh is static
  // (i.e. on the second call to this operator, v gets corrupted)
  /* static */ ShProgram fsh; 

  //if( !fsh ) {
    SH_DEBUG_PRINT( "Generating fragment shader for banded matrix multiply." );
    ShEnvironment::boundShader[0] = 0;
    ShEnvironment::boundShader[1] = 0;

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;

      ShTexCoord2f offsetTc = tc + diag;
      offsetTc(1) += ( offsetTc(0) > 1.0 ) * ShConstant1f( 1.0 / N );
      offsetTc = frac( offsetTc );

      typename global::OutputColorType out;
      out = mad( diag1(tc), diag2(offsetTc), accum(tc) ); 
    } SH_END_PROGRAM;
  //}
  /* end of static only */

  ShlaBandedMatrix<T, M, N> result;

  // TODO get rid of this hack - 
  SH_DEBUG_PRINT( "Binding Shader" );
  global::bindDefault( fsh );

  int diagIndex1, diagIndex2, newDiagIndex;
  ShlaVector<T, M, N> temp; 
  for( typename DiagonalMap::const_iterator diagit = m_diagonal.begin(); 
      diagit != m_diagonal.end(); ++diagit ) {
    diag1.attach( diagit->second.getMem() ); 
    diagIndex1 = diagit->first; 
    diag = ShConstant2f( (double)( diagIndex1 % M ) / M, (double)( diagIndex1 / M ) / N );

    for( typename DiagonalMap::const_iterator mdiagit = m.m_diagonal.begin(); 
        mdiagit != m.m_diagonal.end(); ++mdiagit ) {
      diagIndex2 = mdiagit->first;
      newDiagIndex = (diagIndex1 + diagIndex2) % ( M * N );
      SH_DEBUG_PRINT( "diag " << diagIndex1 << " * diag " << diagIndex2 << " -> diag " << newDiagIndex );
      diag2.attach( mdiagit->second.getMem() ); 
      if( result.hasBand( newDiagIndex ) ) {
        temp = result[ newDiagIndex ];
        accum.attach( temp.getMem() );
      } else {
        accum.attach( global::getZeroMem() );
      }
      global::renderbuf->bind( result[ newDiagIndex ].getMem() );

      global::useRenderbuf();
      global::drawQuad();

      global::renderbuf->bind( 0 );
      shDrawBuffer( 0 );
      accum.attach( 0 );
      diag2.attach( 0 );
    }
    diag1.attach( 0 );
  }
  global::detachAll();
  return result;
}

template< typename T, int M, int N >
const float& ShlaDenseMatrix<T, M, N>::operator()( int r, int c, int element ) const {
  assert( c < M * N );
  return m_column[c]( r, element );
}

template< typename T, int M, int N >
float& ShlaDenseMatrix<T, M, N>::operator()( int r, int c, int element ) {
  assert( c < M * N );
  return m_column[c]( r, element );
}

template< typename T, int M, int N >
const ShlaVector<T, M, N>& ShlaDenseMatrix<T, M, N>::operator[]( int c ) const {
  assert( c < M * N );
  return m_column[c];
}

template< typename T, int M, int N >
ShlaVector<T, M, N>& ShlaDenseMatrix<T, M, N>::operator[]( int c ) {
  assert( c < M * N );
  return m_column[c];
}

template< typename T, int M, int N >
ShlaBandedMatrix<T, M, N>::ShlaBandedMatrix() {
}

template< typename T, int M, int N >
ShlaBandedMatrix<T, M, N>::~ShlaBandedMatrix() {
}

// Note, because of the STL map's default behaviour,
// accessing an element in a previously untouched diagonal 
// automatically creates a zeroed ShlaVector using the default
// ShlaVector constructor.

template< typename T, int M, int N >
const float& ShlaBandedMatrix<T, M, N>::operator()( int r, int c, int element ) const {
  int d = ( c + M * N - r ) % ( M * N ); 
  return m_diagonal[d]( r, element );
}

template< typename T, int M, int N >
float& ShlaBandedMatrix<T, M, N>::operator()( int r, int c, int element ) {
  int d = ( c + (M * N) - r ) % (M * N); 
  return m_diagonal[d]( r, element );
}

template< typename T, int M, int N >
const ShlaVector<T, M, N>& ShlaBandedMatrix<T, M, N>::operator[]( int d ) const {
  return m_diagonal[d];
}

template< typename T, int M, int N >
ShlaVector<T, M, N>& ShlaBandedMatrix<T, M, N>::operator[]( int d ) {
  return m_diagonal[d];
}

template< typename T, int M, int N >
bool ShlaBandedMatrix<T, M, N>::hasBand( int i ) const {
  return m_diagonal.count( i ) > 0;
}

}

#endif
