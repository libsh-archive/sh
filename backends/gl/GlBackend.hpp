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
//#include <GL/glut.h>

#endif /* WIN32 */

namespace shgl {

struct TextureStrategy {
  virtual TextureStrategy* create(int context) = 0;
  
  virtual void bindTexture(const SH::ShTextureNodePtr& texture,
                           GLenum target) = 0;
};

struct StreamStrategy {
  virtual StreamStrategy* create(int context) = 0;
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest) = 0;
};

struct CodeStrategy {
  virtual CodeStrategy* create(int context) = 0;
  virtual SH::ShBackendCodePtr generate(const std::string& target,
                                        const SH::ShProgramNodeCPtr& shader,
                                        TextureStrategy* textures) = 0;
};

class GlBackend : public SH::ShBackend {
public:
  virtual SH::ShBackendCodePtr generateCode(const std::string& target,
                                            const SH::ShProgramNodeCPtr& shader);

  // execute a stream program, if supported
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

  virtual int newContext();
  virtual int context() const;
  virtual void setContext(int context);
  virtual void destroyContext();
  
protected:
  GlBackend(CodeStrategy* code, TextureStrategy* texture,
            StreamStrategy* stream);
  
private:
  int m_curContext;

  struct Context {
    Context(CodeStrategy* code,
            TextureStrategy* textures,
            StreamStrategy* streams)
      : code(code), textures(textures), streams(streams)
    {
    }
    
    CodeStrategy* code;
    TextureStrategy* textures;
    StreamStrategy* streams;
  };
  
  std::vector<Context> m_contexts;

  // NOT IMPLEMENTED
  GlBackend(const GlBackend& other);
  GlBackend& operator=(const GlBackend& other);
};

void shGlCheckError(const char* desc, const char* file, int line);

}

#define SH_GL_CHECK_ERROR(op) \
  op;shGlCheckError( # op, (char*) __FILE__, (int) __LINE__);



#endif
