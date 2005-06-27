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
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <iostream>

#include "Glsl.hpp"
#include "GlTextures.hpp"
#include "GlBackend.hpp"
#include "PBufferStreams.hpp"

namespace shgl {

struct GlslBackend : public GlBackend {
  GlslBackend()
    : GlBackend(new GlslCodeStrategy(),
                new GlTextures(),
                new PBufferStreams())
  {
  }

  std::string name() const { return "glsl"; }
  std::string version() const { return "1.0"; }
};

#ifdef WIN32
static GlslBackend* backend = 0;

extern "C"
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    if (backend) return TRUE;
    backend = new GlslBackend();
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
#endif // WIN32

}

extern "C" {
  using namespace shgl;

#ifdef WIN32
  __declspec(dllexport) 
#endif
  GlslBackend* shBackend_libshglsl_instantiate()
  {
    return new GlslBackend();
  }

#ifdef WIN32
  __declspec(dllexport) 
#endif
  int shBackend_libshglsl_target_cost(const std::string& target)
  {
    if ("glsl:vertex" == target)   return 1;
    if ("glsl:fragment" == target) return 1;
#ifndef __APPLE__
    if ("glsl:stream" == target)   return 1;
#else
    if ("glsl:stream" == target)   return 0;
#endif
    
    if (("gpu:vertex" == target) || ("gpu:fragment" == target)) return 5;
    if (("vertex" == target) || ("fragment" == target))         return 5;
    if (("*:vertex" == target) || ("*:fragment" == target))     return 5;

#ifndef __APPLE__
    if ("gpu:stream" == target) return 10;
    if ("*:stream" == target)   return 10;
    if ("stream" == target)     return 10;
#else
    if ("gpu:stream" == target) return 0;
    if ("*:stream" == target)   return 0;
    if ("stream" == target)     return 0;
#endif

    return 0;
  }
}

