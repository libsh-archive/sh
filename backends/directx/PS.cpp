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

#include <d3d9.h>
#include "DxBackend.hpp"
#include "PS.hpp"
#include "PSCode.hpp"

namespace shdx {

using namespace SH;

PSCodeStrategy::PSCodeStrategy(int context)
  : m_context(context)
{
}

PSCodeStrategy* PSCodeStrategy::create(int context)
{
  return new PSCodeStrategy(context);
}

ShBackendCodePtr PSCodeStrategy::generate(const std::string& target,
                                           const ShProgramNodeCPtr& shader,
                                           TextureStrategy* textures)
{
  std::string::size_type loc = target.rfind(':');
  std::string unit = (loc == std::string::npos ? target : target.substr(loc + 1));
  PSCodePtr code = new PSCode(shader, unit, textures);
  code->setD3DDevice(m_pD3DDevice);
  code->generate();
  return code;
}

int psTarget(const std::string& unit, LPDIRECT3DDEVICE9 pD3DDevice, int& major, int& minor)
{
	// Query the device to get the shader version
	D3DCAPS9 caps;
	pD3DDevice->GetDeviceCaps(&caps);

	if (unit == "vertex")
	{
		if (caps.VertexShaderVersion == D3DVS_VERSION(3, 0))
		{
			major = 3;
			minor = 0;
			return SH_PSTARGET_VS;
		}
	}
    else if (unit == "fragment")
	{
		if (caps.PixelShaderVersion == D3DPS_VERSION(3, 0))
		{
			major = 3;
			minor = 0;
			return SH_PSTARGET_PS;
		}
	}

	// Not a 3.0 version shader
	major = 0;
	minor = 0;
	return SH_PSTARGET_UNKNOWN;
}

}

