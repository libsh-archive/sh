#include "Limits.hpp"

#include "ShDebug.hpp"

#include <iostream>

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>

#define CHECK_ERROR(op) \
  op;shGlCheckError( # op, (char*) __FILE__, (int) __LINE__)

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

namespace rds {

void Limits::dump_limits()
{
  std::cout << "Limits:" << std::endl;
  std::cout << "\t Instructions: " << m_instrs << std::endl;
  std::cout << "\t Half-float temporaries: " << m_halftemps << std::endl;
  std::cout << "\t Temporaries: " << m_temps << std::endl;
  std::cout << "\t Attributes: " << m_attribs << std::endl;
  std::cout << "\t Parameters: " << m_params << std::endl;
  std::cout << "\t Textures: " << m_texs << std::endl;
}

ArbLimits::ArbLimits(const std::string& target)
{
  unsigned int arb_target;
  /*if (target == "vertex")
    arb_target = GL_VERTEX_PROGRAM_ARB;
  else if (target == "fragment")
    arb_target = GL_FRAGMENT_PROGRAM_ARB;
  else
    arb_target = 0;
  
  m_instrs = (target == "gpu:vertex" ? 128 : 48);
  CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,
                                        &m_instrs));

  // TODO implement proper detection of half-float limits.
  m_halftemps = (target == "gpu:vertex" ? 0 : 64);

  // TODO implement proper detection of NVIDIA.
  //  NVIDIA's GlGetProgramivARB does not update m_temps, so set it to 32 here.
  //  ATI will still have the right number because its drivers should set m_temps properly.
   
  m_temps = (target == "gpu:vertex" ? 12 : 32);
  CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEMPORARIES_ARB,
                                        &m_temps));
      
  m_attribs = (target == "gpu:vertex" ? 16 : 10);
  CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_ATTRIBS_ARB,
                                        &m_attribs));
      
  m_params = (target == "gpu:vertex" ? 96 : 24);
  CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,
                                        &m_params));

  // Only get texture instructions for fragment targets
  m_texs = (target == "gpu:vertex" ? 0 : 24);
  if (target == "gpu:vertex") {
    CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,
                                          &m_texs));
  }*/
}

FakeLimits::FakeLimits() {
  m_instrs = 3;
  m_halftemps=6;
  m_temps=6;
  m_attribs=5;
  m_params=5;
  m_texs=5;
}

GraphLimits::GraphLimits() {
  m_instrs = 256;
  m_halftemps=0;
  m_temps=0;
  m_attribs=0;
  m_params=0;
  m_texs=0;
}

}
