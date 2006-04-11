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
#ifndef SHIMAGE_HPP
#define SHIMAGE_HPP

#include <string>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Memory.hpp"

namespace SH {

/** An image, consisting of a rectangle of floating-point elements.
 * This class makes it easy to read PNG files and the like from
 * files.   It stores the image data in a memory object which can
 * then be shared with array, table, and texture objects.
 */
template<typename T>
class TypedImage : public RefCountable {
public:
  TypedImage(); ///< Construct an empty image
  TypedImage(int width, int height, int depth); ///< Construct a black
                                             ///image at the given width/height/depth
  TypedImage(const TypedImage& other); ///< Copy an image

  ~TypedImage();

  TypedImage& operator=(const TypedImage& other); ///< Copy the data from
                                            ///one image to another

  int width() const; ///< Determine the width of the image
  int height() const; ///< Determine the height of the image
  int elements() const; ///< Determine the depth (floats per pixel) of
                        ///the image

  void set_size(int width, int height, int depth); /// Set the size of the image 
                                                   /// and create the underlying memory

  T operator()(int x, int y, int i) const; ///< Retrieve a
                                               ///particular component
                                               ///from the image.
  T& operator()(int x, int y, int i);  ///< Retrieve a
                                               ///particular component
                                               ///from the image.

  TypedImage getNormalImage();

  const T* data() const;
  T* data();
  
  void dirty();
  MemoryPtr memory();
  Pointer<const Memory> memory() const;
  
private:
  int m_width, m_height;
  int m_elements;
  HostMemoryPtr m_memory;
};

typedef TypedImage<float> Image;

}

#include "ImageImpl.hpp"

#endif
