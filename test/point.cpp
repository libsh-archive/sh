#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    ShPoint4f p1; NAME(p1);
    ShMatrix4x4f mvw;
    
    SH_BEGIN_SHADER(0) {
      ShInputPoint1f in1; NAME(in1);
      ShInputPoint1f in2(3.0); NAME(in2);
      ShInputPoint4f a; NAME(a);
      ShInputPoint4f b; NAME(b);
      ShInputPoint3f p; NAME(p);

      ShOutputVector4f out; NAME(out);

      //      out(0) = (a - b) | (b - a);
      out(0,1,2) = mvw | p;

    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
