#include "GlBackend.hpp"
#include "ShDebug.hpp"

#ifdef WIN32

PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = 0;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = 0;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = 0;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = 0;
PFNGLTEXIMAGE3DPROC glTexImage3D = 0;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = 0;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = 0;

#endif

namespace shgl {


void shGlCheckError(const char* desc, const char* file, int line)
{
  GLenum errnum = glGetError();
  char* error = 0;
  switch (errnum) {
  case GL_NO_ERROR:
    return;
  case GL_INVALID_ENUM:
    error = "GL_INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    error = "GL_INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    error = "GL_INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    error = "GL_STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    error = "GL_STACK_UNDERFLOW";
    break;
  case GL_OUT_OF_MEMORY:
    error = "GL_OUT_OF_MEMORY";
    break;
  case GL_TABLE_TOO_LARGE:
    error = "GL_TABLE_TOO_LARGE";
    break;
  default:
    error = "Unknown error!";
    break;
  }
  SH_DEBUG_PRINT("GL ERROR on " << file << ": " <<line<<": "<< error);
  SH_DEBUG_PRINT("GL ERROR call: " << desc);
}

GlBackend::GlBackend(CodeStrategy* code, TextureStrategy* textures,
                     StreamStrategy* streams)
  : m_curContext(0)
{
  m_contexts.push_back(Context(code, textures, streams));
}

SH::ShBackendCodePtr
GlBackend::generateCode(const std::string& target,
                        const SH::ShProgram& shader)
{
  return m_contexts[m_curContext].code->generate(target, shader,
                                                 m_contexts[m_curContext].textures);
}

void
GlBackend::execute(const SH::ShProgram& program,
                   SH::ShStream& dest)
{
  // TODO: error otherwise.
  if (m_contexts[m_curContext].streams) {
    m_contexts[m_curContext].streams->execute(program, dest);
  }
}

int GlBackend::newContext()
{
  int context_num = m_contexts.size();
  const Context& c = m_contexts[0];
  m_contexts.push_back(Context(c.code->create(context_num),
                               c.textures->create(context_num),
                               c.streams->create(context_num)));

  setContext(context_num);
  return context_num;
}

int GlBackend::context() const
{
  return m_curContext;
}

void GlBackend::setContext(int context)
{
  SH_DEBUG_ASSERT(0 <= context < m_contexts.size());
  m_curContext = context;
}

void GlBackend::destroyContext()
{
  if (m_curContext == 0) return;

  Context& c = m_contexts[m_curContext];
  delete c.code; c.code = 0;
  delete c.textures; c.textures = 0;
  delete c.streams; c.streams = 0;
  setContext(0);
}

}