#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include "ShChannelNode.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"

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

}

#endif
