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
