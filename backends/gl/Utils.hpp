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
