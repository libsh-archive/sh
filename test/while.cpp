#include <iostream>
#include "sh.hpp"

using namespace SH;

#define NAME(x) {x.name(#x);}

ShAttrib1f f()
{
  ShAttrib1f ret(1.0); NAME(ret);
  ShAttrib2f x(2.0, 3.0); NAME(x);

  SH_WHILE(x(0) + x(1)) {
    ret += ret;
  } SH_ENDWHILE;
  return ret;
}
int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib1f in; NAME(in);
      ShAttrib1f temp; NAME(temp);
      ShOutputAttrib1f out; NAME(out);

      SH_WHILE(f()) {
        out = temp + temp;
      } SH_ENDWHILE;
      temp = in + in;
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << e.message() << std::endl;
  } catch (...) {
  }
}
