#include "test.hpp"

int main(int argc, char** argv) {
  using namespace SH;

  try {
  Test test(argc, argv);

  { // f_3f_3f_infix
    ShAttrib<3, SH_CONST, float> a(0.0,1.0,2.0);
    ShAttrib<3, SH_CONST, float> b(3.0,4.0,5.0);
    ShAttrib<3, SH_TEMP, float> out;
    out = a + b;
    test.check("f_3f_3f_infix", out, ShAttrib<3, SH_CONST, float>(3.0, 5.0, 7.0));
  }

  { // f_3f_3f_destcall_shADD
    ShAttrib<3, SH_CONST, float> a(0.0,1.0,2.0);
    ShAttrib<3, SH_CONST, float> b(3.0,4.0,5.0);
    ShAttrib<3, SH_TEMP, float> out;
    shADD(out, a, b);
    test.check("f_3f_3f_destcall_shADD", out, ShAttrib<3, SH_CONST, float>(3.0, 5.0, 7.0));
  }

  { // f_1f_3f_infix
    ShAttrib<1, SH_CONST, float> a(1.0);
    ShAttrib<3, SH_CONST, float> b(3.0,4.0,5.0);
    ShAttrib<3, SH_TEMP, float> out;
    out = a + b;
    test.check("f_1f_3f_infix", out, ShAttrib<3, SH_CONST, float>(4.0, 5.0, 6.0));
  }

  { // f_1f_3f_destcall_shADD
    ShAttrib<1, SH_CONST, float> a(1.0);
    ShAttrib<3, SH_CONST, float> b(3.0,4.0,5.0);
    ShAttrib<3, SH_TEMP, float> out;
    shADD(out, a, b);
    test.check("f_1f_3f_destcall_shADD", out, ShAttrib<3, SH_CONST, float>(4.0, 5.0, 6.0));
  }

  { // f_3f_1f_infix
    ShAttrib<3, SH_CONST, float> a(1.0,2.0,3.0);
    ShAttrib<1, SH_CONST, float> b(7.0);
    ShAttrib<3, SH_TEMP, float> out;
    out = a + b;
    test.check("f_3f_1f_infix", out, ShAttrib<3, SH_CONST, float>(8.0, 9.0, 10.0));
  }

  { // f_3f_1f_destcall_shADD
    ShAttrib<3, SH_CONST, float> a(1.0,2.0,3.0);
    ShAttrib<1, SH_CONST, float> b(7.0);
    ShAttrib<3, SH_TEMP, float> out;
    shADD(out, a, b);
    test.check("f_3f_1f_destcall_shADD", out, ShAttrib<3, SH_CONST, float>(8.0, 9.0, 10.0));
  }

  { // f_1f_1f_infix
    ShAttrib<1, SH_CONST, float> a(1.0);
    ShAttrib<1, SH_CONST, float> b(7.0);
    ShAttrib<1, SH_TEMP, float> out;
    out = a + b;
    test.check("f_1f_1f_infix", out, ShAttrib<1, SH_CONST, float>(8.0));
  }

  { // f_1f_1f_destcall_shADD
    ShAttrib<1, SH_CONST, float> a(1.0);
    ShAttrib<1, SH_CONST, float> b(7.0);
    ShAttrib<1, SH_TEMP, float> out;
    shADD(out, a, b);
    test.check("f_1f_1f_destcall_shADD", out, ShAttrib<1, SH_CONST, float>(8.0));
  }

  { // f_3f_3f_infix
    ShAttrib<3, SH_CONST, float> a(4.0,5.0,6.0);
    ShAttrib<3, SH_CONST, float> b(-4.0,-5.0,-6.0);
    ShAttrib<3, SH_TEMP, float> out;
    out = a + b;
    test.check("f_3f_3f_infix", out, ShAttrib<3, SH_CONST, float>(0.0, 0.0, 0.0));
  }

  { // f_3f_3f_destcall_shADD
    ShAttrib<3, SH_CONST, float> a(4.0,5.0,6.0);
    ShAttrib<3, SH_CONST, float> b(-4.0,-5.0,-6.0);
    ShAttrib<3, SH_TEMP, float> out;
    shADD(out, a, b);
    test.check("f_3f_3f_destcall_shADD", out, ShAttrib<3, SH_CONST, float>(0.0, 0.0, 0.0));
  }

  { // i_3i_3i_infix
    ShAttrib<3, SH_CONST, int> a(0,1,2);
    ShAttrib<3, SH_CONST, int> b(3,4,5);
    ShAttrib<3, SH_TEMP, int> out;
    out = a + b;
    test.check("i_3i_3i_infix", out, ShAttrib<3, SH_CONST, int>(3, 5, 7));
  }

  { // i_3i_3i_destcall_shADD
    ShAttrib<3, SH_CONST, int> a(0,1,2);
    ShAttrib<3, SH_CONST, int> b(3,4,5);
    ShAttrib<3, SH_TEMP, int> out;
    shADD(out, a, b);
    test.check("i_3i_3i_destcall_shADD", out, ShAttrib<3, SH_CONST, int>(3, 5, 7));
  }

  { // s_1s_3s_infix
    ShAttrib<1, SH_CONST, short> a(1);
    ShAttrib<3, SH_CONST, short> b(3,4,5);
    ShAttrib<3, SH_TEMP, short> out;
    out = a + b;
    test.check("s_1s_3s_infix", out, ShAttrib<3, SH_CONST, short>(4, 5, 6));
  }

  { // s_1s_3s_destcall_shADD
    ShAttrib<1, SH_CONST, short> a(1);
    ShAttrib<3, SH_CONST, short> b(3,4,5);
    ShAttrib<3, SH_TEMP, short> out;
    shADD(out, a, b);
    test.check("s_1s_3s_destcall_shADD", out, ShAttrib<3, SH_CONST, short>(4, 5, 6));
  }

  { // i_3s_1b_infix
    ShAttrib<3, SH_CONST, short> a(1,2,3);
    ShAttrib<1, SH_CONST, char> b(7);
    ShAttrib<3, SH_TEMP, int> out;
    out = a + b;
    test.check("i_3s_1b_infix", out, ShAttrib<3, SH_CONST, int>(8, 9, 10));
  }

  { // i_3s_1b_destcall_shADD
    ShAttrib<3, SH_CONST, short> a(1,2,3);
    ShAttrib<1, SH_CONST, char> b(7);
    ShAttrib<3, SH_TEMP, int> out;
    shADD(out, a, b);
    test.check("i_3s_1b_destcall_shADD", out, ShAttrib<3, SH_CONST, int>(8, 9, 10));
  }

  { // i_1i_1i_infix
    ShAttrib<1, SH_CONST, int> a(1);
    ShAttrib<1, SH_CONST, int> b(7);
    ShAttrib<1, SH_TEMP, int> out;
    out = a + b;
    test.check("i_1i_1i_infix", out, ShAttrib<1, SH_CONST, int>(8));
  }

  { // i_1i_1i_destcall_shADD
    ShAttrib<1, SH_CONST, int> a(1);
    ShAttrib<1, SH_CONST, int> b(7);
    ShAttrib<1, SH_TEMP, int> out;
    shADD(out, a, b);
    test.check("i_1i_1i_destcall_shADD", out, ShAttrib<1, SH_CONST, int>(8));
  }

  { // i_3i_3i_infix
    ShAttrib<3, SH_CONST, int> a(4,5,6);
    ShAttrib<3, SH_CONST, int> b(-4,-5,-6);
    ShAttrib<3, SH_TEMP, int> out;
    out = a + b;
    test.check("i_3i_3i_infix", out, ShAttrib<3, SH_CONST, int>(0, 0, 0));
  }

  { // i_3i_3i_destcall_shADD
    ShAttrib<3, SH_CONST, int> a(4,5,6);
    ShAttrib<3, SH_CONST, int> b(-4,-5,-6);
    ShAttrib<3, SH_TEMP, int> out;
    shADD(out, a, b);
    test.check("i_3i_3i_destcall_shADD", out, ShAttrib<3, SH_CONST, int>(0, 0, 0));
  }

  { // fui_4f_4f_infix
    ShAttrib<4, SH_CONST, float> a(-4,0.3,1.0,2.0);
    ShAttrib<4, SH_CONST, float> b(-2,0.2,0.0,2.0);
    ShAttrib<4, SH_TEMP, ShFracUInt> out;
    out = a + b;
    test.check("fui_4f_4f_infix", out, ShAttrib<4, SH_CONST, ShFracUInt>(0, 0.5, 1, 1));
  }

  { // fui_4f_4f_destcall_shADD
    ShAttrib<4, SH_CONST, float> a(-4,0.3,1.0,2.0);
    ShAttrib<4, SH_CONST, float> b(-2,0.2,0.0,2.0);
    ShAttrib<4, SH_TEMP, ShFracUInt> out;
    shADD(out, a, b);
    test.check("fui_4f_4f_destcall_shADD", out, ShAttrib<4, SH_CONST, ShFracUInt>(0, 0.5, 1, 1));
  }

  { // i_3f_3f_infix
    ShAttrib<3, SH_CONST, float> a(1.3,2.97,-5.4);
    ShAttrib<3, SH_CONST, float> b(-0.31,-1.5,8);
    ShAttrib<3, SH_TEMP, int> out;
    out = a + b;
    test.check("i_3f_3f_infix", out, ShAttrib<3, SH_CONST, int>(0, 1, 2));
  }

  { // i_3f_3f_destcall_shADD
    ShAttrib<3, SH_CONST, float> a(1.3,2.97,-5.4);
    ShAttrib<3, SH_CONST, float> b(-0.31,-1.5,8);
    ShAttrib<3, SH_TEMP, int> out;
    shADD(out, a, b);
    test.check("i_3f_3f_destcall_shADD", out, ShAttrib<3, SH_CONST, int>(0, 1, 2));
  }

  } catch (const ShException &e) {
    std::cout << "Caught Sh Exception." << std::endl << e.message() << std::endl;
  } catch (const std::exception& e) {
      std::cerr << "Caught C++ exception: " << e.what() << std::endl;
  }
}