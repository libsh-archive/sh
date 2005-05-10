#include <map>
#include <fstream>
#include <iostream>
#include <sh.hpp>
#include <ShAaOps.hpp>

using namespace SH;
using namespace std;

#define TEST(F) {\
  ShAffine<float> result = F;\
  std::cout << #F << " = " << result << " = " << (result).to_interval() << std::endl;\
}

int main() 
{
  shInit();
  shSetBackend("cc");

  ShAffine<float> x(1.0, 2.0);
  ShAffine<float> y(2.5, 1098.0);

   TEST(x);
   TEST(sqrt(x));
   TEST(exp(x));
   TEST(exp(-x));
   TEST(log(exp(x)));
   TEST(log(exp(-x)));
   TEST(rcp(x));
   TEST(rcp(-x));
   TEST(x * rcp(x));
   TEST(-x * rcp(-x));
   TEST((x - x));
   TEST((x + x));
   TEST((x + x) - (x + x));
   TEST(x % x);
   TEST(x % y);
   TEST(pos(x));
   TEST(pos(-x));
   TEST(min(x, y));
   TEST(max(x, y));

   std::cout << "YEAH" << std::endl;
   std::cout << "YEAH" << std::endl;
   TEST(y);
   TEST(sqrt(y));
   TEST(exp(y));
   TEST(exp(-y));
   TEST(log(exp(y)));
   TEST(log(exp(-y)));
   TEST(rcp(y));
   TEST(rcp(-y));
   TEST(y * rcp(y));
   TEST(-y * rcp(-y));
   TEST((y - y));
   TEST((y + y));
   TEST((y + y) - (y + y));

   ShAaVariable foo(new ShAaVariableNode("foo", SH_TEMP, SH_FLOAT, SH_ATTRIB, ShAaSyms(1, false))); 
   shASN(foo.center(), ShConstAttrib1f(-1.5f));
   shASN(foo.err(0), ShConstAttrib1f(0.5f));
   std::cout << "foo=" << foo << std::endl;
   std::cout << aaRCP(foo, ShAaSyms(1, false)) << std::endl;

  return 0;
}

