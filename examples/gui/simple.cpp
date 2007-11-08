#include <sh/sh.hpp>
#include <sh/shutil.hpp>

using namespace SH; 
using namespace ShUtil; 

extern "C" ShProgram shader()
{
  ShConstColor3f foo(3, 3, 3); 

  ShProgram fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
    ShInputPosition4f SH_DECL(posh);

    ShOutputColor3f SH_DECL(result) = foo; 
  } SH_END;

  return fsh; 
}

