#ifndef GLTEXTURES_HPP
#define GLTEXTURES_HPP

#include "GlBackend.hpp"

namespace shgl {

class GlTextures : public TextureStrategy {
public:
  GlTextures();
  
  void bindTexture(const SH::ShTextureNodePtr& texture,
                   GLenum target);
};

}

#endif
