#ifndef ARB_HPP
#define ARB_HPP

#include <string>
#include "GlBackend.hpp"
#include "ShBackend.hpp"
#include "ShProgram.hpp"

namespace shgl {

struct ArbCodeStrategy : public CodeStrategy {
  SH::ShBackendCodePtr generate(const std::string& target,
                                const SH::ShProgram& shader,
                                TextureStrategy* textures);
};

unsigned int arbTarget(const std::string& shTarget);

}

#endif
