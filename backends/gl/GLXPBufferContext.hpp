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
#ifndef GLXPBUFFERCONTEXT_HPP
#define GLXPBUFFERCONTEXT_HPP

#include "ShStream.hpp"
#include "ShProgram.hpp"
#include "PBufferContext.hpp"

namespace shgl {

class GLXPBufferHandle : public PBufferHandle {
public:
  GLXPBufferHandle(Display* display,
                   GLXDrawable drawable,
                   GLXContext context);
  
  virtual ~GLXPBufferHandle();

  virtual void restore();

private:
  Display* m_display;
  
  GLXDrawable m_drawable;
  GLXContext m_context;
};

class GLXPBufferContext : public PBufferContext {
public:
  GLXPBufferContext(int width, int height,
                    void* id,
                    Display* display, GLXPbuffer pbuffer,
                    GLXContext context);
  
  virtual ~GLXPBufferContext();
  
  virtual PBufferHandlePtr activate();

private:
  Display* m_display;

  GLXPbuffer m_pbuffer;
  GLXContext m_context;
};

typedef SH::ShPointer<GLXPBufferContext> GLXPBufferContextPtr;
typedef SH::ShPointer<const GLXPBufferContext> GLXPBufferContextCPtr;

class GLXPBufferFactory : public PBufferFactory {
public:
  static GLXPBufferFactory* instance();

  PBufferContextPtr get_context(int width, int height, void* id);
  FloatExtension get_extension();
  
protected:
  GLXPBufferFactory();
  virtual ~GLXPBufferFactory();

private:
  GLXPBufferContextPtr create_context(int width, int height, void* id);

  void init_display();
  void init_config();

  Display* m_display;
  GLXFBConfig* m_fb_config;
  FloatExtension m_extension;

  std::list<GLXPBufferContextPtr> m_contexts;

  static GLXPBufferFactory* m_instance;
};

}

#endif
