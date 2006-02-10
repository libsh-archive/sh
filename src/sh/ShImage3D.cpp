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
#include "ShImage3D.hpp"
#include <string>
#include <cstring>
#include <cstdio>
#include <sstream>
#include "ShException.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"

namespace SH {

ShImage3D::ShImage3D()
  : m_width(0), m_height(0), m_depth(0), m_elements(0), m_memory(0)
{
}

ShImage3D::ShImage3D(int width, int height, int depth, int elements)
  : m_width(width), m_height(height), m_depth(depth), m_elements(elements),
    m_memory(new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT))
{
}

ShImage3D::ShImage3D(const ShImage3D& other)
  : m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth),
    m_elements(other.m_elements),
    m_memory(other.m_memory ?
             new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT) : 0)
{
  if (m_memory) {
    std::memcpy(m_memory->hostStorage()->data(),
                other.m_memory->hostStorage()->data(),
                m_width * m_height * m_depth * m_elements * sizeof(float));
  }
}

ShImage3D::~ShImage3D()
{
}

ShImage3D& ShImage3D::operator=(const ShImage3D& other)
{
  m_width = other.m_width;
  m_height = other.m_height;
  m_depth = other.m_depth;
  m_elements = other.m_elements;
  m_memory = (other.m_memory ?
              new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements, SH_FLOAT) : 0);
  std::memcpy(m_memory->hostStorage()->data(),
              other.m_memory->hostStorage()->data(),
              m_width * m_height * m_depth * m_elements * sizeof(float));
  
  return *this;
}

int ShImage3D::width() const
{
  return m_width;
}

int ShImage3D::height() const
{
  return m_height;
}

int ShImage3D::depth() const
{
  return m_depth;
}

int ShImage3D::elements() const
{
  return m_elements;
}

float ShImage3D::operator()(int x, int y, int z, int i) const
{
  SH_DEBUG_ASSERT(m_memory);

  return data()[m_elements * ((m_width * (m_height * z + y) + x)) + i];
}

float& ShImage3D::operator()(int x, int y, int z, int i)
{
  SH_DEBUG_ASSERT(m_memory);
  return data()[m_elements * ((m_width * (m_height * z + y) + x)) + i];
}

const float* ShImage3D::data() const
{
  if (!m_memory) return 0;
  return reinterpret_cast<const float*>(m_memory->hostStorage()->data());
}

float* ShImage3D::data()
{
  if (!m_memory) return 0;
  return reinterpret_cast<float*>(m_memory->hostStorage()->data());
}

ShMemoryPtr ShImage3D::memory()
{
  return m_memory;
}

ShPointer<const ShMemory> ShImage3D::memory() const
{
  return m_memory;
}

}
