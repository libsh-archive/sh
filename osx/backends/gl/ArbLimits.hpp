// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef ARBLIMITS_HPP
#define ARBLIMITS_HPP

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
  int m_instrs; ///< Maximum number of instructions for each shader target
  int m_halftemps; ///< Maximum number of half-float temporaries for each shader target
  int m_temps; ///< Maximum number of temporaries for each shader target
  int m_attribs; ///<Maximum number of attributes for each shader target
  int m_params; ///< Maximum number of parameters for each shader target
  int m_texs; ///< Maximum number of TEX instructions for each shader target

  std::string m_target;
};

}

#endif
