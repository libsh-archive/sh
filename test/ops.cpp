#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib4f in1; NAME(in1);
      ShInputAttrib4f in2; NAME(in2);
      ShOutputAttrib4f out; NAME(out);

      out = in1 + in2;
      out = in1 - in2;
      out = in1 * in2;
      out = in1 * in2(3);
      out = in1(0) * in2;
      out(0) = in1(0) * in2(1);
      out = in1 / in2;
      out = in1 / in2(3);
      out = in1 ^ in2(0);
      out = abs(in1);
      out = acos(in1);
      out = asin(in1);
      out = cos(in1);
      out(0) = dot(in1, in2);
      out = frac(in1);
      out = sin(in1);
      out = sqrt(in1);
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
