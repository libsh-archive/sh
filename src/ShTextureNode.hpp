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
#include "ShMemoryObject.hpp"
#include "ShRefCount.hpp"

namespace SH {

enum ShTextureDims {
  SH_TEXTURE_1D,
  SH_TEXTURE_2D,
  SH_TEXTURE_3D,
  SH_TEXTURE_CUBE,
  SH_TEXTURE_RECT
};

enum ShCubeDirection {
  SH_CUBE_POS_X = 0,
  SH_CUBE_NEG_X = 1,
  SH_CUBE_POS_Y = 2,
  SH_CUBE_NEG_Y = 3,
  SH_CUBE_POS_Z = 4,
  SH_CUBE_NEG_Z = 5,
};

class ShTextureNode : public ShVariableNode {
public:
  ShTextureNode(ShTextureDims dims);
  virtual ~ShTextureNode();

  ShTextureDims dims() const;
  
private:
  ShTextureDims m_dims;
  
  // NOT IMPLEMENTED
  ShTextureNode(const ShTextureNode& other);
  ShTextureNode& operator=(const ShTextureNode& other);
};

typedef ShRefCount<ShTextureNode> ShTextureNodePtr;

/// A variable node for textures 
class ShDataTextureNode : public ShTextureNode {
public:
  ShDataTextureNode(ShTextureDims dims, int width, int height, int depth, int elements);
  virtual ~ShDataTextureNode();
  
  int width() const;
  int height() const;
  int depth() const;
  int elements() const;

  // TODO make ShMemoryObjectPtr constant 
  void setMem(ShMemoryObjectPtr data);

  ShMemoryObjectPtr mem() const;
  
private:
  int m_width, m_height, m_depth, m_elements;

  ShMemoryObjectPtr m_mem;

  /// returns true only if width, height, depth, and elements match
  bool compatibleWith(ShMemoryObjectPtr memObj);

  // NOT IMPLEMENTED
  ShDataTextureNode(const ShDataTextureNode& other);
  ShDataTextureNode& operator=(const ShDataTextureNode& other);

};

typedef ShRefCount<ShDataTextureNode> ShDataTextureNodePtr;

/// A variable node for cube map textures
class ShCubeTextureNode : public ShTextureNode {
public:
  ShCubeTextureNode();
  virtual ~ShCubeTextureNode();

  ShDataTextureNodePtr& face(ShCubeDirection dir);
  const ShDataTextureNodePtr face(ShCubeDirection dir) const;
  
private:
  ShDataTextureNodePtr m_faces[6];

  // NOT IMPLEMENTED
  ShCubeTextureNode(const ShCubeTextureNode& other);
  ShCubeTextureNode& operator=(const ShCubeTextureNode& other);
};

typedef ShRefCount<ShCubeTextureNode> ShCubeTextureNodePtr;

}
#endif
