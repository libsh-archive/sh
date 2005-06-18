#include <iostream>
#include <sh/sh.hpp>

/* Stupid example of reduction
 */

template< typename T > 
void reduce( SH::ShProgram *kernel, SH::ShStream stream, const int length )
{
  /* bunch of tail-calls */
  /* check kernel:: (x,y) -> z */
  if( length == 1 )
    ; // return first component of stream
  // realign offsets of `stream', stride = 2, feed to
  // temp_out = kernel << stream_0 << stream_1;
  //   reduce( kernel, temp_out, length/2 );
  return;
}

const int STREAM_LENGTH = 100;

int main()
{
  using namespace SH;

  // allocate space
  ShHostMemoryPtr data = new ShHostMemory( sizeof(float) * STREAM_LENGTH, SH_FLOAT );
  ShHostMemoryPtr res = new ShHostMemory( sizeof(float) * STREAM_LENGTH, SH_FLOAT );
  ShChannel<ShAttrib1f> seq(data, STREAM_LENGTH);
  ShChannel<ShAttrib1f> seq2(res, STREAM_LENGTH);

  // find actual location of data
  ShHostStoragePtr data_loc = shref_dynamic_cast<ShHostStorage>( data->findStorage("host") );
  // sync barrier
  data_loc->dirty();
  // initialize stream
  float *cur_value = (float *)data_loc->data();
  for( int i = 0; i < STREAM_LENGTH; i++ )
    cur_value[i] = i;

  ShProgram mul2 = SH_BEGIN_PROGRAM("stream") {
    ShInputAttrib1f in;
    ShOutputAttrib1f out;

    out = 2 * in;

  } SH_END;

  ShProgram redkern = SH_BEGIN_PROGRAM("stream") {
    ShInputAttrib1f in1, in2;
    ShOutputAttrib1f out;

    out = in1 + in2;

  } SH_END;
  
  ShBackendPtr backend = ShBackend::get_backend(mul2.target());
  std::cerr << "Running the program on " << backend->name() << " backend." << std::endl;

  ShProgram update = mul2 << seq;
  seq2 = update;

  // stupid check
  ShHostMemoryPtr res_data = shref_dynamic_cast< ShHostMemory >( seq2.memory() );
  if( res_data ) {
    float *cur_value2 = (float *)(res_data->hostStorage()->data());
    for( int i = 0; i < STREAM_LENGTH; i++ )
      std::cerr << "at position " << i << "there is a " << cur_value2[i] << std::endl;
  } else
    std::cerr << "Couldn't retrieve data" << std::endl;

  return 0;
}

 
/*
  Local Variables:
  compile-command: "g++ -I/tmp/rgesteve/sh-install/include -L/tmp/rgesteve/sh-install/lib  main.cpp -Wall -g -O2  -o test -lGL -lsh"
  End:
*/
