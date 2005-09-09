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
template<typename T>
class ShTypedImage : public ShRefCountable {
public:
  ShTypedImage(); ///< Construct an empty image
  ShTypedImage(int width, int height, int depth); ///< Construct a black
                                             ///image at the given width/height/depth
  ShTypedImage(const ShTypedImage& other); ///< Copy an image

  ~ShTypedImage();

  ShTypedImage& operator=(const ShTypedImage& other); ///< Copy the data from
                                            ///one image to another

  int width() const; ///< Determine the width of the image
  int height() const; ///< Determine the height of the image
  int elements() const; ///< Determine the depth (floats per pixel) of
                        ///the image

  T operator()(int x, int y, int i) const; ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  T& operator()(int x, int y, int i);  ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  
  ///< Load a PNG file into this image
  void load_PNG(const std::string& filename);
  
  ///< Save a PNG image into this file
  void save_PNG(const std::string& filename, int inverse_alpha=0);
  
  ///< Save a PNG image into this file
  void save_PNG16(const std::string& filename, int inverse_alpha=0);

  ShTypedImage getNormalImage();

  const T* data() const;
  T* data();
  
  void dirty();
  ShMemoryPtr memory();
  ShPointer<const ShMemory> memory() const;
  
private:
  int m_width, m_height;
  int m_elements;
  ShHostMemoryPtr m_memory;

  /// Returns a copy of the memory data in a float array
  float* float_copy() const;
};

/** Libpng wrapper
 * Allows ShImage to load its data from or save to a PNG file.
 */
class 
SH_DLLEXPORT ShPngImage {
public:
  static float* read_PNG(const std::string& filename, int& width, int& height,
                         int& elements);

  static void save_PNG(const std::string& filename, const float* data,
                       int inverse_alpha, int width, int height, int elements);

  static void save_PNG16(const std::string& filename, const float* data,
                         int inverse_alpha, int width, int height, int elements);
};

typedef ShTypedImage<float> ShImage;

}

#include "ShImageImpl.hpp"

#endif
