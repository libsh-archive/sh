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
#include "ShTextureNode.hpp"

namespace SH {

ShTextureNode::ShTextureNode(ShTextureDims dims, int width, int height, int depth, int elements)
  : ShVariableNode(SH_VAR_TEXTURE, 1), m_dims(dims),
    m_width(width), m_height(height), m_depth(depth), m_elements(elements),
    m_data(new float[width * height * elements])
{
}

ShTextureNode::~ShTextureNode()
{
  delete [] m_data;
}

ShTextureDims ShTextureNode::dims() const
{
  return m_dims;
}

int ShTextureNode::width() const
{
  return m_width;
}

int ShTextureNode::height() const
{
  return m_height;
}

int ShTextureNode::depth() const
{
  return m_depth;
}

int ShTextureNode::elements() const
{
  return m_elements;
}

void ShTextureNode::setData(const float* data)
{
  memcpy(m_data, data, m_width * m_height * m_elements * sizeof(float));
}

const float* ShTextureNode::data() const
{
  return m_data;
}

}

