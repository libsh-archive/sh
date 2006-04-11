// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "ArbLimits.hpp"
#include "Arb.hpp"

namespace shgl {

ArbLimits::ArbLimits(const std::string& target)
{
  unsigned int arb_target = arbTarget(target);
  
  m_instrs = (target == "gpu:vertex" ? 128 : 48);
  GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,
                                        &m_instrs));

  /* @todo implement proper detection of half-float limits. */
  m_halftemps = (target == "gpu:vertex" ? 0 : 64);

  /* TODO implement proper detection of NVIDIA.
   * NVIDIA's GlGetProgramivARB does not update m_temps, so set it to 32 here.
   * ATI will still have the right number because its drivers should set m_temps properly.
   */
  m_temps = (target == "gpu:vertex" ? 12 : 32);
  GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEMPORARIES_ARB,
                                        &m_temps));
      
  m_attribs = (target == "gpu:vertex" ? 16 : 10);
  GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_ATTRIBS_ARB,
                                        &m_attribs));
      
  m_params = (target == "gpu:vertex" ? 96 : 24);
  GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,
                                        &m_params));

  // Only get texture instructions for fragment targets
  m_texs = (target == "gpu:vertex" ? 0 : 24);
  if (target == "gpu:vertex") {
    GL_CHECK_ERROR(glGetProgramivARB(arb_target, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,
                                          &m_texs));
  }
}

}
