#include <map>
#include <iostream>
#include <sh.hpp>
#include "ShRecStream.hpp"

using namespace SH;
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

  ShProgram prog = SH_BEGIN_PROGRAM("cc:stream") {
#endif

    ShInputAttrib3f SH_DECL(foo);
    foo(0).range(-2, -1);
    foo(1).range(1, 2);
    foo(2).range(3, 5);

    ShOutputAttrib3f SH_DECL(color);
    color = foo * foo;
  } SH_END;
  
  try {
    ShAaScanErrMap errMap = shAaScanLightnessErr(prog);
  } catch (const ShException &e) {
    std::cout << "Sh Exception: " << e.message() << std::endl;
  }

  return 0;
}

