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
#ifndef SHGA_VERSORIMPL_HPP 
#define SHGA_VERSORIMPL_HPP 

#include "ShDebug.hpp"
#include "ShgaVersor.hpp"

namespace Shga {

  using namespace SH;

/** \file ShgaVersorImpl.hpp
 * TODO add see also ShgaVersor.hpp link (comments are there)
 */

static double _PseudoScalarCoeff[] = { 1 };
template<typename T, int P, int N>
static ShgaVersor<T, P, N>::PseudoScalar( ShgaVersor<T, P, N>::MAX_BASIS - 1, _PseudoScalarCoeff ); 
static ShgaVersor<T, P, N>::PseudoScalarInverse = PseudoScalar.inverse();


/* TODO do a more general implementation
 * Currently this is a specialized implementation for ShgaVersor<T, P, N>
 */

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades ) {
  : m_grades( grades ), size( 0 ) {
  updateSize(); 
}

/* TODO check if C++ spec requires template<> for specilization */
template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, double coeff[] )
  : ShgaVersor( grades ) { 
  for( i = 0; i < numElements[m_grades]; ++i ) {
    int vec = i / VECTOR_SIZE;
    int vecPos = i % VECTOR_SIZE;
    m_coeff[vec]( vecPos ) = coeff[i];
  }
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, ShAttrib1f coeff[] )
  : ShgaVersor( grades ) { 
  for( int i = 0; i < numElements[m_grades]; ++i ) {
    int vec = i / VECTOR_SIZE;
    int vecPos = i % VECTOR_SIZE;
    m_coeff[vec]( vecPos ) = coeff[i];
  }
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, T coeff[] )
  : ShgaVersor( grades ) { 
  updateSize(); 
  for( int i = 0; i < size; ++i ) m_coeff[i] = coeff[i]; 
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::~ShgaVersor() {
  delete[] m_coeff;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator=( ShgaVersor<T, P, N> &b )
  m_grades = b.m_grades;
  updateSize();
  for( int i = 0; i < size; ++i ) m_coeff[i] = b.m_coeff[i];
  return *this;
}

template<typename T, int P, int N>
ShAttrib1f& ShgaVersor<T, P, N>::operator()( int basisElement ) { 
  PositionPair &pos = elementPosition[ m_grades ][ basisElement ];
  return m_coeff[ pos.first ][ pos.second ]; 
}

template<typename T, int P, int N>
const ShAttrib1f ShgaVersor<T, P, N>::operator()( int basisElement ) const {
  PositionPair &pos = elementPosition[ m_grades ][ basisElement ];
  return m_coeff[ pos.first ][ pos.second ]; 
}

template<typename T, int P, int N>
ShgaVersors<T, P, N> ShgaVersor<T, P, N>::grade( int grades ) { 
  ShgaVersors<T, P, N> result( grades );
  runCopySequence( extractGrade[grades][m_grades], result );
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator*( const ShgaVersor<T, P, N> &b ) const {
  BinaryOpSequence &ops = gpOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator*=( const ShgaVersor<T, P, N> &b ) const {
  (*this) = operator*(b);
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator<<( const ShgaVersor<T, P, N> &b ) const {
  BinaryOpSequence &ops = lcpOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator<<=( const ShgaVersor<T, P, N> &b ) const {
  (*this) = operator<<( b );
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator^( const ShgaVersor<T, P, N> &b ) const {
  BinaryOpSequence &ops = opOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
  return result; 
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator^=( const ShgaVersor<T, P, N> &b ) const {
  (*this) = operator^(b);
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::gradeInvolution() const {
  UnaryOpSequence &ops = gradeInvolution[ m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runCopySequence( ops, result );
  return result;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::reverse() const {
  UnaryOpSequence &ops = gradeInvolution[ m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runCopySequence( ops, result );
  return result;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::cliffordConjugate() const {
  return reverse()->gradeInvolution();
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::dual() const {
  return operator*( PseudoScalarInverse ); 
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::inverse() const {
  // TODO general case
  assert( DIMENSION == 3 );
  // Lounesto's inverse
  ShgaVersor<T, P, N> conj = cliffordConjugate();
  ShgaVersor<T, P, N> thisconj = (*this) * conj; 
  ShAttrib1f scalar = thisconj( 0 ); 
  ShAttrib1f pseudoscalar = thisconj( MAX_BASIS - 1 );

  return conj * ( ( scalar - pseudoscalar ) 
    ( scalar * scalar - pseudoscalar * pseudoscalar ) ); 
}


template<typename T, int P, int N>
void ShgaVersor<T, P, N>::runMadSequence( BinaryOpSequence &bos,
    const ShgaVersor<T, P, N> &b, ShgaVersor<T, P, N> &dest ) const {
  for( typename BinaryOpSequence::OpVec::iterator ovit = bos.begin(); 
      ovit != bos.end(); ++ovit ) {
    VectorBinaryOp &op = *ovit;

    dest.m_coeff[ op.destVec ]( op.destSwiz ) =  
      mad( op.neg1 ? -m_coeff[ op.vec1 ]( op.swiz1 ) : m_coeff[ op.vec1 ]( op.swiz1 ), 
           op.neg2 ? b.m_coeff[ op.vec2 ]( op.swiz2 ) : b.m_coeff[ op.vec2 ]( op.swiz2 ),
           dest.m_coeff[ op.destVec ]( op.destSwiz ) );
  }
}

template<typename T, int P, int N>
void ShgaVersor<T, P, N>::runCopySequence( UnaryOpSequence &uos, ShgaVersor<T, P, N> &dest ) const {
  for( typename UnaryOpSequence::OpVec::iterator ovit = uos.begin(); 
      ovit != uos.end(); ++ovit ) {
    VectorUnaryOp &op = *ovit;
    dest.m_coeff[ op.destVec ]( op.destSwiz ) =  
      op.neg ? -m_coeff[ op.srcVec ]( op.srcSwiz ) : m_coeff[ op.srcVec ]( op.srcSwiz ); 
  }
}


template<typename T, int P, int N>
void ShgaVersor<T, P, N>::updateSize() {
  int requiredVecs = numVecs[ m_grades ]; 
  if( size == requiredVecs ) {
    // TODO zero coefficients?
    return; 
  }
  delete[] m_coeff;
  m_coeff = new T[ requiredVecs ];
  size = requiredVecs;
}

}


#endif
