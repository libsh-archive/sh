#include <iostream>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>

using namespace SH; 
using namespace ShUtil; 

extern "C" ShProgram shader()
{
  std::cout << "building blend" << std::endl;

  ShProgram phong = ShKernelSurface::phong<ShColor3f>();
  ShColor3f SH_DECL(kd1) = ShConstColor3f(0.5f, 0.5f, 1.0f);
  ShColor3f SH_DECL(kd2) = ShConstColor3f(1.0f, 0.5f, 0.5f);
  ShColor3f SH_DECL(ks) = ShConstColor3f(0.5f, 0.5f, 0.5f);
  ShAttrib1f SH_DECL(specExp) = ShConstAttrib1f(32);

  ShAttrib3f SH_DECL(alpha) = ShConstAttrib3f(0.5f, 0.5f, 0.5f);

  ShProgram phongCombo = namedCombine(
      renameOutput(phong(kd1, ks, specExp), "result", "a"), 
      renameOutput(phong(kd2, ks, specExp), "result", "b"));
  phongCombo.node()->dump("phongCombo");

  ShProgram blender = SH_BEGIN_PROGRAM("gpu:fragment") {
    ShInputColor3f SH_DECL(a);
    ShInputColor3f SH_DECL(b);

    ShOutputColor3f SH_DECL(result); 

    result = lerp(alpha, a, b); 
  } SH_END;
  blender = blender << phongCombo;

  blender.node()->dump("blender");

  std::cout << "done buildling blend" << std::endl;

  return blender; 
}

