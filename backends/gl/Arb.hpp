#ifndef ARB_HPP
#define ARB_HPP

#include <string>
#include "GlBackend.hpp"
#include "ShBackend.hpp"
#include "ShProgram.hpp"

namespace shgl {

class ArbCodeStrategy : public CodeStrategy {
public:
  ArbCodeStrategy(int context = 0);
  
  SH::ShBackendCodePtr generate(const std::string& target,
                                const SH::ShProgram& shader,
                                TextureStrategy* textures);

  ArbCodeStrategy* create(int context);

private:
  int m_context;
};

unsigned int arbTarget(const std::string& shTarget);

}

#endif
