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
  if( M * N == 1 ) { // handle 1 dimensional matrix
    ShError( ShException( "Cannot handle dimension 1 matrices" ) ); 
  }

  /* TODO these should be static */
  ShProgram fsh; 
  ShTexture2D<T> &columnTex = ShlaVector<T, M, N>::op1;
  ShTexture2D<VecT> &vecTex = v.op2;
  ShTexture2D<VecT> &vecAccum = v.accum;
  ShTexCoord2f colTexCoord;
  /* end of static */

  ShlaVector<VecT, M, N> result;

  if( !fsh ) {
    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;
      typename ShlaVector<VecT, M, N>::OutputColorType out;
      out = mad( columnTex(tc), vecTex(colTexCoord), vecAccum(tc) );
    } SH_END_PROGRAM;
  }
  v.bindDefault( fsh );
  ShFramebufferPtr oldfb = ShEnvironment::framebuffer;

  vecTex.attach( v.getMem() );
  v.accumInit( result.getMem() );
  for( int i = 0; i < M * N; ++i ) {
    if( i == M * N - 1 ) {
      v.accumLastLoop( result.getMem() );
    } else if( i > 0 ) {
      v.accumLoop();
    }
    // calculate the i'th column
    printf( "Calculating Column %d\n", i );
    columnTex.attach( m_column[i].getMem() );
    colTexCoord = ShConstant2f( ( i % M + 0.5 ) / M, ( i / M + 0.5 ) / N );

    v.useRenderbuf();
    v.drawQuad();
  }
  columnTex.attach( 0 ); 
  v.detachAll();

  shDrawBuffer( oldfb );
  return result;
}

template< typename T, int M, int N >
template< typename VecT >
ShlaVector<VecT, M, N> ShlaBandedMatrix<T, M, N>::operator|( ShlaVector<VecT, M, N> &v ) {
  static ShProgram fsh; 
  static ShAttrib1f diag; 

  if( !fsh ) {
    fsh = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputPosition4f p;
      ShInputTexCoord2f tc;

      ShTexCoord2f offsetTc;
      offsetTc(0) = tc(0) + diag;
      offsetTc(1) = mad( floor( offsetTc(0) ), ( 1.0 / N ), tc(1) ); 
      offsetTc = frac( offsetTc );

      typename ShlaVector<VecT, M, N>::OutputColorType out;
      out = v.accum(tc) + v.op1(tc) * v.op2(offsetTc);
    }
  }

  if( N == 1 ) { // handle 1 dimensional matrix
    return m_column[0] * v;
  }

  ShlaVector<VecT, M, N> result;

  v.bindVsh();
  shBindShader( fsh );

  v.op2.attach( v.getMem() );

  v.accumInit( result.getMem() );
  for( typename DiagonalMap::iterator diagIt = m_diagonal.begin();
       diagIt != m_diagonal.end(); ++diagIt ) {
    // calculate the i'th column
    v.op1.attach( diagIt->second.getMem() );
    diag = diagIt->first / (double) M;

    v.clear();
    v.drawQuad();

    if( i == M * N - 1 ) {
      v.accumLastLoop( result.getMem() );
    } else {
      v.accumLoop();
    }
  }
  v.accumDetach();
  v.vecOp.attach( 0 ); 
  
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
