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
#ifndef SHFBOSTREAMS_HPP
#define SHFBOSTREAMS_HPP

#include <string>
#include "Program.hpp"
#include "GlBackend.hpp"
#include "Utils.hpp"

namespace shgl {

struct FBOStreams : public StreamStrategy {
  FBOStreams(std::string name);
  virtual ~FBOStreams();

  void execute(const SH::Program& program, 
               SH::Stream& dest, TextureStrategy *texture);

  SH::BaseTexture gather(const SH::BaseTexture& src,
                         const SH::BaseTexture& index,
                         TextureStrategy* texture_strategy);

  virtual StreamStrategy* create();

private:
  std::string m_name;

  bool m_setup_vp;
  SH::Program m_vp;
  
  enum DrawBuffersExt {
    ATI,
    ARB,
    OGL20,
    NONE
  };
  DrawBuffersExt m_draw_buffers_ext;
  FloatExtension m_float_extension;
  int m_max_draw_buffers;
  
  struct GatherData {
    SH::ProgramSetPtr program;
    SH::TextureNodePtr src, index;
    SH::Attrib4f size;
  };
  typedef std::map<std::pair<SH::TextureDims, SH::TextureDims>, GatherData> GatherCache;
  GatherCache m_gather_cache;
  
  GatherData& get_gather_data(SH::TextureDims src_dims, SH::TextureDims idx_dims);
};

}

#endif
