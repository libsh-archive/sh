#include "ShBaseTexture.hpp"

namespace SH {

ShBaseTexture::ShBaseTexture(const ShTextureNodePtr& node)
  : ShMetaForwarder(node.object()),
    m_node(node)
{
}

}
