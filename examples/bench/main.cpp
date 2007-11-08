#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string> 
#include <sh/sh.hpp>
#include <shutil/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>

using namespace std;
using namespace SH;
using namespace ShUtil;

/* Generates programs with varying number of live temporaries and code size 
 * Tests how each one affects the performance on the GPU */ 

Program genProgram(int numTemps, int length) {
  Program result = SH_BEGIN_PROGRAM("stream") {
    InputAttrib1f input;
    OutputAttrib1f result;
    Attrib1f* temp = new Attrib1f[numTemps];

    /* make all of the temp variables live, and not possible to optimize out easily */ 
    for(int i = 0; i < numTemps; ++i) {
      ConstAttrib1f ci(i + 3.14159 * .25); 
      if(i == 0) temp[i] = mad(input, ci, ci); 
      else temp[i] = mad(input, ci, temp[i-1]);
    }

    /* add in enough mads to fill length, attempting to ensure minimal re-ordering is possible */
    for(int i = 0; i < length - 2 * numTemps; ++i) {
      int di = i % numTemps;  
      int si = (i + numTemps - 1) % numTemps; 
      temp[di] = mad(temp[di], 1.0001, temp[si]);
    }

    /* make sure all the temp variables are still live by gathering results */
    result = 0.0f;
    for(int i = 0; i < numTemps; ++i) {
      result = mad(result, 1.0001, temp[i]);
    }
    delete[] temp;
  } SH_END;
  //result.node()->dump("foo");
  return result;
}

int main(int argc, char** argv)
{
  int maxTemps = atoi(argv[1]);
  int maxLength = atoi(argv[2]);
  int streamWidth = 1024;
  int streamHeight = 1024;
  int streamSize = streamWidth * streamHeight; 

#if true
  Channel<Attrib1f> input(streamSize);
  Channel<Attrib1f> output(streamSize);
#else
  BaseTexture2D<Attrib1f> input(streamWidth, streamHeight, ArrayTraits());
  BaseTexture2D<Attrib1f> output(streamWidth, streamHeight, ArrayTraits());
#endif
  HostMemoryPtr inmem = new SH::HostMemory(streamSize * sizeof(float), SH_FLOAT); 
  HostMemoryPtr outmem = new SH::HostMemory(streamSize * sizeof(float), SH_FLOAT); 

  float* indata = reinterpret_cast<float*>(inmem->hostStorage()->data());
  srand48(0);
  for(int i = 0; i < streamSize; ++i) {
    indata[i] = drand48(); 
  }
#if true
  input.memory(inmem, streamSize);
  output.memory(outmem, streamSize);
#else
  input.memory(inmem);
  output.memory(outmem);
#endif

  Stream instream(input);
  Stream outstream(output);

  string backend = "glsl";
  setBackend(backend);
  int tempStep, lengthStep; 
  int unique_id = 0;
  //cerr << "unique_id,temps,instr_count,real_instr_count,render" << endl;
  cerr << "unique_id,temps,instr_count,real_instr_count,in_bind,out_bind,render" << endl;
  for(int i = 0; i < 4; ++i) {
    tempStep = 16;
    //tempStep = 4; /* high */
    tempStep = 1; /* low */
    for(int t = tempStep; t <= maxTemps; t += tempStep) {
      //if(t >= 64) tempStep = 64;
      //if(t >= 256) tempStep = 128;
      //lengthStep = 256; 
      lengthStep = 2048; /* high */
      //lengthStep = 256; /* low */
      for(int l = (3 * t) / lengthStep * lengthStep + lengthStep; l <= maxLength; l += lengthStep) {
        if(l >= 2048) lengthStep = 512; 
        if(l >= 4096) lengthStep = 2048; 
        Program foo = genProgram(t, l);
        float in_bind, out_bind, instr_count;
        in_bind = out_bind = instr_count = 0;
        for(int i = 0; i < 8; ++i) { 
          outstream = foo << instream;
          if(i == 0) {
            instr_count = SH::Context::current()->get_stat((std::string(backend) + "_instr_count").c_str());
            in_bind = SH::Context::current()->get_stat("fbo_binding");
            out_bind = SH::Context::current()->get_stat("fbo_texbind_output");
            continue;
          }
          float render = SH::Context::current()->get_stat("fbo_render");
          cerr << unique_id++ << "," << t << "," << l << "," << instr_count 
               << "," << in_bind << "," << out_bind 
               << "," << render << endl;
        }
      }
    }
  }
}

