#ifndef UBERTEXTURES_HPP
#define UBERTEXTURES_HPP

#include "GlBackend.hpp"

namespace shgl {

class UberTextures : public TextureStrategy {
public:
  UberTextures(int context = 0);

  TextureStrategy* create(int context);
  
  void bindTexture(const SH::ShTextureNodePtr& texture,
                   GLenum target);

private:
  int m_context;
};

}

#endif
