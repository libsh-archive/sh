#ifndef PBUFFERCONTEXT_HPP
#define PBUFFERCONTEXT_HPP

#include "ShProgram.hpp"
#include "GlBackend.hpp"

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
  
protected:
  PBufferFactory();
};

}

#endif
