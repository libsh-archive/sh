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

#ifndef SHLA_VECTORIMPL_HPP
#define SHLA_VECTORIMPL_HPP

#include "ShlaVector.hpp"
#include "ShDebug.hpp"
#include "ShlaRenderGlobal.hpp"

namespace Shla {

using namespace SH;

template< typename T, int M, int N, ShlaVectorKind K >
ShlaVector<T, M, N, K>::ShlaVector()
  : m_mem(new ShUberbuffer( M, N, 1, T::typesize )) { 
}

template< typename T, int M, int N, ShlaVectorKind K >
template<ShlaVectorKind K2>
ShlaVector<T, M, N, K>::ShlaVector( const ShlaVector<T, M, N, K2> &b ) { 
  SH_DEBUG_WARN( "Copy Constructor may not work\n" );
  m_mem = b.getMem(); // TODO should use cloning
}

template< typename T, int M, int N, ShlaVectorKind K >
ShlaVector<T, M, N, K>::~ShlaVector() {
}

template< typename T, int M, int N, ShlaVectorKind K >
template<ShlaVectorKind K2>
ShlaVector<T, M, N, K>& ShlaVector<T, M, N, K>::operator=( const ShlaVector<T, M, N, K2> &b ) { 
  switch( K ) {
    case SHLA_DEFAULT_VECTOR:
      m_mem = new ShUberbuffer( M, N, 1, T::typesize );
      m_mem->copy( b.getMem() );
      break;
    case SHLA_TEMP_VECTOR:
      m_mem = b.getMem();
      break;
    default:
      SH_DEBUG_ERROR( "Unsupported ShlaVectorKind " << K );
  }
  return *this;
}

template< typename T, int M, int N, ShlaVectorKind K >
ShUberbufferPtr ShlaVector<T, M, N, K>::getMem() const { 
  // TODO fix this (clone ubufs on assignment) 
  // or shield access to the memory object in some way
  if( m_mem.refCount() > 1 ) {
    SH_DEBUG_WARN( "getMem() on ShlaVector whose memory object is shared." );
    SH_DEBUG_WARN( "Do not write to the uber buffer unless you know what you're doing." );
  }
  return m_mem;
}

template< typename T, int M, int N, ShlaVectorKind K >
T ShlaVector<T, M, N, K>::operator()( int i ) const {
  T result;
  float *elementData = m_mem->data( i % M, i / M ); 
  // TODO hack because setValues requires a double array
  for( int i = 0; i < T::typesize; ++i ) {
    result.node()->setValue( i, elementData[i] );
  }
  delete[] elementData;
  return result; 
}

template< typename T, int M, int N, ShlaVectorKind K >
void ShlaVector<T, M, N, K>::setData( const float* data ) {
  if( m_mem.refCount() > 1 ) {
    SH_DEBUG_PRINT( "Found shared mem object on setData.  Making new buf" );
    m_mem = new ShUberbuffer( M, N, 1, T::typesize );
  }
  m_mem->setData( data );
}

template< typename T, int M, int N, ShlaVectorKind K >
void ShlaVector<T, M, N, K>::printData(int c) { 
  ShlaRenderGlobal<T, M, N>::printData( m_mem, c );
}


}

#endif
