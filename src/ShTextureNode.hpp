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
  ShTextureNode(ShTextureDims dims, int width, int height, int depth, int elements);
  virtual ~ShTextureNode();
  
  ShTextureDims dims() const;

  int width() const;
  int height() const;
  int depth() const;
  int elements() const;

  void setData(const float* data);

  const float* data() const;
  
private:
  ShTextureDims m_dims;
  int m_width, m_height, m_depth;
  int m_elements;

  float* m_data;

  // NOT IMPLEMENTED
  ShTextureNode(const ShTextureNode& other);
  ShTextureNode& operator=(const ShTextureNode& other);
};

typedef ShRefCount<ShTextureNode> ShTextureNodePtr;

}
#endif
