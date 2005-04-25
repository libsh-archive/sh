// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ArbLimits.hpp"
#include "GlBackend.hpp"
#include "Arb.hpp"

namespace shgl {

ArbLimits::ArbLimits(const std::string& target)
{
  unsigned int arb_target = arbTarget(target);
  
  m_instrs = (target == "gpu:vertex" ? 128 : 48);
  SH_GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,
                                        &m_instrs));

  /* @todo implement proper detection of half-float limits. */
  m_halftemps = (target == "gpu:vertex" ? 0 : 64);

  /* TODO implement proper detection of NVIDIA.
   * NVIDIA's GlGetProgramivARB does not update m_temps, so set it to 32 here.
   * ATI will still have the right number because its drivers should set m_temps properly.
   */
  m_temps = (target == "gpu:vertex" ? 12 : 32);
  SH_GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEMPORARIES_ARB,
                                        &m_temps));
      
  m_attribs = (target == "gpu:vertex" ? 16 : 10);
  SH_GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_ATTRIBS_ARB,
                                        &m_attribs));
      
  m_params = (target == "gpu:vertex" ? 96 : 24);
  SH_GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,
                                        &m_params));

  // Only get texture instructions for fragment targets
  m_texs = (target == "gpu:vertex" ? 0 : 24);
  if (target == "gpu:vertex") {
    SH_GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,
                                          &m_texs));
  }
}

}
