// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include <exception>
#include <iostream>
#include <fstream>
#include "sh.hpp"

using namespace SH;

int main(int argc, char** argv)
{
  try { 
    shSetBackend("arb");

    ShImage image;
    image.loadPng(argv[1]);

    //ShChannel<ShColor3f> data(image.memory(), image.width() * image.height());

#define INDEX_ELEMENTS 3
    
    ShImage idx_img(image.width(), image.height(), INDEX_ELEMENTS);
    idx_img.memory()->findStorage("host")->dirty();
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        idx_img(x, y, 0) = (float)x/image.width();
        idx_img(x, y, 1) = (float)y/image.height();
      }
    }
    ShChannel<ShTexCoord<INDEX_ELEMENTS, SH_TEMP> >
      index(idx_img.memory(), image.width() * image.height());
    
    ShTableRect<ShColor3f> picture(image.width(), image.height());
    picture.memory(image.memory());
    
    ShImage result(image.width(), image.height(), 3);
    ShChannel<ShColor3f> output(result.memory(), image.width() * image.height());

    /*
    ShMatrix3x3f sobel_x = ShMatrix3x3f(-1.0, 0.0, +1.0,
                                        -2.0, 0.0, +2.0,
                                        -1.0, 0.0, -1.0);
    ShMatrix3x3f sobel_y = transpose(sobel_x);
    */
    ShProgram invert = SH_BEGIN_PROGRAM("gpu:stream") {
      ShTexCoord<INDEX_ELEMENTS, SH_INPUT> idx;
      ShOutputColor3f outcol;

      ShTexCoord2f t = idx(0,1);
      
      //outcol = picture(t - ShConstant2f(1.0, 0.0)) - picture(t);
      //outcol = dot(outcol,outcol)(0,0,0);

      
      ShAttrib2f l;
      l(0) = t(0)*t(0);// /image.width();
      l(1) = t(1)*t(1);// /image.height();
      outcol = picture(l);
    } SH_END;

    ShStream outstream(output);
    ShEnvironment::backend->execute(invert << index, outstream);

    result.savePng("result.png");
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (const std::exception& e) {
    std::cerr << "C++ Exception: " << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    abort();
  }
}
