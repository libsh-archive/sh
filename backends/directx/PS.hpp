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
#ifndef PS_HPP
#define PS_HPP

#include <d3d9.h>
#include <string>
#include "DxBackend.hpp"
#include "ShBackend.hpp"
#include "ShProgram.hpp"

namespace shdx {

class PSCodeStrategy : public CodeStrategy {
public:
  PSCodeStrategy(int context = 0);
  
  SH::ShBackendCodePtr generate(const std::string& target,
                                const SH::ShProgramNodeCPtr& shader,
                                TextureStrategy* textures);

  PSCodeStrategy* create(int context);
  
private:
  int m_context;

};

#define SH_PSTARGET_UNKNOWN 0
#define SH_PSTARGET_VS 1
#define SH_PSTARGET_PS 2

int psTarget(const std::string& unit, LPDIRECT3DDEVICE9 pD3DDevice, int& major, int& minor);

}

#endif
