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
#include "PSLimits.hpp"
#include "DxBackend.hpp"
#include "PS.hpp"

namespace shdx {

PSLimits::PSLimits(const std::string& target, LPDIRECT3DDEVICE9 pD3DDevice) : m_target(target)
{
  m_nTarget = psTarget(target, pD3DDevice, m_nMajor, m_nMinor);

  // TODO: Actually query the card for all of these values

  // Query the device to get the shader version
  D3DCAPS9 caps;
  pD3DDevice->GetDeviceCaps(&caps);

  // Since we're only targetting the 3.0 shaders...
  m_nMajor = 3;
  m_nMinor = 3;

  if (m_nTarget == SH_PSTARGET_VS)
  {
	  m_instrs = caps.MaxVShaderInstructionsExecuted;
	  if (m_nMajor == 3)
		  m_instrs = caps.MaxVertexShader30InstructionSlots;

      m_consts = caps.MaxVertexShaderConst;

	  // Parameters for all versions of vertex shaders
	  m_regs = 12;
	  m_inputs = 16;
	  m_outputs = 13;

	  if (m_nMajor == 3)
		  m_outputs = 12;

	  m_texs = 0; // Vertex shaders have no tex instruction
  }
  else if (m_nTarget == SH_PSTARGET_PS)
  {
	  m_instrs = caps.MaxPShaderInstructionsExecuted;
	  if (m_nMajor == 3)
		  m_instrs = caps.MaxPixelShader30InstructionSlots;

      // ps 1.1-1.3
	  m_consts = 8;
	  m_regs = 2;
	  m_inputs = 2;
	  m_texs = 4;
	  m_outputs = 1;

	  // ps 1.4
	  if (m_nMajor == 1 && m_nMinor == 4)
	  {
		  m_regs = 6;
		  m_texs = 6;
	  }

	  // ps 2.0
	  if (m_nMajor == 2)
	  {
		  m_consts = 32;
		  m_regs = 12;
		  m_texs = 8;
		  m_outputs = 5;
	  }

	  // ps 3.0
	  if (m_nMajor == 3)
	  {
		  m_consts = 224;
		  m_inputs = 10;
		  m_regs = 32;
		  m_outputs = 5;
		  m_texs = m_instrs + 1; // No limit on tex operations
	  }
  }
  else // SH_PSTARGET_UNKNOWN
  {
    m_instrs = 0;
	m_regs = 0;
	m_consts = 0;
	m_inputs = 0;
	m_outputs = 0;
	m_texs = 0;
  }
}

}
