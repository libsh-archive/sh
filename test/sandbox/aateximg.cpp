#include <iostream>
#include <sstream>
#include <sh.hpp>
#include "ShLp.hpp"
#include "ShAffineTexture.hpp"

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
  ShTexture2D<ShColor3f> tex(width, height);
  tex.memory(img.memory());

  try {
    ShAffineTexturePtr aatex = ShAffineTexture::convert(tex.node());

    for(int i = 0; i < 4; ++i) {
      ShMemoryPtr mem = aatex->node(i)->memory();
      ShHostStoragePtr strg = shref_dynamic_cast<ShHostStorage>(mem->findStorage("host"));

      std::ostringstream fname;
      fname << "aatex" << i << ".png";

      ShImage result(width * 3, height, 3);
      memcpy(result.data(), strg->data(), 3 * width * height * 3 * sizeof(float));
      result.savePng(fname.str());
    }
    
  } catch (const ShUnboundedLpException &e) {
    std::cout << "Unbounded LP" << std::endl;
  } catch (const ShInfeasibleLpException &e) {
    std::cout << "Infeasible LP" << std::endl;
  }


  return 0;
}

