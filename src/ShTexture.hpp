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
#ifndef SHTEXTURE_HPP
#define SHTEXTURE_HPP

#include "ShRefCount.hpp"
#include "ShTextureNode.hpp"
#include "ShImage.hpp"

namespace SH {

template<typename T>
class ShTexture : public ShRefCountable {
public:
  ~ShTexture();
  
protected:
  ShTexture(ShTextureNodePtr node);
  
  ShTextureNodePtr m_node;
};  

template<typename T>
class ShTexture2D : public ShTexture<T> {
public:
  ShTexture2D(int width, int height);

  // TODO: Use some sort of TexCoord class instead?
  /// Lookup a value, coords being in [0,1] x [0,1]
  T operator()(const ShVariableN<2, double>& coords);

  void load(const ShImage& image);
};

}

#include "ShTextureImpl.hpp"

#endif
