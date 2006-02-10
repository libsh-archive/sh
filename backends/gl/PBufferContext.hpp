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
#ifndef PBUFFERCONTEXT_HPP
#define PBUFFERCONTEXT_HPP

#include "ShProgram.hpp"
#include "GlBackend.hpp"
#include "ShMemory.hpp"

namespace shgl {

enum FloatExtension {
  SH_ARB_NV_FLOAT_BUFFER,
  SH_ARB_ATI_PIXEL_FORMAT_FLOAT,
  SH_ARB_NO_FLOAT_EXT
};

// A handle representing a previously active pbuffer.
class PBufferHandle : public SH::ShRefCountable {
public:
  virtual ~PBufferHandle();

  // Restore this context
  virtual void restore() = 0;

protected:
  PBufferHandle();
};

typedef SH::ShPointer<PBufferHandle> PBufferHandlePtr;
typedef SH::ShPointer<const PBufferHandle> PBufferHandleCPtr;

class PBufferContext;

class PBufferStorage : public SH::ShStorage {
public:
  ~PBufferStorage();

  SH::ShPointer<PBufferContext> context() const;

  std::string id() const { return "opengl:pbuffer"; }
  
private:
  SH::ShPointer<PBufferContext> m_context;
};

typedef SH::ShPointer<PBufferStorage> PBufferStoragePtr;
typedef SH::ShPointer<const PBufferStorage> PBufferStorageCPtr;

class PBufferContext : public SH::ShRefCountable {
public:
  virtual ~PBufferContext();

  int width() const { return m_width; }
  int height() const { return m_height; }
  const void* id() const { return m_id; }
  
  virtual PBufferHandlePtr activate() = 0;

protected:
  PBufferContext(int width, int height, void* id);
  
  int m_width, m_height;
  void* m_id;
};

typedef SH::ShPointer<PBufferContext> PBufferContextPtr;
typedef SH::ShPointer<const PBufferContext> PBufferContextCPtr;

class PBufferFactory {
public:
  virtual ~PBufferFactory();
  
  // Make a context if necessary, or return an existing one which
  // matches width, height and id.
  virtual PBufferContextPtr get_context(int width, int height, void* id = 0) = 0;

  virtual FloatExtension get_extension() = 0;
  
  static PBufferFactory* instance();
  
protected:
  PBufferFactory();
};

}

#endif
