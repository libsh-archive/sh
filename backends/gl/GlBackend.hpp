// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef GLBACKEND_HPP
#define GLBACKEND_HPP

#include "ShVariableType.hpp"
#include "ShProgram.hpp"
#include "ShStream.hpp"
#include "ShTextureNode.hpp"

#ifdef _WIN32

#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

extern PFNGLTEXIMAGE3DPROC glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

// WGL_ARB_pixel_format
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

// WGL_ARB_pbuffer
extern PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
extern PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
extern PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
extern PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
extern PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB;
extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
extern PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
extern PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
extern PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC glUniform4fvARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORM2IARBPROC glUniform2iARB;
extern PFNGLUNIFORM3IARBPROC glUniform3iARB;
extern PFNGLUNIFORM4IARBPROC glUniform4iARB;
extern PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
extern PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
extern PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
extern PFNGLUNIFORM4IVARBPROC glUniform4ivARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB;
extern PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB;
extern PFNGLGETHANDLEARBPROC glGetHandleARB;

// extern PFN ARBPROC ARB;

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
#endif /* __APPLE */

#endif /* _WIN32 */

namespace shgl {

struct TextureStrategy {
  virtual ~TextureStrategy() {}
  virtual TextureStrategy* create() = 0;
  virtual void bindTexture(const SH::ShTextureNodePtr& texture,
                           GLenum target) = 0;
};

struct StreamStrategy {
  virtual ~StreamStrategy() {}
  virtual StreamStrategy* create() = 0;
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest) = 0;
};

struct CodeStrategy {
  virtual ~CodeStrategy() {}
  virtual CodeStrategy* create() = 0;
  virtual SH::ShBackendCodePtr generate(const std::string& target,
                                        const SH::ShProgramNodeCPtr& shader,
                                        TextureStrategy* texture) = 0;

  // If you want to use a special set generation function, override
  // generate_set, and override use_default_set() to return false.
  virtual SH::ShBackendSetPtr generate_set(const SH::ShProgramSet& s);
  virtual bool use_default_set() const;

  virtual void unbind_all_programs();
  virtual bool use_default_unbind_all() const;
};

class GlBackend : public SH::ShBackend {
public:
  virtual SH::ShBackendCodePtr generate_code(const std::string& target,
                                             const SH::ShProgramNodeCPtr& shader);
  virtual SH::ShBackendSetPtr generate_set(const SH::ShProgramSet& s);
  virtual void unbind_all_programs();

  // execute a stream program, if supported
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

protected:
  GlBackend(CodeStrategy* code, TextureStrategy* texture, StreamStrategy* stream,
            const std::string& name, const std::string& version);
  
private:
  CodeStrategy* m_code;
  TextureStrategy* m_texture;
  StreamStrategy* m_stream;

  // NOT IMPLEMENTED
  GlBackend(const GlBackend& other);
  GlBackend& operator=(const GlBackend& other);
};

void shGlCheckError(const char* desc, const char* file, int line);

/* Returns glReadPixels/glTexImage type for a given value type 
 * and returns a value type for the temporary buffer
 * (or SH_VALUETYPE_END if we can use the original buffer directly) 
 */
GLenum shGlType(SH::ShValueType valueType, SH::ShValueType &convertedType); 

}

#define SH_GL_CHECK_ERROR(op) \
  op;shGlCheckError( # op, (char*) __FILE__, (int) __LINE__);



#endif
