// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors:  Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//           Bryan Chan, Michael McCool
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
#include "ShlaRenderGlobal.hpp"
#include "ShlaMatrix.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaLib.hpp
 * This is an implementation of the linear algebra operators.
 */

// TODO handle operands with different T
#define SHLA_LIB_VEC_OP( op, op_src ) \
  template<typename T, int M, int N>  \
  ShlaVector<T, M, N> op( const ShlaVector<T, M, N> &a, const ShlaVector<T, M, N> &b ) { \
    /* only use static variables in the shader */ \
    typedef ShlaRenderGlobal<T, M, N> global; \
    static ShProgram fsh; \
    static ShTexture2D<T>& op1 = global::op1; \
    static ShTexture2D<T>& op2 = global::op2; \
    if( !fsh ) {\
      ShEnvironment::boundShader[0] = 0; \
      ShEnvironment::boundShader[1] = 0; \
      fsh = SH_BEGIN_FRAGMENT_PROGRAM { \
        ShInputTexCoord2f tc;\
        ShInputPosition4f p;\
        typename global::OutputColorType out;\
        out = op_src; \
      } SH_END_PROGRAM;\
    }\
    \
    ShlaVector<T, M, N> result; \
\
    global::renderbuf->bind( result.getMem() );\
    op1.attach( a.getMem() );\
    op2.attach( b.getMem() );\
\
    global::bindDefault( fsh ); \
\
    ShFramebufferPtr oldfb = ShEnvironment::framebuffer;\
    global::useRenderbuf();\
    global::drawQuad();\
    global::detachAll();\
    shDrawBuffer( oldfb );\
\
    return result; \
  }

SHLA_LIB_VEC_OP( operator+, op1(tc) + op2(tc) ); 
SHLA_LIB_VEC_OP( max, max( op1(tc), op2(tc) ) );
SHLA_LIB_VEC_OP( min, min( op1(tc), op2(tc) ) );
SHLA_LIB_VEC_OP( operator*, op1(tc) * op2(tc) );
SHLA_LIB_VEC_OP( operator/, op1(tc) / op2(tc) );
SHLA_LIB_VEC_OP( operator-, op1(tc) - op2(tc) );

#define SHLA_LIB_RIGHT_SCALAR_OP( op, op_src ) \
  template<typename T, int M, int N>  \
  ShlaVector<T, M, N> op( const ShlaVector<T, M, N> &a, double s ) { \
    /* only use static variables in the shader */ \
    typedef ShlaRenderGlobal<T, M, N> global; \
    static ShProgram fsh; \
    static ShTexture2D<T>& op1 = global::op1; \
    if( !fsh ) {\
      ShEnvironment::boundShader[0] = 0; \
      ShEnvironment::boundShader[1] = 0; \
      fsh = SH_BEGIN_FRAGMENT_PROGRAM { \
        ShInputTexCoord2f tc;\
        ShInputPosition4f p;\
        typename global::OutputColorType out;\
        out = op_src; \
      } SH_END_PROGRAM;\
    }\
    \
    ShlaVector<T, M, N> result; \
\
    global::renderbuf->bind( result.getMem() );\
    op1.attach( a.getMem() );\
\
    global::bindDefault( fsh ); \
\
    ShFramebufferPtr oldfb = ShEnvironment::framebuffer;\
    global::useRenderbuf();\
    global::drawQuad();\
    global::detachAll();\
    shDrawBuffer( oldfb );\
\
    return result; \
  }

// uses RIGHT_SCALAR_OP to make a commutative vector-scalar operator
#define SHLA_LIB_COMMUTATIVE_SCALAR_OP( op, op_src ) \
  SHLA_LIB_RIGHT_SCALAR_OP( op, op_src ); \
  template< typename T, int M, int N> \
    ShlaVector<T, M, N> op( double s, const ShlaVector<T, M, N> &a ) { \
      return op( a, s ); \
    } 
