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
#ifndef GLSL_HPP
#define GLSL_HPP

#include <string>
#include "GlBackend.hpp"
#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShException.hpp"

namespace shgl {

class GlslCodeStrategy : public CodeStrategy {
public:
  GlslCodeStrategy(void);
  
  SH::ShBackendCodePtr generate(const std::string& target,
                                const SH::ShProgramNodeCPtr& shader,
                                TextureStrategy* textures);
  SH::ShBackendSetPtr generate_set(const SH::ShProgramSet& s);
  bool use_default_set() const;

  void unbind_all_programs();
  bool use_default_unbind_all() const;
  
  GlslCodeStrategy* create(void);
};

unsigned int glslTarget(const std::string& unit);

class GlslException : public SH::ShBackendException {
public:
  GlslException(const std::string& message);
};

enum GlslProgramType { SH_GLSL_FP, SH_GLSL_VP }; 

void print_infolog(GLhandleARB obj, std::ostream& out = std::cerr);
void print_shader_source(GLhandleARB shader, std::ostream& out = std::cerr);
std::string glsl_typename(SH::ShValueType type, int size);
}

#endif
