#include "sh.hpp"
#include <iostream>

using namespace SH;

#define NAME(x) {x.name(#x);}

int main()
{
  try {
    ShMatrix4x4f mv;
    SH_BEGIN_SHADER(0) {
      ShInputAttrib4f in; NAME(in);
      ShOutputMatrix4x4f matrix;

      //matrix = mv | matrix;
      ShAttrib4f prod = mv | in; NAME(prod);
      //      matrix(0,1)(0,1) = mv(1,2)(1,2);
      //      matrix[0] = ShAttrib4f(1.0, 2.0, 3.0, 4.0);
      /*      matrix += mv;
      matrix -= mv;
      matrix *= mv;
      matrix /= mv;
      matrix *= ShAttrib1f(3.0);
      matrix /= ShAttrib1f(3.0); */

    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
  }
    
}
