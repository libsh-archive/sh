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
#ifndef SH_INTERVALTEXTURE_HPP
#define SH_INTERVALTEXTURE_HPP

#include <iostream>
#include <string>
#include "ShTextureNode.hpp"
#include "ShVariable.hpp"
#include "ShRecord.hpp"
#include "ShMemoryDep.hpp"

namespace SH {

/** Turns a regular texture into a interval texture with mip-map like acceleration structure. 
 *
 * Modified from Sylvain's HDRShIntervalTexture.hpp to handle nD textures (but not Rect/Cube yet) 
 * (actually @todo range it only works for 2D and Rect textures)  
 *
 * Since this is for internal use, it works on ShTextureNodes. 
  */
class ShIntervalTexture: public virtual ShMemoryDep, public virtual ShRefCountable {
public:
  ShIntervalTexture(ShTextureNodePtr other);

  // create a new texture memory
  // save the original texture and all the mipmap levels on its right
  void memory_update(); 

  ShRecord lookup(const ShVariable &lo, const ShVariable& hi) const;
  ShRecord rect_lookup(const ShVariable &lo, const ShVariable& hi) const;

  ShTextureNodePtr node(size_t i); 

  static ShPointer<ShIntervalTexture> convert(ShTextureNodePtr other);

private:
  ShTextureNodePtr m_other;  
  ShTextureNodePtr m_node[2];  // lo and hi nodes
  size_t m_width;  // width of other
  size_t m_dims;


  static ShVariableNodePtr makeTemp(const ShVariable &v, 
          const std::string& name, int size=0);

  typedef std::map<ShTextureNodePtr, ShPointer<ShIntervalTexture> > TexIATexMap;
  static TexIATexMap tiatmap;

};

typedef ShPointer<ShIntervalTexture> ShIntervalTexturePtr;

}

#endif
