#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib4f in; NAME(in);
      ShOutputAttrib4f out; NAME(out);
      
      ShAttrib4f temp; NAME(temp);
      temp = in * in;
      out = temp;
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
