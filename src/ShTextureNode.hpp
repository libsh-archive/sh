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
#include "ShMemory.hpp"
#include "ShRefCount.hpp"

namespace SH {

enum ShTextureDims {
  SH_TEXTURE_1D,   // Power of two
  SH_TEXTURE_2D,   // Power of two
  SH_TEXTURE_RECT, // Non power of two
  SH_TEXTURE_3D,   // Power of two, but depth may not be
  SH_TEXTURE_CUBE, // 6 "2D" memory objects, power of two
};

enum ShCubeDirection {
  SH_CUBE_POS_X = 0,
  SH_CUBE_NEG_X = 1,
  SH_CUBE_POS_Y = 2,
  SH_CUBE_NEG_Y = 3,
  SH_CUBE_POS_Z = 4,
  SH_CUBE_NEG_Z = 5,
};

const unsigned int SH_LOOKUP_NEAREST = 0x001;
const unsigned int SH_LOOKUP_LINEAR  = 0x002;
const unsigned int SH_LOOKUP_MASK    = 0x00f;
const unsigned int SH_FILTER_NONE    = 0x010;
const unsigned int SH_FILTER_MIPMAP  = 0x020;
const unsigned int SH_FILTER_MASK    = 0x0f0;
// TODO: different wrapping modes for different texcoords
// TODO: edges
const unsigned int SH_WRAP_CLAMP     = 0x100;
const unsigned int SH_WRAP_REPEAT    = 0x200;
const unsigned int SH_WRAP_MASK      = 0xf00;

class ShTextureNode : public ShVariableNode {
public:
  ShTextureNode(ShTextureDims dims,
                int size, // scalars per tuple
                unsigned int traits,
                int width, int height = 0, int depth = 0);
  virtual ~ShTextureNode();

  ShTextureDims dims() const;

  // Memory
  ShRefCount<const ShMemory> memory(int n = 0) const;
  ShRefCount<const ShMemory> memory(ShCubeDirection dir) const;
  ShMemoryPtr memory(int n = 0);
  ShMemoryPtr memory(ShCubeDirection dir);
  void memory(ShMemoryPtr memory, int n = 0);
  void memory(ShMemoryPtr memory, ShCubeDirection dir);

  // Basic properties - not all may be valid for all types
  unsigned int traits() const; // valid for all texture nodes
  void traits(unsigned int traits); // valid for all texture nodes
  int width() const; // valid for all texture nodes
  int height() const; // not for SH_TEXTURE_1D
  int depth() const; // only for SH_TEXTURE_3D

private:
  ShTextureDims m_dims;

  ShMemoryPtr* m_memory; // array of either 1 or 6 (for cubemaps)
  
  unsigned int m_traits;
  int m_width, m_height, m_depth;
  
  // NOT IMPLEMENTED
  ShTextureNode(const ShTextureNode& other);
  ShTextureNode& operator=(const ShTextureNode& other);
};

typedef ShRefCount<ShTextureNode> ShTextureNodePtr;

}
#endif
