#ifndef GLBACKEND_HPP
#define GLBACKEND_HPP

#include "ShProgram.hpp"
#include "ShStream.hpp"
#include "ShTextureNode.hpp"

#ifdef WIN32

#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

#else

#define GL_GLEXT_VERBOSE 1
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>

#endif /* WIN32 */

namespace shgl {

struct TextureStrategy {
  virtual void bindTexture(const SH::ShTextureNodePtr& texture,
                           GLenum target) = 0;
};

struct StreamStrategy {
  virtual void execute(const SH::ShProgram& program, SH::ShStream& dest) = 0;
};

struct CodeStrategy {
  virtual SH::ShBackendCodePtr generate(const std::string& target,
                                        const SH::ShProgram& shader,
                                        TextureStrategy* textures) = 0;
};

// TODO: Think about GL contexts.
// Maybe we should swap out the actual backend each time the context
// is changed.
// I think that might work well.

class GlBackend : public SH::ShBackend {
public:
  virtual SH::ShBackendCodePtr generateCode(const std::string& target,
                                            const SH::ShProgram& shader);

  // execute a stream program, if supported
  virtual void execute(const SH::ShProgram& program, SH::ShStream& dest);

protected:
  GlBackend(CodeStrategy* code, TextureStrategy* texture,
            StreamStrategy* stream);
  
private:
  CodeStrategy* m_code;
  TextureStrategy* m_textures;
  StreamStrategy* m_stream;

  // NOT IMPLEMENTED
  GlBackend(const GlBackend& other);
  GlBackend& operator=(const GlBackend& other);
};

void shGlCheckError(const char* desc = "");

}

#define SH_GL_CHECK_ERROR(op) op; shGlCheckError( # op );

#endif
