#include <sh/sh.hpp>
#include <sh/shutil.hpp>

using namespace SH; 
using namespace ShUtil; 

extern "C" ShProgram shader()
{
  ShProgram result = ShKernelSurface::phong<ShColor3f>();
  ShColor3f SH_DECL(kd) = ShConstColor3f(0.5f, 0.5f, 1.0f);
  ShColor3f SH_DECL(ks) = ShConstColor3f(0.5f, 0.5f, 0.5f);
  ShAttrib1f SH_DECL(specExp) = ShConstAttrib1f(32);
  return result(kd, ks, specExp);
}

