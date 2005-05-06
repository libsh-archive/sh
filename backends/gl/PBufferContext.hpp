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
  PBufferStorage(const SH::ShPointer<PBufferContext>& context,
                 SH::ShMemory* memory);

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

  PBufferStoragePtr make_storage(SH::ShMemory* memory) { return new PBufferStorage(this, memory); }

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
