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
#ifndef SHLA_LIB_HPP 
#define SHLA_LIB_HPP 

#include "ShlaVector.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaLib.hpp
 * This is an implementation of the linear algebra operators.
 */

// TODO handle operands with different T
#define SHLA_LIB_VEC_OP( op, op_src ) \
  template<typename T, int M, int N>  \
  ShlaVector<T, M, N> op( ShlaVector<T, M, N> &a, ShlaVector<T, M, N> &b ) { \
    ShlaVector<T, M, N> result; \
    static ShProgram fsh; \
    \
    if( !fsh ) {\
      ShEnvironment::boundShader[0] = 0; \
      ShEnvironment::boundShader[1] = 0; \
      fsh = SH_BEGIN_FRAGMENT_PROGRAM { \
        ShInputTexCoord2f tc;\
        ShInputPosition4f p;\
        typename ShlaVector<T, M, N>::OutputColorType out;\
        out = op_src; \
      } SH_END_PROGRAM;\
    }\
\
    a.renderbuf->bind( result.getMem() );\
    a.op1.attach( a.getMem() );\
    a.op2.attach( b.getMem() );\
\
    a.bindDefault( fsh ); \
\
    ShFramebufferPtr oldfb = ShEnvironment::framebuffer;\
    a.useRenderbuf();\
    a.drawQuad();\
    a.detachAll();\
    shDrawBuffer( oldfb );\
\
    return result; \
  }

SHLA_LIB_VEC_OP( operator+, a.op1(tc) + a.op2(tc) ); 
SHLA_LIB_VEC_OP( max, max( a.op1(tc), a.op2(tc) ) );
SHLA_LIB_VEC_OP( min, min( a.op1(tc), a.op2(tc) ) );
SHLA_LIB_VEC_OP( operator*, a.op1(tc) * a.op2(tc) );
SHLA_LIB_VEC_OP( operator-, a.op1(tc) - a.op2(tc) );


//TODO this should return something of type T
// implement for non-square, non-power of 2 vectors
template<typename T, int N> 
T reduceAdd( ShlaVector<T, N, N> &a ) { 
  ShlaVector<T, N, N> resultVec; 
  T result;

  static ShProgram fsh; 
  if( !fsh ) {
    ShEnvironment::boundShader[0] = 0;
    ShEnvironment::boundShader[1] = 0;
    fsh = SH_BEGIN_FRAGMENT_PROGRAM { 
      ShInputTexCoord2f tc[4]; 
      ShInputPosition4f p;  
      typename ShlaVector<T, N, N>::TempType c[4]; /* components */ 
      typename ShlaVector<T, N, N>::OutputColorType out; 

      //TODO get rid of toobig
      ShAttrib1f toobig = ( tc[0](0) > 1.0 ) + ( tc[0](1) > 1.0 );
      toobig = min( toobig, 1.0 );

      for( int i = 0; i < 4; ++i ) c[i] = a.accum(tc[i]); 
      out = ( 1.0 - toobig ) * ( c[0] + c[1] + c[2] + c[3] );
    } SH_END_PROGRAM;
  }
  a.bindReduce( fsh );
  ShFramebufferPtr oldfb = ShEnvironment::framebuffer;

  a.accumInit( a.getMem());
  int curN = N;
  for( int curN = N / 2; curN >= 1; curN /= 2 ) {
    //TODO handle very small vectors
    if( curN < 4 ) {
      a.accumLastLoop( resultVec.getMem() );
    } else if( curN < N / 2 ) {
      a.accumLoop();
    }
    //TODO get rid of clear, and just draw over original values
    a.useRenderbuf();
    a.drawQuad();
    printf( "curN: %d\n", curN );
  }

  a.detachAll();
  shDrawBuffer( oldfb );

  for( int i = 0; i < T::typesize; ++i ) {
    result.node()->setValue( i, resultVec(0, i) );
  }

  return result; 
}

/*
SHLA_LIB_REDUCE_OP( reduceAbs, abs( c[0] ) + abs( c[1] ) + abs( c[2] ) + abs( c[3] ) );
SHLA_LIB_REDUCE_OP( reduceAdd, SHLA_REDUCE_ADD );
SHLA_LIB_REDUCE_OP( reduceMax, max( max( c[0], c[1] ), max( c[2], c[3] ) ) );
SHLA_LIB_REDUCE_OP( reduceMin, min( min( c[0], c[1] ), min( c[2], c[3] ) ) ); 
SHLA_LIB_REDUCE_OP( reduceMul, c[0] * c[1] * c[2] * c[3] );
*/


}


#endif
