#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  shInit();
  shSetBackend("cc");
  ShVector3f a(10, 10, 10);
  ShVector3f b(8, 9, 10);
  ShConstPoint4f c(19, 20, 21, 22);

  for(int i = 0; i < 5000; ++i) {
    ShProgram test = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputVector3f SH_DECL(in);
      ShOutputVector3f SH_DECL(out);
      out = in * (a + b * c(2,1,0));
    } SH_END;
  }
  return 0;
}

