#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    ShVector4f v1; NAME(v1);
    ShMatrix4x4f mvw;

    SH_BEGIN_SHADER(0) {
      ShInputVector1f in1; NAME(in1);
      ShInputVector1f in2(3.0); NAME(in2);
      ShInputVector4f a; NAME(a);
      ShInputVector4f b; NAME(b);

      ShInputVector3f t; NAME(t);

      ShOutputVector4f out; NAME(out);

      out(0,1,2) = mvw | t;
      
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
