#include <iostream>
#include "sh.hpp"

using namespace SH;

#define NAME(x) {x.name(#x);}

ShAttrib1f f()
{
  ShAttrib1f ret(1.0); NAME(ret);
  ShAttrib2f x(2.0, 3.0); NAME(x);

  SH_DO {
    ret += ret;
  } SH_UNTIL(x(0) + x(1));
  return ret;
}
int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib1f in; NAME(in);
      ShAttrib1f temp; NAME(temp);
      ShOutputAttrib1f out; NAME(out);

      SH_DO {
        out = temp + temp;
      } SH_UNTIL(out);
      temp = in + in;
    } SH_END_SHADER;
  } catch (const ShException& e) {
    std::cerr << e.message() << std::endl;
  } catch (...) {
  }
}
