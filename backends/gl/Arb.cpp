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
#include "GlBackend.hpp"
#include "Arb.hpp"
#include "ArbCode.hpp"

namespace shgl {

using namespace SH;

ArbCodeStrategy::ArbCodeStrategy(void)
{
}

ArbCodeStrategy* ArbCodeStrategy::create(void)
{
  return new ArbCodeStrategy;
}

ShBackendCodePtr ArbCodeStrategy::generate(const std::string& target,
                                           const ShProgramNodeCPtr& shader,
                                           TextureStrategy* texture)
{
  std::string::size_type loc = target.rfind(':');
  std::string unit = (loc == std::string::npos ? target : target.substr(loc + 1));
  ArbCodePtr code = new ArbCode(shader, unit, texture);
  code->generate();
  return code;
}

unsigned int arbTarget(const std::string& unit)
{
  if (unit == "vertex") return GL_VERTEX_PROGRAM_ARB;
  if (unit == "fragment") return GL_FRAGMENT_PROGRAM_ARB;  
  return 0;
}

ArbException::ArbException(const std::string& message)
  : ShBackendException(std::string("ARB error: ") + message)
{
}

}

