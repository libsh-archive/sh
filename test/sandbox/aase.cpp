#include <map>
#include <iostream>
#include <sh.hpp>
#include <shutil.hpp>
#include "ShRecStream.hpp"

using namespace SH;
using namespace ShUtil;
using namespace std;


int main() 
{
  shInit();
#if 0
  // long-tuple handling on outputs is not ready (and will take some major work
  // since long-tuple streams will need to work)
  
  shSetBackend("arb");

  ShProgram prog = SH_BEGIN_PROGRAM("gpu:stream") {
#else
  shSetBackend("cc");

  ShConstColor3f SH_DECL(kd) = ShConstAttrib3f(0.5f, 1.0f, 0.5f);
  ShConstColor3f SH_DECL(irrad) = ShConstAttrib3f(1.0f, 1.0f, 1.0f);

  ShProgram prog = SH_BEGIN_PROGRAM("cc:stream") {
#endif
    ShInputVector3f SH_DECL(normal);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);
    normal.range(-1.0, 1.0);
    lightVec.range(-1.0, 1.0);
    posh.range(-1.0, 1.0);

    ShProgram diffProg = ShKernelSurface::diffuse<ShColor3f>();
    ShColor3f SH_DECL(diffuse);
    SH_BEGIN_SECTION("diffuse") {
      diffuse = diffProg(kd & irrad & normal & lightVec & posh);
    } SH_END_SECTION;
    
    ShOutputColor3f SH_DECL(color) = diffuse;
  } SH_END;
  
  try {
    ShAaScanErrMap errMap = shAaScanLightnessErr(prog);
    shAaScanErrDump("aase_result", errMap, prog);
  } catch (const ShException &e) {
    std::cout << "Sh Exception: " << e.message() << std::endl;
  }

  return 0;
}

