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

    shSetBackend("arb");
    
    float data1[] = {0.7, 0.0, 1.0,
                    0.5, 0.5, 0.5,
                    0.6, 0.2, 0.1,
                    0.7, 0.2, 0.4,
                    0.0, 0.1, 0.0,
    };
    float data2[] = {-0.7, 0.0, 1.0,
                    -0.5, 0.5, 0.5,
                    -0.6, 0.2, 0.1,
                    -0.7, 0.2, 0.4,
                    -0.0, 0.1, 0.0,
    };
    
    const int elements = sizeof(data1)/sizeof(float*)/3;
    
    float out_data1[elements * 3];

    for (int i = 0; i < elements * 3; i++) {
      out_data1[i] = -1.0;
    }

    float out_data2[elements * 3];

    for (int i = 0; i < elements * 3; i++) {
      out_data2[i] = -1.0;
    }

    ShHostMemoryPtr host1 = new ShHostMemory(sizeof(data1), data1);
    ShHostMemoryPtr host2 = new ShHostMemory(sizeof(data2), data2);
    ShHostMemoryPtr host_out1 = new ShHostMemory(sizeof(out_data1), out_data1);
    ShHostMemoryPtr host_out2 = new ShHostMemory(sizeof(out_data2), out_data2);
    ShChannel<ShColor3f> stream1(host1, elements);
    ShChannel<ShColor3f> stream2(host2, elements);
    ShChannel<ShColor3f> output1(host_out1, elements);
    ShChannel<ShColor3f> output2(host_out2, elements);
    
    ShProgram shader = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputColor3f color_in[2];
      ShOutputColor3f color_out[2];

      color_out[0] = color_in[1] * 2.0f; // + color_in[1];
      color_out[1] = color_in[0];
    } SH_END;

    std::cout << "shader:" << std::endl;
    shader->ctrlGraph->print(std::cout, 0);

    //ShStream combined = combine(stream1, stream2);

    ShProgram final = shader << (stream1 & stream2);
    
    //ShProgram final = shader << stream1;// << ShConstant3f(1.0, 0.0, 0.0);// << stream2;

    std::cout << "final:" << std::endl;
    final->ctrlGraph->print(std::cout, 0);

    ShStream s = output1 & output2;

    (output1 & output2) = (shader << stream1 << stream2);
    std::cerr << "out_data1 = [";
    for (int i = 0; i < elements * 3; i++) {
      if (i % 3 == 0) {
        if (i > 0) std::cerr << "; ";
        std::cerr << "(";
      } else if (i > 0) {
        std::cerr << ", ";
      }
      std::cerr << out_data1[i];
      if (i % 3 == 2) std::cerr << ")";
    }
    std::cerr << "]" << std::endl;

    std::cerr << "out_data2 = [";
    for (int i = 0; i < elements * 3; i++) {
      if (i % 3 == 0) {
        if (i > 0) std::cerr << "; ";
        std::cerr << "(";
      } else if (i > 0) {
        std::cerr << ", ";
      }
      std::cerr << out_data2[i];
      if (i % 3 == 2) std::cerr << ")";
    }
    std::cerr << "]" << std::endl;

    (output1 & output2) = (shader << stream1 << stream2);


    std::cerr << "out_data1 = [";
    for (int i = 0; i < elements * 3; i++) {
      if (i % 3 == 0) {
        if (i > 0) std::cerr << "; ";
        std::cerr << "(";
      } else if (i > 0) {
        std::cerr << ", ";
      }
      std::cerr << out_data1[i];
      if (i % 3 == 2) std::cerr << ")";
    }
    std::cerr << "]" << std::endl;

    std::cerr << "out_data2 = [";
    for (int i = 0; i < elements * 3; i++) {
      if (i % 3 == 0) {
        if (i > 0) std::cerr << "; ";
        std::cerr << "(";
      } else if (i > 0) {
        std::cerr << ", ";
      }
      std::cerr << out_data2[i];
      if (i % 3 == 2) std::cerr << ")";
    }
    std::cerr << "]" << std::endl;
    
    return 0;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }
}
