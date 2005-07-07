// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

namespace shgl {

typedef std::map<SH::ShChannelNodePtr, SH::ShTextureNodePtr> ChannelMap;

// Find all the channels read by a given program. Make textures for them.
struct ChannelGatherer {
  ChannelGatherer(ChannelMap& channel_map, SH::ShTextureDims dims)
    : channel_map(channel_map),
      dims(dims)
  {
  }

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
             bool indexed,
             const SH::ShProgramNodePtr& program);

  void operator()(SH::ShCtrlGraphNode* node);

private:
  ChannelMap& channel_map;
  SH::ShVariableNodePtr tc_node;
  bool indexed;
  SH::ShProgramNodePtr program;
};

void split_program(SH::ShProgramNode* program,
                   std::list<SH::ShProgramNodePtr>& programs,
                   const std::string& target);

}

#endif
