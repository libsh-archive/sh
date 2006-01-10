// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

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
#ifdef __APPLE__
                0,
#else
                new PBufferStreams(),
#endif
                "glsl", "1.0")
  {
  }
};

#ifdef _WIN32
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
#endif // _WIN32

}

extern "C" {
  using namespace shgl;

#ifdef _WIN32
  __declspec(dllexport) 
#endif
  GlslBackend* shBackend_libshglsl_instantiate()
  {
    return new GlslBackend();
  }

#ifdef _WIN32
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

