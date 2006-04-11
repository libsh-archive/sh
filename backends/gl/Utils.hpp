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
#ifndef SHUTILS_HPP
#define SHUTILS_HPP

#include <map>
#include <list>
#include "ChannelNode.hpp"
#include "TextureNode.hpp"
#include "CtrlGraph.hpp"
#include "ProgramNode.hpp"
#include "ProgramSet.hpp"
#include "Attrib.hpp"

namespace shgl {

enum FloatExtension {
  ARB_NV_FLOAT_BUFFER,
  ARB_ATI_PIXEL_FORMAT_FLOAT,
  ARB_NO_FLOAT_EXT
};

struct ChannelData {
  ChannelData(SH::TextureNodePtr t, 
              SH::VariableNodePtr o1,
              SH::VariableNodePtr o2)
    : tex_var(t), os1_var(o1), os2_var(o2) { }

  SH::TextureNodePtr tex_var;
  SH::VariableNodePtr os1_var, os2_var;
};

typedef std::map<SH::ChannelNodePtr, ChannelData> ChannelMap;

// Find all the channels read by a given program. Make textures for them.
struct ChannelGatherer {
  ChannelGatherer(ChannelMap& channel_map, SH::TextureDims dims)
    : channel_map(channel_map),
      dims(dims)
  {
  }

  // assignment operator could not be generated
  ChannelGatherer& operator=(ChannelGatherer const&);

  void operator()(const SH::CtrlGraphNode* node);
  
  ChannelMap& channel_map;
  SH::TextureDims dims;
};


// Replace FETCH and LOOKUP operations with texture fetches
// Run this after a pass with channelgatherer.
class TexFetcher {
public:
  TexFetcher(ChannelMap& channel_map,
             const SH::VariableNodePtr& tc_node,
             bool indexed, bool os_calculation,
             const SH::VariableNodePtr& tex_size_node,
             const SH::ProgramNodePtr& program);

  void operator()(SH::CtrlGraphNode* node);

private:
  // assignment operator could not be generated
  TexFetcher& operator=(TexFetcher const&);

  ChannelMap& channel_map;
  SH::VariableNodePtr tc_node;
  bool indexed, os_calculation;
  SH::VariableNodePtr tex_size_node;
  SH::ProgramNodePtr program;
};

class StreamCache : public SH::Info {
public:
  StreamCache(SH::ProgramNode* stream_program,
              SH::ProgramNode* vertex_program,
              int tex_size, int max_outputs, FloatExtension ext);

  SH::Info* clone() const;

  void update_channels();
  void update_destination(int dest_offset, int dest_stride);

  void build_sets(SH::ProgramNode* vertex_program);
  
  void freeze_inputs(bool state);

  typedef std::list<SH::ProgramSetPtr>::iterator set_iterator;
  typedef std::list<SH::ProgramSetPtr>::const_iterator set_const_iterator;
  typedef std::list<SH::ProgramNodePtr>::iterator program_iterator;
  typedef std::list<SH::ProgramNodePtr>::const_iterator program_const_iterator;

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
  SH::ProgramNode* m_stream_program;
  SH::ProgramNode* m_vertex_program;
  ChannelMap m_channel_map;
  std::list<SH::ProgramNodePtr> m_programs[NUM_SET_TYPES];
  std::list<SH::ProgramSetPtr> m_program_sets[NUM_SET_TYPES];
  int m_tex_size;
  int m_max_outputs;
  FloatExtension m_float_extension;
  SH::Attrib4f m_output_offset;
  SH::Attrib3f m_output_stride;

  StreamCache(const StreamCache& other);
  StreamCache& operator=(const StreamCache& other);
};

void split_program(SH::ProgramNode* program,
                   std::list<SH::ProgramNodePtr>& programs,
                   const std::string& target, int chunk_size);

std::string get_target_backend(const SH::ProgramNodeCPtr &program);

}

#endif
