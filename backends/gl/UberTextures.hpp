#ifndef UBERTEXTURES_HPP
#define UBERTEXTURES_HPP

#include "GlBackend.hpp"

namespace shgl {

class UberTextures : public TextureStrategy {
public:
  UberTextures();
  
  void bindTexture(const SH::ShTextureNodePtr& texture,
                   GLenum target);
};

}

#endif
