#include <iostream>
#include "Arb.hpp"
#include "GlTextures.hpp"
#include "GlBackend.hpp"
#ifndef WIN32
#include "PBufferStreams.hpp"
#endif
#ifdef WIN32
#include <windows.h>
#endif

namespace shgl {

struct ArbBackend : public GlBackend {
  ArbBackend()
    : GlBackend(new ArbCodeStrategy(),
                new GlTextures(),
#ifdef WIN32
		0)
#else
                new PBufferStreams())
#endif
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
  std::cerr << "Arb Backend DllMain called!" << std::endl;
  std::cerr << "Have backend: " << backend << std::endl;
  std::cerr << "lpReserved = " << lpReserved << std::endl;
  std::cerr << "hModule = " << lpReserved << std::endl;
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    std::cerr << "Process attach!" << std::endl;
    if (backend) return TRUE;
    backend = new ArbBackend();
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
static ArbBackend* backend = new ArbBackend();
#endif
}
