#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    ShAttrib4f uniform(2.0, 3.0, 4.0, 5.0); NAME(uniform);
    ShAttrib4f computed; NAME(computed);

    computed(3) = uniform(1) + uniform(1) - uniform(0);
    
    SH_BEGIN_SHADER(0) {
      ShInputAttrib4f in1; NAME(in1);
      ShInputAttrib4f in2; NAME(in2);
      ShOutputAttrib4f out; NAME(out);

      out = computed + uniform;
      
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
