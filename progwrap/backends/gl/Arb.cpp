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
                                           const ShProgramNodeCPtr& shader,
                                           TextureStrategy* textures)
{
  std::string::size_type loc = target.rfind(':');
  std::string unit = (loc == std::string::npos ? target : target.substr(loc + 1));
  ArbCodePtr code = new ArbCode(shader, unit, textures);
  code->generate();
  return code;
}

unsigned int arbTarget(const std::string& unit)
{
  if (unit == "vertex") return GL_VERTEX_PROGRAM_ARB;
  if (unit == "fragment") return GL_FRAGMENT_PROGRAM_ARB;  
  return 0;
}

}

