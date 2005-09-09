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
#ifndef SHIMAGEIMPL_HPP
#define SHIMAGEIMPL_HPP

#include "ShImage.hpp"
#include "ShDebug.hpp"

namespace SH {

template<typename T>
ShTypedImage<T>::ShTypedImage(int width, int height, int elements)
  : m_width(width), m_height(height), m_elements(elements),
    m_memory(new ShHostMemory(sizeof(T) * m_width * m_height * m_elements, ShStorageTypeInfo<T>::value_type))
{
}

template<typename T>
ShTypedImage<T>::ShTypedImage(const ShTypedImage<T>& other)
  : m_width(other.m_width), m_height(other.m_height), m_elements(other.m_elements),
    m_memory(other.m_memory ? new ShHostMemory(sizeof(T) * m_width * m_height * m_elements, ShStorageTypeInfo<T>::value_type) : 0)
{
  if (m_memory) {
    std::memcpy(m_memory->hostStorage()->data(),
                other.m_memory->hostStorage()->data(),
                m_width * m_height * m_elements * sizeof(T));
  }
}

template<typename T>
ShTypedImage<T>& ShTypedImage<T>::operator=(const ShTypedImage<T>& other)
{
  m_width = other.m_width;
  m_height = other.m_height;
  m_elements = other.m_elements;
  m_memory = (other.m_memory ? new ShHostMemory(sizeof(T) * m_width * m_height * m_elements, ShStorageTypeInfo<T>::value_type) : 0);
  std::memcpy(m_memory->hostStorage()->data(),
              other.m_memory->hostStorage()->data(),
              m_width * m_height * m_elements * sizeof(T));
  
  return *this;
}

template<typename T>
void ShTypedImage<T>::load_PNG(const std::string& filename)
{
  float* png_data = ShPngImage::read_PNG(filename, m_width, m_height, m_elements);
  int array_length = m_width * m_height * m_elements;

  m_memory = new ShHostMemory(sizeof(T) * array_length, ShStorageTypeInfo<T>::value_type);
  T* storage_data = static_cast<T*>(m_memory->hostStorage()->data());

  for (int i=0; i < array_length; i++) {
    storage_data[i] = png_data[i];
  }

  delete [] png_data;
}

template<typename T>
float* ShTypedImage<T>::float_copy() const
{
  int array_length = m_width * m_height * m_elements;
  float* float_data = new float[array_length];
  
  const T* storage_data = static_cast<const T*>(m_memory->hostStorage()->data());
  for (int i=0; i < array_length; i++) {
    float_data[i] = storage_data[i];
  }
  
  return float_data;
}

template<typename T>
void ShTypedImage<T>::save_PNG(const std::string& filename, int inverse_alpha)
{
  float* float_data = float_copy();
  ShPngImage::save_PNG(filename, float_data, inverse_alpha, m_width, m_height, m_elements);
  delete [] float_data;
}

template<typename T>
void ShTypedImage<T>::save_PNG16(const std::string& filename, int inverse_alpha)
{
  float* float_data = float_copy();
  ShPngImage::save_PNG16(filename, float_data, inverse_alpha, m_width, m_height, m_elements);
  delete [] float_data;
}

template<typename T>
ShTypedImage<T> ShTypedImage<T>::getNormalImage()
{
  int w = width();
  int h = height();
  ShTypedImage<T> output_image(w,h,3);
  for (int j = 0; j < h; j++) {
    int jp1 = j + 1;
    if (jp1 >= h) jp1 = 0;
    int jm1 = (j - 1);
    if (jm1 < 0) jm1 = h - 1;
    for (int i = 0; i < w; i++) {
      int ip1 = i + 1;
      if (ip1 >= w) ip1 = 0;
      int im1 = (i - 1);
      if (im1 < 0) im1 = w - 1;
      T x, y, z;
      x = ((*this)(ip1,j,0) - (*this)(im1,j,0))/2.0f;
      output_image(i,j,0) = x/2.0f + 0.5f;
      y = ((*this)(i,jp1,0) - (*this)(i,jm1,0))/2.0f;
      output_image(i,j,1) = y/2.0f + 0.5f;
      z = x*x + y*y;
      if (z < 1.0f) {
	z = std::sqrt(1 - z);
      } else {
	z = 0.0f;
      }
      output_image(i,j,2) = z;
    }
  }
  return output_image;
}

template<typename T>
const T* ShTypedImage<T>::data() const
{
  if (!m_memory) return 0;
  return reinterpret_cast<const T*>(m_memory->hostStorage()->data());
}

template<typename T>
T* ShTypedImage<T>::data()
{
  if (!m_memory) return 0;
  return reinterpret_cast<T*>(m_memory->hostStorage()->data());
}

template<typename T>
T ShTypedImage<T>::operator()(int x, int y, int i) const
{
  SH_DEBUG_ASSERT(m_memory);
  return data()[m_elements * (m_width * y + x) + i];
}

template<typename T>
T& ShTypedImage<T>::operator()(int x, int y, int i)
{
  return data()[m_elements * (m_width * y + x) + i];
}

template<typename T>
ShTypedImage<T>::ShTypedImage()
  : m_width(0), m_height(0), m_elements(0), m_memory(0)
{
}

template<typename T>
ShTypedImage<T>::~ShTypedImage()
{
}

template<typename T>
int ShTypedImage<T>::width() const
{
  return m_width;
}

template<typename T>
int ShTypedImage<T>::height() const
{
  return m_height;
}

template<typename T>
int ShTypedImage<T>::elements() const
{
  return m_elements;
}

template<typename T>
void ShTypedImage<T>::dirty() 
{
  if (!m_memory) return;
  m_memory->hostStorage()->dirty();
}

template<typename T>
ShMemoryPtr ShTypedImage<T>::memory()
{
  return m_memory;
}

template<typename T>
ShPointer<const ShMemory> ShTypedImage<T>::memory() const
{
  return m_memory;
}

}

#endif
