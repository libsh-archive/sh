#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    ShTexture2D<ShAttrib3f> texture(320, 200);
    
    SH_BEGIN_SHADER(0) {
      ShOutputAttrib3f value;

      value = texture(ShAttrib2f(0.5, 0.3));
      
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
