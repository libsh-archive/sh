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
