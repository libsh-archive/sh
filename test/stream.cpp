// -*- C++ -*-
#include <iostream>
#include <fstream>
#include "sh.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"

using namespace SH;

#define NAME(x) {x.name(#x);}

int main(int argc, char** argv)
{
  try {
    double data[] = {1.0, 0.0, 1.0,
                     0.5, 0.5, 0.5};

    ShChannel<ShColor3f> stream1(data, 2);
    ShChannel<ShColor3f> stream2(data, 2);
    
    ShProgram shader = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputColor3f colors[2];
      ShOutputColor3f color_out;

      ShColor3f c = stream1();
      
      color_out = colors[0] + 2.0 * colors[1] + c;
    } SH_END_PROGRAM;

    std::cout << "shader:" << std::endl;
    shader->ctrlGraph->print(std::cout, 0);

    ShStream combined = combine(stream2, stream1);
    ShProgram final = shader << combined; // combine(stream1, stream2);
    //ShProgram final = shader << stream1;// << ShConstant3f(1.0, 0.0, 0.0);// << stream2;

    std::cout << "final:" << std::endl;
    final->ctrlGraph->print(std::cout, 0);

    return 0;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }
}
