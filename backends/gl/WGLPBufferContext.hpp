// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHWGLPBUFFERCONTEXT_HPP
#define SHWGLPBUFFERCONTEXT_HPP

#include "Stream.hpp"
#include "Program.hpp"
#include "PBufferContext.hpp"

namespace shgl {

class WGLPBufferHandle : public PBufferHandle {
public:
  WGLPBufferHandle(HDC drawable,
                   HGLRC context);
  
  virtual ~WGLPBufferHandle();

  virtual void restore();

private:
  HDC m_dc;
  HGLRC m_hglrc;
};

class WGLPBufferContext : public PBufferContext {
public:
  WGLPBufferContext(int width, int height,
                    void* id,
		    HPBUFFERARB pbuffer,
		    HDC dc, HGLRC hglrc);
  
  virtual ~WGLPBufferContext();
  
  virtual PBufferHandlePtr activate();

private:
  HPBUFFERARB m_pbuffer;
  HDC m_dc;
  HGLRC m_hglrc;
};

typedef SH::Pointer<WGLPBufferContext> WGLPBufferContextPtr;
typedef SH::Pointer<const WGLPBufferContext> WGLPBufferContextCPtr;

class WGLPBufferFactory : public PBufferFactory {
public:
  static WGLPBufferFactory* instance();

  PBufferContextPtr get_context(int width, int height, void* id);
  FloatExtension get_extension();
  
protected:
  WGLPBufferFactory();
  virtual ~WGLPBufferFactory();

private:
  WGLPBufferContextPtr create_context(int width, int height, void* id);

  void init_dc();
  void init_config();

  HDC m_dc;
  int m_format;
  FloatExtension m_extension;

  std::list<WGLPBufferContextPtr> m_contexts;

  static WGLPBufferFactory* m_instance;
};

}

#endif
