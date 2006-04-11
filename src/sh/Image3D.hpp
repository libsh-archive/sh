// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#ifndef IMAGE3D_HPP
#define IMAGE3D_HPP

#include <string>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Memory.hpp"

namespace SH {

/** A 3D image.
 * Consists of a grid of floating-point elements.   Stores data
 * in a memory object that can be shared with a 3D array, table, or
 * texture.
 * @see Image
 */
class
DLLEXPORT Image3D : public RefCountable {
public:
  Image3D(); ///< Construct an empty image
  Image3D(int width, int height, int depth, int elements); ///< Construct a black
                                             ///image at the given width/height/elements
  Image3D(const Image3D& other); ///< Copy an image

  ~Image3D();

  Image3D& operator=(const Image3D& other); ///< Copy the data from
                                            ///one image to another

  int width() const; ///< Determine the width of the image
  int height() const; ///< Determine the height of the image
  int depth() const; ///< Determine the depth of the image
  int elements() const; ///< Determine the elements (floats per pixel) of
                     ///the image

  float operator()(int x, int y, int z, int i) const; ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  float& operator()(int x, int y, int z, int i);  ///< Retrieve a
                                               ///particular component
                                               ///from the image.

  const float* data() const;
  float* data();
  
  MemoryPtr memory();
  Pointer<const Memory> memory() const;
  
private:
  int m_width, m_height, m_depth;
  int m_elements;
  HostMemoryPtr m_memory;
};

}

#endif
