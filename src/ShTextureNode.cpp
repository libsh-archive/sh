#include "ShTextureNode.hpp"

namespace SH {

ShTextureNode::ShTextureNode(ShTextureDims dims, int width, int height, int elements)
  : ShVariableNode(SH_VAR_TEXTURE, 1), m_dims(dims),
    m_width(width), m_height(height), m_elements(elements),
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

