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
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <iostream>

#include "Arb.hpp"
#include "GlTextures.hpp"
#include "GlBackend.hpp"

#include "PBufferStreams.hpp"

namespace shgl {

struct ArbBackend : public GlBackend {
  ArbBackend()
    : GlBackend(new ArbCodeStrategy(),
                new GlTextures(),
                new PBufferStreams())
  {
  }

  std::string name() const { return "arb"; }
};

#ifdef WIN32
static ArbBackend* backend = 0;

extern "C"
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    if (backend) return TRUE;
    backend = new ArbBackend();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    delete backend;
    break;
  default:
    break;
  }
  return TRUE;
}

#else
static ArbBackend* backend = new ArbBackend();
#endif /* WIN32 */
}
