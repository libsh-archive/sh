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
                    
    ShStream<ShColor3f> stream(data, 2);
    
    ShProgram shader = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputColor3f color_in;
      ShOutputColor3f color_out;
      
      color_out = color_in * 3.0;
    } SH_END_PROGRAM;

    std::cout << "shader:" << std::endl;
    shader->ctrlGraph->print(std::cout, 0);

    ShProgram final = connect(stream, shader);
    
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
