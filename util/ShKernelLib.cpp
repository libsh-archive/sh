#include "ShKernelLib.hpp"

namespace ShUtil {

ShProgram ShKernelLib::bump() {
  ShProgram kernel = SH_BEGIN_PROGRAM() {
    ShInputAttrib2f gradient;
    ShOutputVector3f normal;
    normal(0) = 1.0;
    normal(1,2) = gradient;
  } SH_END_PROGRAM;
  return kernel;
}

};
