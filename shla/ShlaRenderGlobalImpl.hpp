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
#ifndef SHLA_RENDERGLOBALIMPL_HPP
#define SHLA_RENDERGLOBALIMPL_HPP

// TODO get rid of this when backend support for drawing geometry stabilizes
#include <GL/gl.h>

#include "ShlaRenderGlobal.hpp"
#include "ShSyntax.hpp"
#include "ShArrayData.hpp"
#include "ShEnvironment.hpp"
#include "ShDebug.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaRenderGlobalImpl.hpp
 */
template<typename T, int M, int N>
ShFramebufferPtr ShlaRenderGlobal<T, M, N>::renderbuf = new ShFramebuffer( M, N, 1, T::typesize );

template<typename T, int M, int N>
ShTexture2D<T> ShlaRenderGlobal<T, M, N>::op1( M, N );

template<typename T, int M, int N>
ShTexture2D<T> ShlaRenderGlobal<T, M, N>::op2( M, N );

template<typename T, int M, int N>
ShTexture2D<T> ShlaRenderGlobal<T, M, N>::accum( M, N );

template<typename T, int M, int N>
ShUberbufferPtr ShlaRenderGlobal<T, M, N>::accumRead = new ShUberbuffer( M, N, 1, T::typesize );

template<typename T, int M, int N>
ShUberbufferPtr ShlaRenderGlobal<T, M, N>::accumWrite = new ShUberbuffer( M, N, 1, T::typesize );

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::useRenderbuf() {
  shDrawBuffer( renderbuf );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::drawQuad( int w, int h, double z ) {
  // TODO hacked in for now
  // This should be in the backend when backends support drawing geometry
  double tcx = w / (double) M; 
  double tcy = h / (double) N; 
  double maxx = -1.0 + 2.0 * tcx; 
  double miny = 1.0 - 2.0 * tcy; 

  //TODO fix ShVertexArray stuff
  /*
  double verts[] = { -1.0, 1.0, z, 
                     maxx, 1.0, z, 
                     maxx, miny, z,
                     -1.0, miny, z };
  double normals[] = { 0.0, 0.0, 1.0,
                       0.0, 0.0, 1.0,
                       0.0, 0.0, 1.0,
                       0.0, 0.0, 1.0 };
  double tc[] = { 0.0, 0.0,
                  tcx, 0.0,
                  tcx, tcy,
                  0.0, tcy };
  unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };

  ShVertexArray array; 
  array.addData( verts, SH_RGB, 4, "vertices" );
  array.addData( normals, SH_RGB, 4, "normals" );
  array.addData( tc, SH_2D, 4, "texture coordinates" );
  array.addIndex( indices, SH_TRI, 6, "indices" );

  if( !ShEnvironment::backend ) {
    SH_DEBUG_WARN( "Used renderQuad without a backend" );
    return;
  }
  ShEnvironment::backend->render( array );
  */
  glBegin( GL_QUADS );
    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( -1.0, 1.0, z );

    glTexCoord2f( tcx, 0.0 );
    glVertex3f( maxx, 1.0, z );

    glTexCoord2f( tcx, tcy );
    glVertex3f( maxx, miny, z );

    glTexCoord2f( 0.0, tcy );
    glVertex3f( -1.0, miny, z );
  glEnd();
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::drawQuad() {
  drawQuad( M, N, 0.0 ); 
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::detachAll() {
  renderbuf->bind( 0 );
  op1.attach( 0 );
  op2.attach( 0 );
  accum.attach( 0 );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::bindDefault( ShProgram fsh ) {
  static ShProgram defaultVsh;

  ShEnvironment::boundShader[0] = 0;
  ShEnvironment::boundShader[1] = 0;

  if( !defaultVsh ) {
    printf( "defaultVsh\n" );
    defaultVsh = SH_BEGIN_VERTEX_PROGRAM {
      ShInputTexCoord2f tci;
      ShInputPosition4f pi;
      ShOutputTexCoord2f tco = tci;
      ShOutputPosition4f po = pi;
    } SH_END_PROGRAM;
  }

  shBindShader( defaultVsh );
  shBindShader( fsh );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::bindReduce( ShProgram fsh ) {
  static ShProgram reduceVsh;

  ShEnvironment::boundShader[0] = 0;
  ShEnvironment::boundShader[1] = 0;

  if( !reduceVsh ) {
    printf( "reduceVsh\n" );
    reduceVsh = SH_BEGIN_VERTEX_PROGRAM {
      ShInputTexCoord2f tci;
      ShInputPosition4f pi;
      ShOutputTexCoord2f tco[4];
      ShOutputPosition4f po = pi;

      ShTexCoord2f twiceTci = mad( tci, 2.0, ShConstant2f( -0.5 / M, -0.5 / N ) );
      tco[0] = twiceTci;
      tco[1] = twiceTci + ShConstant2f( 0.0, 1.0 / N );
      tco[2] = twiceTci + ShConstant2f( 1.0 / M, 0.0 );
      tco[3] = twiceTci + ShConstant2f( 1.0 / M, 1.0 / N );
    } SH_END_PROGRAM;
  }


  shBindShader( reduceVsh );
  shBindShader( fsh );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::bindOutput( ShTexture2D<T> &tex ) { 
  ShProgram texFsh;

  ShEnvironment::boundShader[0] = 0;
  ShEnvironment::boundShader[1] = 0;
  texFsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputTexCoord2f tc;
    ShInputPosition4f p;
    OutputColorType out = tex( tc ); 
  } SH_END_PROGRAM;

  bindDefault( texFsh );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::accumInit( ShUberbufferPtr init ) {
  accum.attach( init );
  renderbuf->bind( accumWrite );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::accumLoop() {
  accumDetach();

  ShUberbufferPtr temp = accumRead;
  accumRead = accumWrite;
  /*
  accumWrite = temp;
  */
  // try without swapping
  accumWrite = new ShUberbuffer( M, N, 1, T::typesize ); 

  printf( "Attaching accumRead: %d to tex\n", accumRead->mem() );
  accum.attach( accumRead );
  // TODO add invalidate?
  renderbuf->bind( accumWrite );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::accumLastLoop( ShUberbufferPtr out ) {
  accumDetach();
  accum.attach( accumWrite );
  // TODO  add invalidate?
  renderbuf->bind( out );
}

template<typename T, int M, int N>
void ShlaRenderGlobal<T, M, N>::accumDetach() {
  renderbuf->bind( 0 );
  accum.attach( 0 );
}

}

#endif
