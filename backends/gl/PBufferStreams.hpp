// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef PBUFFERSTREAMS_HPP
#define PBUFFERSTREAMS_HPP

#include "ShProgram.hpp"
#include "GlBackend.hpp"

namespace shgl {

enum FloatExtension {
  SH_ARB_NV_FLOAT_BUFFER,
  SH_ARB_ATI_PIXEL_FORMAT_FLOAT,
  SH_ARB_NO_FLOAT_EXT
};

#if defined( WIN32 )
#elif defined( __APPLE__ )
#include <AGL/agl.h>
struct ShAGLPBufferInfo {
  ShAGLPBufferInfo()
    : extension(SH_ARB_NO_FLOAT_EXT),
      pixformat(0), pbuffer(0), context(0), width(0), height(0),
      shcontext(-1)
  {
  }

  FloatExtension extension;
  AGLPixelFormat pixformat; // will be the pixel format for the Pbuffer
  AGLContext     context;   // the Pbuffer context
  AGLPbuffer     pbuffer;   // the equivalent to the drawable

  int width, height;
  int shcontext;
  bool valid() { return extension != SH_ARB_NO_FLOAT_EXT
                   && pbuffer
                   && context; }
};
#else // Linux
struct ShGLXPBufferInfo {
  ShGLXPBufferInfo()
    : extension(SH_ARB_NO_FLOAT_EXT),
      pbuffer(0), context(0), width(0), height(0),
      shcontext(-1)
  {
  }
  FloatExtension extension;

  GLXPbuffer pbuffer;
  GLXContext context;
  int width, height;
  int shcontext;
  bool valid() { return extension != SH_ARB_NO_FLOAT_EXT
                   && pbuffer
                   && context; }
};
#endif


struct PBufferStreams : public StreamStrategy {
  PBufferStreams(int context = 0);
  virtual ~PBufferStreams();
  virtual StreamStrategy* create(int context);
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

#if defined( WIN32 )
#elif defined( __APPLE__ )
  void makeCurrenContext()
  {
  	aglSetCurrentContext( m_info.context );
  }
#else
  void makeCurrenContext()
  {
    glXMakeCurrent(m_display, m_info.pbuffer, m_info.context);
  }
#endif

private:
  int m_context;

  FloatExtension setupContext(int width, int height);

  int m_setup_vp;
  SH::ShProgram m_vp;

  
#if defined( WIN32 )
#elif defined( __APPLE__ )
  ShAGLPBufferInfo m_info;
#else
  Display* m_display;
  ShGLXPBufferInfo m_info;
#endif
};

}
#endif
