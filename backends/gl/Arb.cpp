#include <GL/gl.h>
#include <GL/glext.h>
#include "GlBackend.hpp"
#include "Arb.hpp"
#include "ArbCode.hpp"

namespace shgl {

using namespace SH;

ArbCodeStrategy::ArbCodeStrategy(int context)
  : m_context(context)
{
}

ArbCodeStrategy* ArbCodeStrategy::create(int context)
{
  return new ArbCodeStrategy(context);
}

ShBackendCodePtr ArbCodeStrategy::generate(const std::string& target,
                                           const ShProgram& shader,
                                           TextureStrategy* textures)
{
  ArbCodePtr code = new ArbCode(shader, target, textures);
  code->generate();
  return code;
}

unsigned int arbTarget(const std::string& shTarget)
{
  if (shTarget == "gpu:vertex") return GL_VERTEX_PROGRAM_ARB;
  if (shTarget == "gpu:fragment") return GL_FRAGMENT_PROGRAM_ARB;  
  return 0;
}

}

