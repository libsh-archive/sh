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
static ArbBackend* backend;

extern "C"
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    backend = new ArbBackend();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    delete backend;
    break;
  }
  return TRUE;
}

#else
static ArbBackend* backend = new ArbBackend();
#endif
}
