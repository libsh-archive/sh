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
    m_memory(new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements))
{
}

ShImage3D::ShImage3D(const ShImage3D& other)
  : m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth),
    m_elements(other.m_elements),
    m_memory(other.m_memory ?
             new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements) : 0)
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
              new ShHostMemory(sizeof(float) * m_width * m_height * m_depth * m_elements) : 0);
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
