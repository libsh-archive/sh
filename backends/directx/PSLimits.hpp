// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef PSLIMITS_HPP
#define PSLIMITS_HPP

#include <string>

namespace shdx {

struct PSLimits {
  PSLimits(const std::string& target, LPDIRECT3DDEVICE9 pD3DDevice);
  
  int instrs() const { return m_instrs; }
  int regs() const { return m_regs; }
  int consts() const { return m_consts; }
  int inputs() const { return m_inputs; }
  int outputs() const { return m_outputs; }
  int texs() const { return m_texs; }

  const std::string& target() const { return m_target; }
  
private:
  int m_instrs; ///< Maximum number of instructions for each shader target
  int m_regs; ///< Maximum number of registers for each shader target
  int m_consts; ///< Maximum number of constants for each shader target
  int m_inputs; ///< Maximum number of inputs for each shader target
  int m_outputs; ///< Maximum number of outputs for each shader target
  int m_texs; ///< Maximum number of TEX instructions for each shader target

  std::string m_target;
  int m_nTarget; // Target (SH_PSTARGET_VS or SH_PSTARGET_PS)
  int m_nMajor; // Major and minor versions of the highest-supported shader
  int m_nMinor;
};

}

#endif
