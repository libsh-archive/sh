#include <map>
#include <iostream>
#include <sh.hpp>
#include "ShRecStream.hpp"

using namespace SH;
using namespace std;


int main() 
{
  shInit();
#if 1
  shSetBackend("arb");

  ShProgram prog = SH_BEGIN_PROGRAM("gpu:stream") {
#else
  shSetBackend("arb");

  ShProgram prog = SH_BEGIN_PROGRAM("cc:stream") {
#endif

    ShInputAttrib3f SH_DECL(foo);
    foo(0).range(-2, -1);
    foo(1).range(1, 2);
    foo(2).range(3, 5);


    ShOutputAttrib3f SH_DECL(foo_out) = foo; 
    ShOutputAttrib1f SH_DECL(foo_0_0) = foo(0) * foo(0);
    ShOutputAttrib1f SH_DECL(foo_1_1) = foo(1) * foo(1);
    ShOutputAttrib1f SH_DECL(foo_2_2) = foo(2) * foo(2);
    ShOutputAttrib1f SH_DECL(foo_dot_foo) = foo | foo; 
    ShOutputAttrib3f SH_DECL(foo_plus_foo) = foo + foo; 
    ShOutputAttrib3f SH_DECL(foo_minus_foo) = foo - foo; 
    ShOutputAttrib3f SH_DECL(foo_foo) = foo * foo; 
    ShConstAttrib1f A(2.0f);
    ShConstAttrib1f B(1.0f);
    ShConstAttrib1f C(-1.0f);
    ShOutputAttrib3f SH_DECL(foo_mad) = mad(foo, mad(foo, A(0,0,0), B(0,0,0)), C(0,0,0));
    ShOutputAttrib3f SH_DECL(exp_foo) = exp(foo);
    ShOutputAttrib3f SH_DECL(log_foo) = log(foo);
    ShOutputAttrib3f SH_DECL(rcp_foo) = rcp(foo);
    ShOutputAttrib3f SH_DECL(sqrt_foo) = sqrt(foo);
    ShOutputAttrib3f SH_DECL(hier1_foo);
    ShOutputAttrib3f SH_DECL(hier2_foo);

    SH_BEGIN_SECTION("inside") {
      ShAttrib3f SH_DECL(i2result);
      SH_BEGIN_SECTION("inside2") {
        hier2_foo = i2result = exp(foo) + exp(foo); // should add 2 error symbols
      } SH_END_SECTION;
      hier1_foo = i2result + exp(foo); 
    } SH_END_SECTION;
  } SH_END;
  
  try {
    ShAaScanCore aas(prog);

    ShStream input; // null ==> all default bounds

    ShAaScanCore::AaVarVec resultPattern;
    ShRecord resultRecord; 
    ShStream result;

    result = aas.doit(0, input, resultPattern, resultRecord);

    // fetch one into resultRecord/resultPattern 
    lookup(resultRecord, result, 0);

    std::cout << "results: " << std::endl;
    for(int i = 0; i < resultPattern.size(); ++i) {
      std::cout << i << ": " << resultPattern[i].name() << "=" << resultPattern[i] << std::endl; 
    }
  } catch (const ShException &e) {
    std::cout << "Sh Exception: " << e.message();
  }

  return 0;
}

