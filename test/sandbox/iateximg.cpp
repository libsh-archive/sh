#include <iostream>
#include <sstream>
#include <sh.hpp>
#include "ShLp.hpp"
#include "ShIntervalTexture.hpp"

using namespace SH;
using namespace std;


int main(int argc, char* argv[]) 
{
  shInit();
  shSetBackend("cc");

  ShImage img;
  if(argc < 2) {
    std::cout << "You dolt. provide a filename!";
    return 0;
  }
  img.loadPng(argv[1]);

  int width = img.width();
  int height = img.height();
  SH_DEBUG_ASSERT(width == height);
#if 1
  ShTexture2D<ShColor3f> tex(width, height);
  tex.memory(img.memory());
#else
  ShTexture1D<ShColor3f> tex(width);
  height = 1;
  tex.memory(img.memory()); // test only the first row
#endif

  ShIntervalTexturePtr iatex = ShIntervalTexture::convert(tex.node());

  for(int i = 0; i < 2; ++i) {
    ShMemoryPtr mem = iatex->node(i)->memory();
    ShHostStoragePtr strg = shref_dynamic_cast<ShHostStorage>(mem->findStorage("host"));

    std::ostringstream fname;
    fname << "iatex" << i << ".png";

    ShImage result(width * 2, height, 3);
    memcpy(result.data(), strg->data(), 2 * width * height * 3 * sizeof(float));
    result.savePng(fname.str());
  }

  return 0;
}

