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
#ifdef WIN32
#include <windows.h>
#endif
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

