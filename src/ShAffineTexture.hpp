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
#ifndef SH_AFFINETEXTURE_HPP
#define SH_AFFINETEXTURE_HPP

#include <iostream>
#include <string>
#include "ShTextureNode.hpp"
#include "ShVariable.hpp"
#include "ShRecord.hpp"
#include "ShMemoryDep.hpp"

namespace SH {

/** Turns a k-dimensional texture into an affine texture with Mip-Mapped
 * acceleration structure. 
 *
 * This doesn't take the "shape" of affine forms for multidimensional 
 * texture coordinates into account, it just handles nearly square 
 * lookups best. 
 *
 * Skeleton code based on Sylvain's HDR texture classes. 
 *
 * @see ShLp.hpp Linear programming used for minimax approximation.
 */
class ShAffineTexture: public virtual ShMemoryDep, public virtual ShRefCountable {
public:
  ~ShAffineTexture();


  // create a new texture memory
  // save the original texture and all the mipmap levels on its right
  void memory_update(); 

  // Returns affine approximation values for a given lookup range
  // @todo range (Does not handle wrapping, clamping, etc.)
  // @{
  ShRecord rect_lookup(const ShVariable& lo, const ShVariable& hi) const;
  ShRecord lookup(const ShVariable& lo, const ShVariable& hi) const;
  // @}

  /* Returns i'th m_node.  Valid 
   * @todo range - for debugging use
   */
  ShTextureNodePtr node(size_t i);

  static ShPointer<ShAffineTexture> convert(ShTextureNodePtr other); 

private:
  ShAffineTexture(ShTextureNodePtr other);

  // makes a temporary from the given variable
  static ShVariableNodePtr makeTemp(const ShVariable &v, 
      const std::string& name, int size = 0);

  ShTextureNodePtr m_other;  

  // texture nodes for new textures.  m_node[0..k-1] represent slopes
  // of the approximation in each dimension, 
  // m_node[k] is the constant factor
  // and m_node[k+1] is the approximation error. 
  ShTextureNodePtr *m_node;  
  size_t m_dims; 
  size_t m_width;

  typedef std::map<ShTextureNodePtr, ShPointer<ShAffineTexture> > TexAaTexMap;
  static TexAaTexMap tatmap;
};

typedef ShPointer<ShAffineTexture> ShAffineTexturePtr;

}

#endif
