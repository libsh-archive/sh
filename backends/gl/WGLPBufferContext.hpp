// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef WGLPBUFFERCONTEXT_HPP
#define WGLPBUFFERCONTEXT_HPP

#include "ShStream.hpp"
#include "ShProgram.hpp"
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

typedef SH::ShPointer<WGLPBufferContext> WGLPBufferContextPtr;
typedef SH::ShPointer<const WGLPBufferContext> WGLPBufferContextCPtr;

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
