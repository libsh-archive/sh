#ifndef GLTEXTURES_HPP
#define GLTEXTURES_HPP

#include "GlBackend.hpp"

namespace shgl {

class GlTextures : public TextureStrategy {
public:
  GlTextures(int context = 0);

  TextureStrategy* create(int context);
  
  void bindTexture(const SH::ShTextureNodePtr& texture,
                   GLenum target);

private:
  int m_context;
};

}

#endif
