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
#ifndef SHIMAGE_HPP
#define SHIMAGE_HPP

#include <string>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShMemory.hpp"

namespace SH {

/** An image, consisting of a rectangle of floating-point elements.
 * This class makes it easy to read PNG files and the like from
 * files.   It stores the image data in a memory object which can
 * then be shared with array, table, and texture objects.
 */
class
SH_DLLEXPORT ShImage : public ShRefCountable {
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
  int elements() const; ///< Determine the depth (floats per pixel) of
                        ///the image

  float operator()(int x, int y, int i) const; ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  float& operator()(int x, int y, int i);  ///< Retrieve a
                                               ///particular component
                                               ///from the image.

  /** Load a PNG file into this image 
   *
   * @deprecated Provided for backward compatibility with the book
   * (e.g. page 181)
   */
  void load_PNG(const std::string& filename) { loadPng(filename); }
  void loadPng(const std::string& filename); ///< Load a PNG file into
                                             ///this image.
  
  void savePng(const std::string& filename, int inverse_alpha=0);///< Save a PNG image into
                                             ///this file
  
  void savePng16(const std::string& filename, int inverse_alpha=0);///< Save a PNG image into
 
  ShImage getNormalImage();

  const float* data() const;
  float* data();
  
  void dirty();
  ShMemoryPtr memory();
  ShPointer<const ShMemory> memory() const;
  
private:
  int m_width, m_height;
  int m_elements;
  ShHostMemoryPtr m_memory;
};

}

#endif
