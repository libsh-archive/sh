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
ShlaVector<T, M, N>::ShlaVector(): m_mem(0) { 
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
  ShDataMemoryObjectPtr bDataMem= b.m_mem;
  if( bDataMem ) {
    ShDataMemoryObjectPtr myDataMem = new ShDataMemoryObject( M, N, 1, T::typesize );
    float *bData = bDataMem->data();
    myDataMem->setData( bData );
    m_mem = myDataMem;
    delete[] bData;
  } else {
#if 0
    ShUberbufferPtr bUbuf = b.m_mem;
    if( bUbuf ) {
      /* only use static variables in the copy shader */ 
      // TODO switch this to use glCloneMem in backend when that 
      // entry point is supported
      typedef ShlaRenderGlobal<T, M, N> global;
      static ShProgram fsh; 
      static ShTexture2D<T>& btex = global::op1; 
      if( !fsh ) {
        ShEnvironment::boundShader[0] = 0;
        ShEnvironment::boundShader[1] = 0;
        fsh = SH_BEGIN_FRAGMENT_PROGRAM { 
          ShInputTexCoord2f tc; 
          ShInputPosition4f p;  
          typename global::OutputColorType out; 
          out = btex(tc); 
        } SH_END_PROGRAM;
      }
      /* end of static section */
      btex.attach( bUbuf );  
      ShFramebufferPtr oldfb = ShEnvironment::framebuffer;
      
      ShUberbufferPtr myUbuf = new ShUberbuffer( M, N, 1, T::typesize );
      global::renderbuf->bind( myUbuf );
      global::bindDefault( fsh );
      global::useRenderbuf();
      global::drawQuad();

      /* restore old state */
      global::detachAll();
      shDrawBuffer( oldfb );
      m_mem = myUbuf;
    } else {
      m_mem = 0;
    } 
#endif
    // TODO fix above
    m_mem = b.m_mem;
  }
  return *this;
}

template< typename T, int M, int N >
ShUberbufferPtr ShlaVector<T, M, N>::getMem() { 
  ShUberbufferPtr ubuf = upload();
  return ubuf;
}

template< typename T, int M, int N >
const float& ShlaVector<T, M, N>::operator()( int i, int element ) const {
  ShDataMemoryObjectPtr dataMem = download(); 
  return (*dataMem)( i % M, i / M, 0, element ); 
}

template< typename T, int M, int N >
float& ShlaVector<T, M, N>::operator()( int i, int element ) {
  ShDataMemoryObjectPtr dataMem = download(); 
  return (*dataMem)( i % M, i / M, 0, element ); 
}

template< typename T, int M, int N >
void ShlaVector<T, M, N>::setData( float* data ) {
  ShDataMemoryObjectPtr dataMem = m_mem;
  if( !dataMem ) m_mem = dataMem = new ShDataMemoryObject( M, N, 1, T::typesize ); 
  dataMem->setData( data );
}

template< typename T, int M, int N >
void ShlaVector<T, M, N>::zeroData() {
  m_mem = new ShDataMemoryObject( M, N, 1, T::typesize );
}

template< typename T, int M, int N >
ShDataMemoryObjectPtr ShlaVector<T, M, N>::getDataMem() {
  ShDataMemoryObjectPtr dataMem = download(); 
  return dataMem; 
}

template< typename T, int M, int N >
ShUberbufferPtr ShlaVector<T, M, N>::upload() {
  ShUberbufferPtr ubuf = m_mem;
  if( !ubuf ) { 
    ShDataMemoryObjectPtr dataMem = m_mem;

    m_mem = ubuf = new ShUberbuffer( M, N, 1, T::typesize );
    float *data;
    if( dataMem ) { // if data exists, copy
      data = dataMem->data();
    } else {
      data = new float[ M * N * T::typesize ];
      memset( data, 0, M * N * T::typesize * sizeof( float ) );
    }

    ubuf->setData( data ); 
    delete[] data;
  }
  return ubuf;
}

template< typename T, int M, int N >
ShDataMemoryObjectPtr ShlaVector<T, M, N>::download() {
  ShDataMemoryObjectPtr dataMem = m_mem;
  if( !dataMem ) {
    ShUberbufferPtr ubuf = m_mem;
    m_mem = dataMem = new ShDataMemoryObject( M, N, 1, T::typesize ); 
    if( ubuf ) {
      float* ubufData = ubuf->data();
      for( int i = 0; i < N; ++i ) {
        std::cout << "Row #" << i << "*****" << std::endl;
        for( int j = 0; j < M; ++j ) {
          std::cout << "  C# " << j << ": "; 
          for( int k = 0; k < T::typesize; ++k ) {
            std::cout << ubufData[ T::typesize * ( M * i + j ) + k ] << " ";
          }
          std::cout << std::endl;
        }
      }
      dataMem->setData( ubufData ); 
      delete ubufData;
      //TODO implement converting ubuf to data memory object
      // This should be doable in the bakcned when glGetMemImage method is implemented
      SH_DEBUG_WARN( "download might not work" );
    } 
  } 
  return dataMem;
}


}

#endif
