#include "sh.hpp"

using namespace SH;

#define NAME(x) {x.name(#x);}

ShAttrib1f f()
{
  ShAttrib1f ret; NAME(ret);
  ShAttrib2f x(2.0, 3.0); NAME(x);

  ret(0) = x(1);
  
  SH_IF(x(0)) {
    ret = x(0) + x(1);
  } SH_ELSE {
    ret = x(1) * x(1);
  } SH_ENDIF;
  return ret;
}
int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib1f in; NAME(in);
      ShAttrib1f temp; NAME(temp);
      ShOutputAttrib1f out; NAME(out);

      temp = in * in;
      SH_IF(f()) {
        out = temp + temp;
        SH_IF(in + in) {
          out = temp + in;
        } SH_ENDIF;
      } SH_ELSE {
        out = ShAttrib1f(0.0);
      } SH_ENDIF;
      temp = in + in;
    } SH_END_SHADER;
  } catch (const ShException& e) {
  } catch (...) {
  }
}
