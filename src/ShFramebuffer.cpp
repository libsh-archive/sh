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
#include <cstring>
#include <cassert>
#include "ShFramebuffer.hpp"
#include "ShEnvironment.hpp"
#include "ShUberbuffer.hpp"

#include <iostream>

namespace SH {

ShFramebuffer::ShFramebuffer(int width, int height, int depth, int elements)
  : m_width(width), m_height(height), m_depth(depth), m_elements(elements),
    m_fb(0), m_ub(0) {
}

ShFramebuffer::~ShFramebuffer() {
  if( m_fb != 0 && ShEnvironment::backend ) {
    ShEnvironment::backend->deleteFramebuffer( this );
  }
}

int ShFramebuffer::width() const {
  return m_width;
}

int ShFramebuffer::height() const {
  return m_height;
}

int ShFramebuffer::depth() const {
  return m_depth;
}

int ShFramebuffer::elements() const {
  return m_elements;
}

unsigned int ShFramebuffer::fb() const {
  return m_fb;
}

void ShFramebuffer::setFb( unsigned int fb ) {
  m_fb = fb;
}


void ShFramebuffer::bind( ShUberbufferPtr ub ) {
  m_ub = ub;
  if( ShEnvironment::framebuffer == this ) {
    // update backend if this is the active draw buffer
    if( ShEnvironment::backend ) ShEnvironment::backend->bindFramebuffer();
  } 
}

ShUberbufferPtr ShFramebuffer::getUberbuffer() {
  return m_ub;
}


};
