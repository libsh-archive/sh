#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include "ShChannelNode.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShProgramNode.hpp"

namespace shgl {

typedef std::map<SH::ShChannelNodePtr, SH::ShTextureNodePtr> ChannelMap;

struct ChannelGatherer {
  ChannelGatherer(ChannelMap& channel_map, SH::ShTextureDims dims)
    : channel_map(channel_map),
      dims(dims)
  {
  }

  void operator()(const SH::ShCtrlGraphNode* node);
  
  ChannelMap channel_map;
  SH::ShTextureDims dims;
};


// Replace FETCH and LOOKUP operations with texture fetches
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

}

#endif
