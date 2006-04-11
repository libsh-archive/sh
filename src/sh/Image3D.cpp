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
#include "Image3D.hpp"
#include <string>
#include <cstring>
#include <cstdio>
#include <sstream>
#include "Exception.hpp"
#include "Error.hpp"
#include "Debug.hpp"

namespace SH {

Image3D::Image3D()
  : m_width(0), m_height(0), m_depth(0), m_elements(0), m_memory(0)
{
}

Image3D::Image3D(int width, int height, int depth, int elements)
  : m_width(width), m_height(height), m_depth(depth), m_elements(elements),
    m_memory(new HostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT))
{
}

Image3D::Image3D(const Image3D& other)
  : m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth),
    m_elements(other.m_elements),
    m_memory(other.m_memory ?
             new HostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT) : 0)
{
  if (m_memory) {
    std::memcpy(m_memory->hostStorage()->data(),
                other.m_memory->hostStorage()->data(),
                m_width * m_height * m_depth * m_elements * sizeof(float));
  }
}

Image3D::~Image3D()
{
}

Image3D& Image3D::operator=(const Image3D& other)
{
  m_width = other.m_width;
  m_height = other.m_height;
  m_depth = other.m_depth;
  m_elements = other.m_elements;
  m_memory = (other.m_memory ?
              new HostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT) : 0);
  std::memcpy(m_memory->hostStorage()->data(),
              other.m_memory->hostStorage()->data(),
              m_width * m_height * m_depth * m_elements * sizeof(float));
  
  return *this;
}

int Image3D::width() const
{
  return m_width;
}

int Image3D::height() const
{
  return m_height;
}

int Image3D::depth() const
{
  return m_depth;
}

int Image3D::elements() const
{
  return m_elements;
}

float Image3D::operator()(int x, int y, int z, int i) const
{
  DEBUG_ASSERT(m_memory);

  return data()[m_elements * ((m_width * (m_height * z + y) + x)) + i];
}

float& Image3D::operator()(int x, int y, int z, int i)
{
  DEBUG_ASSERT(m_memory);
  return data()[m_elements * ((m_width * (m_height * z + y) + x)) + i];
}

const float* Image3D::data() const
{
  if (!m_memory) return 0;
  return reinterpret_cast<const float*>(m_memory->hostStorage()->data());
}

float* Image3D::data()
{
  if (!m_memory) return 0;
  return reinterpret_cast<float*>(m_memory->hostStorage()->data());
}

MemoryPtr Image3D::memory()
{
  return m_memory;
}

Pointer<const Memory> Image3D::memory() const
{
  return m_memory;
}

}
