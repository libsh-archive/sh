#ifndef _LIMITS_HPP
#define _LIMITS_HPP

#include <string>
namespace rds {

//copy of ArbLimits
class Limits {
public:
  int instrs() const { return m_instrs; }
  int halftemps() const { return m_halftemps; }
  int temps() const { return m_temps; }
  int attribs() const { return m_attribs; }
  int params() const { return m_params; }
  int texs() const { return m_texs; }

  void dump_limits();

protected:
  int m_instrs; ///< Maximum number of instructions for each shader target
  int m_halftemps; ///< Maximum number of half-float temporaries for each shader target
  int m_temps; ///< Maximum number of temporaries for each shader target
  int m_attribs; ///<Maximum number of attributes for each shader target
  int m_params; ///< Maximum number of parameters for each shader target
  int m_texs; ///< Maximum number of TEX instructions for each shader target
};

class ArbLimits : public Limits {
public:
  
  ArbLimits(const std::string&);
  const std::string& target() const { return m_target; }

protected:
  
  std::string m_target;
};

class FakeLimits : public Limits {
public:

  FakeLimits();

};

}

#endif
