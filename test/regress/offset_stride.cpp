#include <sh/sh.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "test.hpp"

#define ELEMENTS 16

using namespace std;
using namespace SH;

void reset_memory(HostMemoryPtr mem)
{
  mem->hostStorage()->dirtyall();
  float* data = reinterpret_cast<float *>(mem->hostStorage()->data());
  for (int i = 0; i < ELEMENTS; ++i) {
    data[i] = i;
  }
}

void reset_memories(HostMemoryPtr mem[], int n)
{
  for (int i = 0; i < n; ++i) {
    reset_memory(mem[i]);
  }
}

int main(int argc, char* argv[])
{
  int errors = 0;
  int total_tests = 6;

  Test test(argc, argv);
  test.ignore_backend("host");

  Program prg = SH_BEGIN_PROGRAM("stream") {
    InputAttrib1f a;
    InputAttrib1f b;
    OutputAttrib1f c;
    c = a + b;
  } SH_END;

  HostMemoryPtr mem[3];
  for (int i = 0; i < 3; ++i) {
    mem[i] = new HostMemory(sizeof(float)*ELEMENTS, SH_FLOAT);
  }

  Array1D<Attrib1f> a(mem[0], ELEMENTS),
                        b(mem[1], ELEMENTS),
                        c(mem[2], ELEMENTS);
  vector<string> inputs;
  inputs.push_back("a"); inputs.push_back("b");
  
  a = count(a, 4);
  b = count(b, 4);
  c = count(c, 4);
  
  {float expected[] = {3,5,7,9, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  c = prg << a << offset(b, 3);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("one offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {3,7,11,15, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  c = prg << a << stride(offset(b, 3), 3);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("one stride offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}
  
  {float expected[] = {11,16,21,26, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  c = prg << stride(offset(a, 2), 3) << stride(offset(b, 9), 2);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("two stride offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {0,1,2,3, 4,5,0,2, 4,6,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  offset(c, 6) = prg << a << b;
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("destination offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {0,1,2,2, 4,5,4,7, 8,6,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  stride(c, 3) = prg << a << b;
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("destination stride", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {0,1,2,11, 4,5,16,7, 8,21,10,11, 26,13,14,15};
  reset_memories(mem, 3);
  stride(offset(c, 3), 3) = prg << stride(offset(a, 2), 3)
                                << stride(offset(b, 9), 2);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("everything", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}


  Array2D<Attrib1f> a2(mem[0], 4, 4), b2(mem[1], 4, 4), c2(mem[2], 4, 4);

  {float expected[] = {0,2,4,6, 8,10,12,14, 16,18,20,22, 24,26,28,30};
  reset_memories(mem, 3);
  c2 = prg << a2 << b2;
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("2D", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {9,11,2,3, 17,19,6,7, 25,27,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  count(c2, 2, 3) = prg << offset(count(a2, 2, 3), 1, 1)
                        << offset(count(b2, 2, 3), 0, 1);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("2D offset", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  {float expected[] = {5,8,2,3, 21,24,6,7, 8,9,10,11, 12,13,14,15};
  reset_memories(mem, 3);
  count(c2, 2, 2) = prg << stride(count(a2, 2, 2), 2, 2)
                        << offset(stride(count(b2, 2, 2), 1, 2), 1, 1);
  mem[2]->hostStorage()->sync();
  if (test.output_result<float*>("2D stride", inputs, 
                     reinterpret_cast<float*>(mem[2]->hostStorage()->data()),
                     expected, 16, 0.001)) errors++;}

  if (errors !=0) {
    std::cout << "Total Errors: " << errors << "/" << total_tests << std::endl;
    return 1;
  }
  return 0;
} 
