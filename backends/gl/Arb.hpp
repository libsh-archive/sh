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
#ifndef SHARB_HPP
#define SHARB_HPP

#include <string>
#include "GlBackend.hpp"
#include "Backend.hpp"
#include "Program.hpp"
#include "Exception.hpp"

namespace shgl {

class ArbCodeStrategy : public CodeStrategy {
public:
  ArbCodeStrategy(void);
  
  SH::BackendCodePtr generate(const std::string& target,
                                const SH::ProgramNodeCPtr& shader,
                                TextureStrategy* textures);

  ArbCodeStrategy* create(void);
};

unsigned int arbTarget(const std::string& unit);

class ArbException : public SH::BackendException {
public:
  ArbException(const std::string& message);
};

}

#endif
