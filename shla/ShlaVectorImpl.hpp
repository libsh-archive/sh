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

template< typename T, int M, int N >
ShlaVector<T, M, N>::ShlaVector()
  : m_mem(new ShUberbuffer( M, N, 1, T::typesize )) { 
}

template< typename T, int M, int N >
ShlaVector<T, M, N>::ShlaVector( const ShlaVector<T, M, N> &b ) { 
  SH_DEBUG_WARN( "Copy Constructor may not work\n" );
  m_mem = b.m_mem; // TODO should use cloning
}

template< typename T, int M, int N >
ShlaVector<T, M, N>::~ShlaVector() {
}

template< typename T, int M, int N >
ShlaVector<T, M, N>& ShlaVector<T, M, N>::operator=( const ShlaVector<T, M, N> &b ) { 
  //TODO do this properly (copy uber buffers)
  m_mem = b.m_mem;
  return *this;
}

template< typename T, int M, int N >
ShUberbufferPtr ShlaVector<T, M, N>::getMem() const { 
  return m_mem;
}

template< typename T, int M, int N >
T ShlaVector<T, M, N>::operator()( int i ) const {
  T result;
  float *elementData = m_mem->data( i % M, i / M ); 
  // TODO hack because setValues requires a double array
  for( int i = 0; i < T::typesize; ++i ) {
    result.node()->setValue( i, elementData[i] );
  }
  delete[] elementData;
  return result; 
}

template< typename T, int M, int N >
void ShlaVector<T, M, N>::setData( const float* data ) {
  m_mem->setData( data );
}


template< typename T, int M, int N >
float* ShlaVector<T, M, N>::zeros = 0;

template< typename T, int M, int N >
void ShlaVector<T, M, N>::zeroData() {
  // TODO faster way to do this?
  if( zeros == 0 ) {
    zeros = new float[ M * N * T::typesize ];
    memset( zeros, 0, M * N * T::typesize * sizeof( float ) );
  }
  m_mem->setData( zeros );
}


}

#endif
