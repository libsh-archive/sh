// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHIMAGE_HPP
#define SHIMAGE_HPP

#include <string>
#include "ShRefCount.hpp"

namespace SH {

/** An image, consisting of a rectangle of floating-point elements.
 */
class ShImage : public ShRefCountable {
public:
  ShImage(); ///< Construct an empty image
  ShImage(int width, int height, int depth); ///< Construct a black
                                             ///image at the given width/height/depth
  ShImage(const ShImage& other); ///< Copy an image

  ~ShImage();

  ShImage& operator=(const ShImage& other); ///< Copy the data from
                                            ///one image to another

  int width() const; ///< Determine the width of the image
  int height() const; ///< Determine the height of the image
  int depth() const; ///< Determine the depth (floats per pixel) of
                     ///the image

  float operator()(int x, int y, int i) const; ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  float& operator()(int x, int y, int i);  ///< Retrieve a
                                               ///particular component
                                               ///from the image.

  void loadPng(const std::string& filename); ///< Load a PNG file into
                                             ///this image.

  const float* data() const; ///< Access the raw pixel data.
  
private:
  int m_width, m_height;
  int m_depth;
  float* m_data;
};

}

#endif
