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
#ifndef ARBLIMITS_HPP
#define ARBLIMITS_HPP

#include "GlBackend.hpp"
#include <string>

namespace shgl {

struct ArbLimits {
  ArbLimits(const std::string& target);
  
  int instrs() const { return m_instrs; }
  int halftemps() const { return m_halftemps; }
  int temps() const { return m_temps; }
  int attribs() const { return m_attribs; }
  int params() const { return m_params; }
  int texs() const { return m_texs; }

  const std::string& target() const { return m_target; }
  
private:
  GLint m_instrs; ///< Maximum number of instructions for each shader target
  int m_halftemps; ///< Maximum number of half-float temporaries for each shader target
  GLint m_temps; ///< Maximum number of temporaries for each shader target
  GLint m_attribs; ///<Maximum number of attributes for each shader target
  GLint m_params; ///< Maximum number of parameters for each shader target
  GLint m_texs; ///< Maximum number of TEX instructions for each shader target

  std::string m_target;
};

}

#endif
