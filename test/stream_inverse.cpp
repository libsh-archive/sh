#include "sh.hpp"

using namespace SH;

int main(int argc, char** argv)
{
  try { 
    shSetBackend("arb");

    ShImage image;
    image.loadPng(argv[1]);

    //ShChannel<ShColor3f> data(image.memory(), image.width() * image.height());

    ShImage idx_img(image.width(), image.height(), 3);
    idx_img.memory()->findStorage("host")->dirty();
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        idx_img(x, y, 0) = (float)x;///(float)image.width();
        idx_img(x, y, 1) = (float)y;///(float)image.height();
      }
    }
    ShChannel<ShTexCoord3f> index(idx_img.memory(), image.width() * image.height());
    
    ShArrayRect<ShColor3f> picture(image.width(), image.height());
    picture.memory(image.memory());
    
    ShImage result(image.width(), image.height(), 3);
    ShChannel<ShColor3f> output(result.memory(), image.width() * image.height());

    ShProgram invert = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputTexCoord3f idx;
      ShOutputColor3f outcol;
      outcol = idx;//picture(idx);
    } SH_END;

    ShStream outstream(output);
    ShEnvironment::backend->execute(invert << index, outstream);

    result.savePng("result.png");
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }
}
