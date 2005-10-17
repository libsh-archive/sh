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
#ifndef SHPNGIMAGEIMPL_HPP
#define SHPNGIMAGEIMPL_HPP

#include "ShPngImage.hpp"
#include "ShDllExport.hpp"

namespace ShUtil {

using namespace SH;
using namespace std;

struct ShPngImage {
  static float* read_PNG(const string& filename, int& width, int& height, 
                         int& elements);
  static void write_PNG(const string& filename, const float* data, 
                        int inverse_alpha, int width, int height, int elements);
  static void write_PNG16(const string& filename, const float* data, 
                          int inverse_alpha, int width, int height, int elements);
};

/// Copy the contents of the image into a float array
template<typename T>
float* float_copy(const ShTypedImage<T>& image)
{
  int array_length = image.width() * image.height() * image.elements();
  float* float_data = new float[array_length];
  
  const T* storage_data = image.data();
  for (int i=0; i < array_length; i++) {
    float_data[i] = storage_data[i];
  }

  return float_data;
}

// Public functions

template<typename T>
void load_PNG(ShTypedImage<T>& image, const string& filename)
{
  int width, height, depth;
  float* png_data = ShPngImage::read_PNG(filename, width, height, depth);

  image.set_size(width, height, depth);
  T* storage_data = image.data();

  int array_length = width * height * depth;
  for (int i=0; i < array_length; i++) {
    storage_data[i] = png_data[i];
  }

  delete [] png_data;
}

template<typename T>
void save_PNG(const ShTypedImage<T>& image, const string& filename, int inverse_alpha)
{
  float* float_data = float_copy(image);
  ShPngImage::write_PNG(filename, float_data, inverse_alpha, image.width(), image.height(), image.elements());
  delete [] float_data;
}

template<typename T>
void save_PNG16(const ShTypedImage<T>& image, const string& filename, int inverse_alpha)
{
  float* float_data = float_copy(image);
  ShPngImage::write_PNG16(filename, float_data, inverse_alpha, image.width(), image.height(), image.elements());
  delete [] float_data;
}

} // namespace

#endif // SHPNGIMAGEIMPL_HPP
