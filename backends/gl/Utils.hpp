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
#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <list>
#include "ShChannelNode.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"
#include "ShProgramSet.hpp"

namespace shgl {

enum FloatExtension {
  SH_ARB_NV_FLOAT_BUFFER,
  SH_ARB_ATI_PIXEL_FORMAT_FLOAT,
  SH_ARB_NO_FLOAT_EXT
};

struct ChannelData {
  ChannelData(SH::ShTextureNodePtr t, 
              SH::ShVariableNodePtr o1,
              SH::ShVariableNodePtr o2)
    : tex_var(t), os1_var(o1), os2_var(o2) { }

  SH::ShTextureNodePtr tex_var;
  SH::ShVariableNodePtr os1_var, os2_var;
};

typedef std::map<SH::ShChannelNodePtr, ChannelData> ChannelMap;

// Find all the channels read by a given program. Make textures for them.
struct ChannelGatherer {
  ChannelGatherer(ChannelMap& channel_map, SH::ShTextureDims dims)
    : channel_map(channel_map),
      dims(dims)
  {
  }

  // assignment operator could not be generated
  ChannelGatherer& operator=(ChannelGatherer const&);

  void operator()(const SH::ShCtrlGraphNode* node);
  
  ChannelMap& channel_map;
  SH::ShTextureDims dims;
};


// Replace FETCH and LOOKUP operations with texture fetches
// Run this after a pass with channelgatherer.
class TexFetcher {
public:
  TexFetcher(ChannelMap& channel_map,
             const SH::ShVariableNodePtr& tc_node,
             bool indexed, bool os_calculation, int tex_size,
             const SH::ShProgramNodePtr& program);

  void operator()(SH::ShCtrlGraphNode* node);

private:
  // assignment operator could not be generated
  TexFetcher& operator=(TexFetcher const&);

  ChannelMap& channel_map;
  SH::ShVariableNodePtr tc_node;
  bool indexed, os_calculation;
  int tex_size;
  SH::ShProgramNodePtr program;
};

class StreamCache : public SH::ShInfo {
public:
  StreamCache(SH::ShProgramNode* stream_program,
              SH::ShProgramNode* vertex_program,
              int tex_size, int max_outputs, FloatExtension ext);

  SH::ShInfo* clone() const;

  void update_channels();

  void build_sets(SH::ShProgramNode* vertex_program);
  
  void freeze_inputs(bool state);

  typedef std::list<SH::ShProgramSetPtr>::iterator set_iterator;
  typedef std::list<SH::ShProgramSetPtr>::const_iterator set_const_iterator;
  typedef std::list<SH::ShProgramNodePtr>::iterator program_iterator;
  typedef std::list<SH::ShProgramNodePtr>::const_iterator program_const_iterator;

  enum SetType {
    NO_OFFSET_STRIDE = 0,
    FULL,
    SINGLE_OUTPUT,
    NUM_SET_TYPES
  };

  set_iterator sets_begin(SetType type);
  set_iterator sets_end(SetType type);
  set_const_iterator sets_begin(SetType type) const;
  set_const_iterator sets_end(SetType type) const;

  int tex_size() { return m_tex_size; }

private:
  SH::ShProgramNode* m_stream_program;
  SH::ShProgramNode* m_vertex_program;
  ChannelMap m_channel_map;
  std::list<SH::ShProgramNodePtr> m_programs[NUM_SET_TYPES];
  std::list<SH::ShProgramSetPtr> m_program_sets[NUM_SET_TYPES];
  int m_tex_size;
  int m_max_outputs;
  FloatExtension m_float_extension;

  StreamCache(const StreamCache& other);
  StreamCache& operator=(const StreamCache& other);
};

void split_program(SH::ShProgramNode* program,
                   std::list<SH::ShProgramNodePtr>& programs,
                   const std::string& target, int chunk_size);

std::string get_target_backend(const SH::ShProgramNodeCPtr &program);

}

#endif