SHLA_LIB_RIGHT_SCALAR_OP( operator/, op1(tc) * ( 1.0 / s ) ); 
SHLA_LIB_COMMUTATIVE_SCALAR_OP( operator*, op1(tc) * s ); 


//TODO this should return something of type T
// implement for non-square, non-power of 2 vectors
// reduces in half until reaching size. (must have size < N / 2, power of 2)
#define SHLA_LIB_REDUCE_OP( op, op_src ) \
template<typename T, int N> \
T op( const ShlaVector<T, N, N> &a, int size = 1 ) { \
  /* only use static variables in the shader */ \
  typedef ShlaRenderGlobal<T, N, N> global;\
  static ShProgram fsh; \
  static ShTexture2D<T>& accum = global::accum; \
  if( !fsh ) {\
    ShEnvironment::boundShader[0] = 0;\
    ShEnvironment::boundShader[1] = 0;\
    fsh = SH_BEGIN_FRAGMENT_PROGRAM { \
      ShInputTexCoord2f tc[4]; \
      ShInputPosition4f p;  \
      typename global::TempType c[4]; /* components */ \
      typename global::OutputColorType out; \
      /* TODO get rid of smallenough by using smaller drawQuad*/ \
      ShAttrib1f smallenough = ( tc[0](0) <= 1.0  ) * ( tc[0](1) <= 1.0 ); \
      for( int i = 0; i < 4; ++i ) c[i] = accum(tc[i]); \
      out = smallenough * ( op_src ); \
    } SH_END_PROGRAM;\
  }\
\
  ShlaVector<T, N, N> resultVec; \
  global::bindReduce( fsh );\
  ShFramebufferPtr oldfb = ShEnvironment::framebuffer;\
\
  global::accumInit( a.getMem()); \
  for( int curN = N / 2; curN >= size; curN /= 2 ) { \
    /* TODO handle very small vectors */ \
    if( curN == size ) {\
      global::accumLastLoop( resultVec.getMem() );\
    } else if( curN < N / 2 ) {\
      global::accumLoop();\
    }\
    /* TODO find way to get rid of the clearing \
     * and just draw smaller quads */\
    global::useRenderbuf();\
    global::drawQuad();\
    printf( "curN: %d\n", curN );\
    shDrawBuffer( 0 ); /* TODO get rid of temp fix for u-buffer bugs */\
  }\
  global::detachAll();\
  shDrawBuffer( oldfb );\
\
  return resultVec(0); \
}

SHLA_LIB_REDUCE_OP( reduceAbs, abs( c[0] ) + abs( c[1] ) + abs( c[2] ) + abs( c[3] ) );
SHLA_LIB_REDUCE_OP( reduceAdd, c[0] + c[1] + c[2] + c[3] );
SHLA_LIB_REDUCE_OP( reduceMax, max( max( c[0], c[1] ), max( c[2], c[3] ) ) );
SHLA_LIB_REDUCE_OP( reduceMin, min( min( c[0], c[1] ), min( c[2], c[3] ) ) ); 
SHLA_LIB_REDUCE_OP( reduceMul, c[0] * c[1] * c[2] * c[3] );

// Does iter iterations of unpreconditioned conjugate gradient
/// TODO change BandedMatrix to Matrix
template<typename T, int N>
ShlaVector<T, N, N> conjugateGradient( ShlaBandedMatrix<T, N, N> &A, ShlaVector<T, N, N> &b, int iter = 4 ) {
  T alpha, beta, rho;
  ShlaVector<T, N, N> p, q, r, x;
 
  p = r = b - ( A | x );
  for( int i = 0; i < 4; ++i ) {
    rho = reduceAdd( r * r );
    q = A | p;
    alpha = rho / ( reduceAdd( p * q ) ); 
    x = x + p * alpha;
    r = r - alpha * q;
    beta = reduceAdd( r * r ) / rho;
    p = r + beta * p;
  }
  return x;
}


}


#endif
