// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHGLTEXTURES_HPP
#define SHGLTEXTURES_HPP

#include "GlBackend.hpp"

namespace shgl {

class GlTextures : public TextureStrategy {
public:
  GlTextures(void);

  TextureStrategy* create(void);
  
  void bindTexture(const SH::TextureNodePtr& texture,
                   GLenum target, bool write);

private:
  // Utility for setting/maintaining active textures
  struct ActiveTexture {
    ActiveTexture(GLenum texture_unit);
    ~ActiveTexture();
  
    GLenum texture_unit;
    GLenum last_unit;
  };

};

}

#endif
