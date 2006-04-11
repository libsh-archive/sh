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
#ifndef GLXPBUFFERCONTEXT_HPP
#define GLXPBUFFERCONTEXT_HPP

#include "Stream.hpp"
#include "Program.hpp"
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

typedef SH::Pointer<GLXPBufferContext> GLXPBufferContextPtr;
typedef SH::Pointer<const GLXPBufferContext> GLXPBufferContextCPtr;

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
