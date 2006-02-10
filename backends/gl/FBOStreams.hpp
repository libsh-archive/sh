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
#ifndef FBOSTREAMS_HPP
#define FBOSTREAMS_HPP

#include "ShProgram.hpp"
#include "GlBackend.hpp"

namespace shgl {

struct FBOStreams : public StreamStrategy {
  FBOStreams();
  virtual ~FBOStreams();

  void execute(const SH::ShProgramNodeCPtr& program, 
               SH::ShStream& dest, TextureStrategy *texture);

  virtual StreamStrategy* create();

private:
  SH::ShProgramSet* m_shaders;
  bool m_setup_vp;
  SH::ShProgram m_vp;
  
  enum DrawBuffersExt {
    ATI,
    ARB,
    OGL20,
    NONE
  };
  DrawBuffersExt m_draw_buffers_ext;
  int m_max_draw_buffers;
  
  GLuint m_framebuffer;
};

}

#endif
