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
#ifndef GLBACKEND_HPP
#define GLBACKEND_HPP

#include "ShProgram.hpp"
#include "ShStream.hpp"
#include "ShSchedule.hpp"	// TODO: Create a header file
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

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
//#include <GL/glut.h>
#endif /* __APPLE */

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
  // TODO: Add execute for a schedule

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
