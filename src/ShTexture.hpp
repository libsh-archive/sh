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

  ShTextureNodePtr node() const;

  /// Attach a memory object to this texture
  // This currently does not work with cube maps
  void attach(ShMemoryObjectPtr memObj);

protected:
  ShTexture(ShTextureNodePtr node);
  
  ShTextureNodePtr m_node;

  void loadImage(const ShImage& image, int slice = 0);
};  

template<typename T>
class ShTexture1D : public ShTexture<T> {
public:
  ShTexture1D(int length);

  /// Lookup a value, coords being in [0,1]
  T operator()(const ShVariableN<1, double>& coords) const;

  void load(const ShImage& image);
};

template<typename T>
class ShTexture2D : public ShTexture<T> {
public:
  ShTexture2D(int width, int height);

  /// Lookup a value, coords being in [0,1] x [0,1]
  T operator()(const ShVariableN<2, double>& coords) const;

  void load(const ShImage& image);
};

template<typename T>
class ShTexture3D : public ShTexture<T> {
public:
  ShTexture3D(int width, int height, int depth);

  /// Lookup a value, coords being in [0,1] x [0,1] x [0,1]
  T operator()(const ShVariableN<3, double>& coords) const;

  /// Load the given image at the given depth slice 
  // (loads the 2D plane with tex coords (x,y,slice), 
  // (x,y) in [0,1] x [0,1]
  void load(const ShImage& image, int slice);
};

template<typename T>
class ShTextureCube {
public:
  ShTextureCube();

  void set(ShCubeDirection dir, const ShTexture2D<T>& texture);

  T operator()(const ShVariableN<3, double>& direction) const ;
  
private:
  ShCubeTextureNodePtr m_node;
};

}

#include "ShTextureImpl.hpp"

#endif
