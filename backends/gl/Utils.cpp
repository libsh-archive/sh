#include "Utils.hpp"
#include "ShTextureNode.hpp"
#include "ShArray.hpp"

namespace shgl {

using namespace SH;

void ChannelGatherer::operator()(const ShCtrlGraphNode* node)
{
  if (!node) return;

  ShBasicBlockPtr block = node->block;
  if (!block) return;

  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    const ShStatement& stmt = *I;
    if (stmt.op != SH_OP_FETCH) continue;

    // TODO: ought to complain here
    if (stmt.src[0].node()->kind() != SH_STREAM) continue;
    
    ShChannelNodePtr channel = shref_dynamic_cast<ShChannelNode>(stmt.src[0].node());

    ShTextureNodePtr tex;
    ShTextureTraits traits = ShArrayTraits();
    traits.clamping(ShTextureTraits::SH_UNCLAMPED);

    tex = new ShTextureNode(dims, channel->size(), channel->valueType(),
                            traits, 1, 1, 1, 0);
    tex->memory(channel->memory());

    channel_map.insert(std::make_pair(channel, tex));
  }
}

}
