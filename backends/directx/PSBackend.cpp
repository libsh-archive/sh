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
#include <iostream>
#include "PS.hpp"
#include "DxBackend.hpp"
#include "DxTextures.hpp"
#ifndef WIN32
#include "PBufferStreams.hpp"
#endif
#ifdef WIN32
#include <windows.h>
#include <d3d9.h>
#endif

namespace shdx {



struct PSBackend : public DxBackend {
  PSBackend()
    : DxBackend(new PSCodeStrategy(),
                new DxTextures(),
#ifdef WIN32
		0)
#else
                new PBufferStreams())
#endif
  {
  }

  std::string name() const { return "ps"; }
};

#ifdef WIN32
static PSBackend* backend = 0;

extern "C"
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
  std::cerr << "PS Backend DllMain called!" << std::endl;
  std::cerr << "Have backend: " << backend << std::endl;
  std::cerr << "lpReserved = " << lpReserved << std::endl;
  std::cerr << "hModule = " << lpReserved << std::endl;
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    std::cerr << "Process attach!" << std::endl;
    if (backend) return TRUE;
    backend = new PSBackend();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    std::cerr << "Thread!" << std::endl;
    break;
  case DLL_PROCESS_DETACH:
    std::cerr << "Process detach!" << std::endl;
    delete backend;
    std::cerr << "Deleted backend!" << std::endl;
    break;
  default:
    std::cerr << "Some Other Thing!" << std::endl;
  }
  return TRUE;
}

#else
static PSBackend* backend = new PSBackend();
#endif
}
