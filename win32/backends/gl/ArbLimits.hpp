#ifndef ARBLIMITS_HPP
#define ARBLIMITS_HPP

#include <string>

namespace shgl {

struct ArbLimits {
  ArbLimits(const std::string& target);
  
  int instrs() const { return m_instrs; }
  int temps() const { return m_temps; }
  int attribs() const { return m_attribs; }
  int params() const { return m_params; }
  int texs() const { return m_texs; }

  const std::string& target() const { return m_target; }
  
private:
  int m_instrs; ///< Maximum number of instructions for each shader target
  int m_temps; ///< Maximum number of temporaries for each shader target
  int m_attribs; ///<Maximum number of attributes for each shader target
  int m_params; ///< Maximum number of parameters for each shader target
  int m_texs; ///< Maximum number of TEX instructions for each shader target

  std::string m_target;
};

}

#endif
