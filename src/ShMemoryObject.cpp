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
#include <cstring>
#include <cassert>
#include "ShMemoryObject.hpp"
#include "ShDebug.hpp"

namespace SH {

ShMemoryObject::ShMemoryObject(int width, int height, int depth, int elements)
  : m_width(width), m_height(height), m_depth(depth), m_elements(elements) {
}

ShMemoryObject::~ShMemoryObject()
{
}

int ShMemoryObject::width() const {
  return m_width;
}

int ShMemoryObject::height() const {
  return m_height;
}

int ShMemoryObject::depth() const {
  return m_depth;
}

int ShMemoryObject::elements() const {
  return m_elements;
}

bool ShMemoryObject::copy( ShMemoryObjectPtr b ) {
  float *bdata = b->data();
  if( !compatibleWith(b) || bdata == 0 ) return false;

  setData(bdata);
  delete [] bdata;

  return true;
}

bool ShMemoryObject::compatibleWith( ShMemoryObjectPtr b ) {
  return m_width == b->width() &&
         m_height == b->height() &&
         m_depth == b->depth() &&
         m_elements == b->elements();
}

ShDataMemoryObject::ShDataMemoryObject(int width, int height, int depth, int elements)
  : ShMemoryObject(width, height, depth, elements),
    m_data(new float[m_width * m_height * m_depth * m_elements]) {
}

ShDataMemoryObject::~ShDataMemoryObject()
{
  delete [] m_data;
}

void ShDataMemoryObject::setData(const float *data, int slice) {
  int sliceSize = m_width * m_height * m_elements;
  std::memcpy(&(m_data[slice * sliceSize]), data, sliceSize * sizeof(float));
}

void ShDataMemoryObject::setData(const float *data) {
  setData(data, 0);
}

void ShDataMemoryObject::setPartialData(const float *data, int count) {
  std::memcpy(m_data, data, count * m_elements * sizeof(float));
}

float* ShDataMemoryObject::data() const {
  int size = width() * height() * depth() * elements();
  float* result = new float[ size ];
  memcpy( result, m_data, size * sizeof( float ) );
  return result; 
}

ShExternalMemoryObject::ShExternalMemoryObject(int width, int height, int depth,
    int elements)
  : ShMemoryObject(width, height, depth, elements) {
}

ShExternalMemoryObject::~ShExternalMemoryObject() {
}

};
