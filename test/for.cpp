#include "sh.hpp"

using namespace SH;

#define NAME(x) {x.name(#x);}

ShAttrib1f f()
{
  ShAttrib1f ret(1.0); NAME(ret);
  ShAttrib2f x(2.0, 3.0); NAME(x);

  SH_FOR(x(0) = x(1), x(0), x(0) += x(0)) {
    ret = x(0) + x(1);
  } SH_ENDFOR;
  return ret;
}
int main()
{
  try {
    SH_BEGIN_SHADER(0) {
      ShInputAttrib1f in; NAME(in);
      ShAttrib1f temp; NAME(temp);
      ShOutputAttrib1f out; NAME(out);

      SH_FOR(temp = in*in, temp, temp -= in) {
        out = temp + temp;
        /*        SH_IF(in + in) {
          out = temp + in;
          } SH_ENDIF; */
      } SH_ENDFOR;
      temp = in + in;
    } SH_END_SHADER;
  } catch (const ShException& e) {
  } catch (...) {
  }
}
