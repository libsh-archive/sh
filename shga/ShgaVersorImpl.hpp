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

#include <iostream>
#include <sstream>
#include "ShDebug.hpp"
#include "ShgaVersor.hpp"

namespace Shga {

  using namespace SH;

/** \file ShgaVersorImpl.hpp
 * TODO add see also ShgaVersor.hpp link (comments are there)
 */



/* TODO do a more general implementation
 * Currently this is a specialized implementation for ShgaVersor<T, P, N>
 */

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor() 
  : m_grades( 0 ), size( 0 ) {
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( double scalar ) 
  : m_grades( SHGA_GRADE0 ), size( 0 ) {
    updateSize();
  m_coeff[0](0) = scalar;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( ShAttrib1f scalar ) 
  : m_grades( SHGA_GRADE0 ), size( 0 ) {
    updateSize();
  m_coeff[0](0) = scalar;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades ) 
  : m_grades( grades ), size( 0 ) {
  updateSize(); 
}

/* TODO check if C++ spec requires template<> for specilization */
template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, double coeff[] )
  : m_grades( grades ), size( 0 ) {
  updateSize();
  for( int i = 0; i < numElements[m_grades]; ++i ) {
    int vec = i / VECTOR_SIZE;
    int vecPos = i % VECTOR_SIZE;
    m_coeff[vec]( vecPos ) = coeff[i];
  }
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, ShAttrib1f coeff[] )
  : m_grades( grades ), size( 0 ) {
  updateSize();
  for( int i = 0; i < numElements[m_grades]; ++i ) {
    int vec = i / VECTOR_SIZE;
    int vecPos = i % VECTOR_SIZE;
    m_coeff[vec]( vecPos ) = coeff[i];
  }
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( int grades, const T coeff[] )
  : m_grades( grades ), size( 0 ) {
  updateSize();
  for( int i = 0; i < size; ++i ) m_coeff[i] = coeff[i]; 
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::ShgaVersor( const ShgaVersor<T, P, N> &b )
  : m_grades( b.m_grades ), size( 0 ){
  updateSize();
  for( int i = 0; i < size; ++i ) m_coeff[i] = b.m_coeff[i];
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>::~ShgaVersor() {
  if( size > 0 ) delete[] m_coeff;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator=( const ShgaVersor<T, P, N> &b ) {
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
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::grade( int grades ) { 
  ShgaVersor<T, P, N> result( grades );
  runCopySequence( extractGradeOps[grades][m_grades], result );
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator*( const ShgaVersor<T, P, N> &b ) const {
  // TODO figure out if this is a bug, or if this is in the C++ spec
  // (I cannot refer to BinaryOpSequence without ShgaBase<T, P, N>)
  typename ShgaBase<T, P, N>::BinaryOpSequence &ops = gpOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
  return result;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator*=( const ShgaVersor<T, P, N> &b ) {
  (*this) = operator*(b);
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator<<( const ShgaVersor<T, P, N> &b ) const {
  typename ShgaBase<T, P, N>::BinaryOpSequence &ops = lcpOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
  return result;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator<<=( const ShgaVersor<T, P, N> &b ) {
  (*this) = operator<<( b );
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::operator^( const ShgaVersor<T, P, N> &b ) const {
  typename ShgaBase<T, P, N>::BinaryOpSequence &ops = opOps[ m_grades ][ b.m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runMadSequence( ops, b, result );
  return result; 
}

template<typename T, int P, int N>
ShgaVersor<T, P, N>& ShgaVersor<T, P, N>::operator^=( const ShgaVersor<T, P, N> &b ) {
  (*this) = operator^(b);
  return *this;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::gradeInvolution() const {
  typename ShgaBase<T, P, N>::UnaryOpSequence &ops = gradeInvolution[ m_grades ];
  ShgaVersor<T, P, N> result( ops.resultGrade );
  runCopySequence( ops, result );
  return result;
}

template<typename T, int P, int N>
ShgaVersor<T, P, N> ShgaVersor<T, P, N>::reverse() const {
  typename ShgaBase<T, P, N>::UnaryOpSequence &ops = gradeInvolution[ m_grades ];
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
  double coeff[] = { 1 };

  ShgaVersor<T, P, N>::PseudoScalar( SHGA_GRADE0 << DIMENSION, coeff ); 
  return operator*( PseudoScalar.inverse() ); 
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

  return conj * ( ( scalar - pseudoscalar ) *  
    ( scalar * scalar - pseudoscalar * pseudoscalar ).inverse() ); 
}

template<typename T, int P, int N>
std::string ShgaVersor<T, P, N>::toString() const {
  std::ostringstream out;
 
  bool firstTerm = true;
  for( int i = 0; i < numElements[ m_grades ]; ++i ) {
    int vec = i / VECTOR_SIZE;
    int offset = i % VECTOR_SIZE;

    double coeff = m_coeff[vec].node()->getValue( offset );
    if( coeff != 0 ) {
      if( !firstTerm ) out << " + ";
      out << coeff << " * " << basisName[ elements[ m_grades ][ i ] ]; 
      firstTerm = false;
    }
  }
  return out.str();
}


template<typename T, int P, int N>
void ShgaVersor<T, P, N>::runMadSequence( typename ShgaBase<T, P, N>::BinaryOpSequence &bos,
    const ShgaVersor<T, P, N> &b, ShgaVersor<T, P, N> &dest ) const {
  double destData[VECTOR_SIZE];
  double src1Data[VECTOR_SIZE];
  double src2Data[VECTOR_SIZE];
  std::cout << "running Mad Sequence" << std::endl;
  for( typename ShgaBase<T, P, N>::BinaryOpSequence::OpVec::iterator ovit = bos.ops.begin(); 
      ovit != bos.ops.end(); ++ovit ) {
    typename ShgaBase<T, P, N>::VectorBinaryOp &op = *ovit;

    // TODO figure out how to do this properly
    // (now the only way to do it is mess with Sh internals in a BAD way,
    // since we don't know the size of each swizzle)
    std::cout << op.toString() << std::endl;

    T& destVec = dest.m_coeff[ op.destVec ];
    T& src1Vec = m_coeff[ op.vec1 ]; 
    T& src2Vec = b.m_coeff[ op.vec2 ]; 
    bool neg = op.neg1 ^ op.neg2;

    destVec.getValues( destData );
    src1Vec.getValues( src1Data );
    src2Vec.getValues( src2Data );
    for( int i = 0; i < op.size; ++i ) {
      destData[ op.destSwiz[i] ] += ( neg ? -1 : 1 ) * src1Data[ op.swiz1[i] ] * src2Data[ op.swiz2[i] ]; 
    }
    destVec.setValues( destData );
    /*
    destVec( op.destSwiz ) =
      mad( ( op.neg1 ? -src1Vec : src1Vec ) (op.swiz1 ), 
           ( op.neg2 ? -src2Vec : src2Vec ) (op.swiz1 ), 
           destVec( op.destSwiz ) );
     */
           
  }
}

template<typename T, int P, int N>
void ShgaVersor<T, P, N>::runCopySequence( typename ShgaBase<T, P, N>::UnaryOpSequence &uos, 
    ShgaVersor<T, P, N> &dest ) const {
  double destData[VECTOR_SIZE];
  double srcData[VECTOR_SIZE];
  for( typename ShgaBase<T, P, N>::UnaryOpSequence::OpVec::iterator ovit = uos.ops.begin(); 
      ovit != uos.ops.end(); ++ovit ) {
    typename ShgaBase<T, P, N>::VectorUnaryOp &op = *ovit;
    /*
    dest.m_coeff[ op.destVec ]( op.destSwiz ) =  
      op.neg ? -m_coeff[ op.srcVec ]( op.srcSwiz ) : m_coeff[ op.srcVec ]( op.srcSwiz ); 
      */

    T& destVec = dest.m_coeff[ op.destVec ];
    T srcVec = ( op.neg1 ? -m_coeff[ op.vec1 ] : m_coeff[ op.vec1 ] );

    destVec.getValues( destData );
    srcVec.getValues( srcData );
    for( int i = 0; i < op.size; ++i ) {
      destData[ op.destSwiz[i] ] = srcData[ op.srcSwiz[i] ]; 
    }
    destVec.setValues( destData );
  }
}


template<typename T, int P, int N>
void ShgaVersor<T, P, N>::updateSize() {
  int requiredVecs = numVecs[ m_grades ]; 
  if( size == requiredVecs ) {
    // TODO zero coefficients?
    return; 
  }
  if( size > 0 ) {
    delete[] m_coeff;
    m_coeff = 0;
  }
  if( requiredVecs > 0 ) {
    m_coeff = new T[ requiredVecs ];
  }
  size = requiredVecs;
}

}


#endif
