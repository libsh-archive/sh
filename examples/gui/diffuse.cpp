#include <sh/sh.hpp>
#include <sh/shutil.hpp>

using namespace SH; 
using namespace ShUtil; 

extern "C" ShProgram shader()
{
  ShProgram result = ShKernelSurface::diffuse<ShColor3f>();
  ShColor3f SH_DECL(kd) = ShConstColor3f(0.5f, 0.5f, 1.0f);
  return result(kd);
}

