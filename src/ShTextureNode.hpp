#ifndef SHTEXTURENODE_HPP
#define SHTEXTURENODE_HPP

#include "ShVariableNode.hpp"
#include "ShRefCount.hpp"

namespace SH {

enum ShTextureDims {
  SH_TEXTURE_1D,
  SH_TEXTURE_2D,
  SH_TEXTURE_3D,
  SH_TEXTURE_CUBE
};

class ShTextureNode : public ShVariableNode {
public:
  ShTextureNode(ShTextureDims dims, int width, int height, int elements);
  virtual ~ShTextureNode();
  
  ShTextureDims dims() const;

  int width() const;
  int height() const;
  int elements() const;

  void setData(const float* data);

  const float* data() const;
  
private:
  ShTextureDims m_dims;
  int m_width, m_height;
  int m_elements;

  float* m_data;

  // NOT IMPLEMENTED
  ShTextureNode(const ShTextureNode& other);
  ShTextureNode& operator=(const ShTextureNode& other);
};

typedef ShRefCount<ShTextureNode> ShTextureNodePtr;

}
#endif
