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
#include "ShException.hpp"
#include "ShlaMatrix.hpp"
#include "ShlaRenderGlobal.hpp"
#include "ShlaLib.hpp"

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
template< typename VecT >
ShlaVector<VecT, M, N> ShlaDenseMatrix<T, M, N>::operator|( ShlaVector<VecT, M, N> &v ) {
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

  ShlaVector<VecT, M, N> result;
  ShFramebufferPtr oldfb = ShEnvironment::framebuffer;

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
  return result;
}

template< typename T, int M, int N >
template< typename VecT >
ShlaVector<VecT, M, N> ShlaBandedMatrix<T, M, N>::operator|( ShlaVector<VecT, M, N> &v ) {
  /* only use static variables in the shader */
  typedef ShlaRenderGlobal<VecT, M, N> vecGlobal;
  static ShTexture2D<T> &diagTex = ShlaRenderGlobal<T, M, N>::op1; 
  static ShTexture2D<VecT> &vecTex = vecGlobal::op2; 
  static ShTexture2D<VecT> &vecAccum = vecGlobal::accum; 
  static ShAttrib1f diag; 
  static ShProgram fsh; 

  if( !fsh ) {
    ShEnvironment::boundShader[0] = 0;
    ShEnvironment::boundShader[1] = 0;

    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;

      ShTexCoord2f offsetTc;
      offsetTc(0) = tc(0) + diag;
      offsetTc(1) = mad( floor( offsetTc(0) ), ( 1.0 / N ), tc(1) ); 
      offsetTc = frac( offsetTc );

      typename vecGlobal::OutputColorType out;
      out = vecAccum(tc) + diagTex(tc) * vecTex(offsetTc);
    } SH_END_PROGRAM;
  }
  /* end of static only */

  if( M * N == 1 ) { // handle 1 dimensional matrix
    ShError( ShException( "Cannot handle dimension 1 matrices" ) ); 
  }

  ShlaVector<VecT, M, N> result;

  ShFramebufferPtr oldfb = ShEnvironment::framebuffer;

  vecGlobal::bindDefault( fsh );
  vecTex.attach( v.getMem() );
  vecGlobal::accumInit( result.getMem() );
  for( typename DiagonalMap::iterator diagIt = m_diagonal.begin();
       diagIt != m_diagonal.end(); ++diagIt ) {

    typename DiagonalMap::iterator tempIt = diagIt;
    if( ++tempIt == m_diagonal.end() ) { 
      vecGlobal::accumLastLoop( result.getMem() );
    } else if( diagIt != m_diagonal.begin() ){
      vecGlobal::accumLoop();
    }

    // calculate the i'th column
    diagTex.attach( diagIt->second.getMem() );
    diag = diagIt->first / (double) M;

    vecGlobal::useRenderbuf();
    vecGlobal::drawQuad();

  }
  vecGlobal::detachAll();
  diagTex.attach( 0 ); 

  shDrawBuffer( oldfb );
  
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

}

#endif
