// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
             std::list<SH::ShVariableNodePtr> &tc_node,
             bool indexed,
             const SH::ShProgramNodePtr& program);

  void operator()(SH::ShCtrlGraphNode* node);

private:
  ChannelMap& channel_map;
  std::list<SH::ShVariableNodePtr> &tc_node;
  std::list<SH::ShVariableNodePtr>::iterator tc_node_itr;
  bool indexed;
  SH::ShProgramNodePtr program;
};

void split_program(SH::ShProgramNode* program,
                   std::list<SH::ShProgramNodePtr>& programs,
                   const std::string& target);

}

class OffStrideGatherer {
public:
  typedef std::list< std::pair <int, int > > OffStrideList;
  typedef std::list< std::pair <int, int > >::iterator iterator;
  void operator()(SH::ShCtrlGraphNode* node);
  iterator begin() { return m_list.begin(); }
  iterator end() { return m_list.end(); }
private:
  //std::list< std::pair <int, int > > m_list;
  OffStrideList m_list; // (stride, offset) list
};


#endif
