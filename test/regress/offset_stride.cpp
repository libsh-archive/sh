#include <sh/sh.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "test.hpp"

#define ELEMENTS 16

using namespace std;
using namespace SH;

void reset_memory(ShHostMemoryPtr mem)
{
  mem->hostStorage()->dirtyall();
  float* data = reinterpret_cast<float *>(mem->hostStorage()->data());
  for (int i = 0; i < ELEMENTS; ++i) {
    data[i] = i;
  }
}

void reset_memories(ShHostMemoryPtr mem[], int n)
{
  for (int i = 0; i < n; ++i) {
    reset_memory(mem[i]);
  }
}

int main(int argc, char* argv[])
{
  Test test(argc, argv);

  ShProgram prg = SH_BEGIN_PROGRAM("stream") {
    ShInputAttrib1f a;
    ShInputAttrib1f b;
    ShOutputAttrib1f c;
    c = a + b;
  } SH_END;

  ShHostMemoryPtr mem[3];
  for (int i = 0; i < 3; ++i) {
    mem[i] = new ShHostMemory(sizeof(float)*ELEMENTS, SH_FLOAT);
  }
  
  ShChannel<ShAttrib1f> a(mem[0], ELEMENTS),
                        b(mem[1], ELEMENTS),
                        c(mem[2], ELEMENTS);
  vector<string> inputs;
  inputs.push_back("a"); inputs.push_back("b");
  prg = prg << a << b;
  
  a.count(4);
  b.count(4);
  c.count(4);
  
  
  {float expected[] = {3,5,7,9, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  b.offset(3);
  c = prg;
  mem[2]->hostStorage()->sync();
  test.output_result<float*>("one offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001);}
                     
  {float expected[] = {3,7,11,15, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  b.offset(3);
  b.stride(3);
  c = prg;
  mem[2]->hostStorage()->sync();
  test.output_result<float*>("one stride offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001);}
  
  {float expected[] = {11,16,21,26, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  a.offset(2);
  a.stride(3);
  b.offset(9);
  b.stride(2);
  c = prg;
  mem[2]->hostStorage()->sync();
  test.output_result<float*>("two stride offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001);}
                     
} 
