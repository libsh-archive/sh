// -*- C++ -*-
#include <iostream>
#include <fstream>
#include "sh.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include <sys/time.h>
#include <time.h>

using namespace SH;

#define NAME(x) {x.name(#x);}


int main(int argc, char** argv)
{
  try {

    shSetBackend("arb");

#define COUNT 262144
#define REPS 50
    
    float data1[COUNT*3];
    float data2[COUNT*3];

    ShChannel<ShAttrib3f> c1(new ShHostMemory(sizeof(data1), data1), COUNT);
    ShChannel<ShAttrib3f> c2(new ShHostMemory(sizeof(data2), data2), COUNT);
    
    float out_data[COUNT*3];

    ShChannel<ShAttrib3f> out(new ShHostMemory(sizeof(out_data), out_data), COUNT);

    ShProgram kernel = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputAttrib3f in[2];
      ShOutputAttrib3f out;

      out = dot(in[0], in[1]) * (in[0] * in[1] + in[0]);
    } SH_END;

    ShProgram bound = kernel << c1 << c2;

    timeval tv1, tv2;
    
    gettimeofday(&tv1, 0);
    
    for (int i = 0; i < REPS; i++) {
      out = bound;
    }

    gettimeofday(&tv2, 0);

    double diff = (tv2.tv_sec - tv1.tv_sec)*1000.0
      + (tv2.tv_usec/1000 - tv1.tv_usec/1000);
    
    std::cout << "Count = " << COUNT << ", reps = " << REPS
              << ", time = " << diff << " ms" << std::endl;
    
    return 0;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }
}
