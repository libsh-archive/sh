// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#include "ShArb.hpp"
#include <map>
#include "sh.hpp"

namespace ShArb {

using namespace SH;

class StreamInputGatherer
{
public:
  StreamInputGatherer(std::map<ShChannelNodePtr, ShTextureNodePtr>& input_map)
    : input_map(input_map)
  {
  }
  
  void operator()(const ShCtrlGraphNode* node)
  {
    if (!node->block) return;
    
    for (ShBasicBlock::ShStmtList::const_iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      const ShStatement& stmt = *I;
      if (stmt.op != SH_OP_FETCH) continue;
      if (stmt.src[0].node()->kind() != SH_VAR_STREAM) continue;
      ShChannelNodePtr stream_node = stmt.src[0].node();
      input_map.insert(std::make_pair(stream_node, ShTextureNodePtr(0)));
    }
  }

private:
  std::map<ShChannelNodePtr, ShTextureNodePtr>& input_map;
};

void ArbBackend::execute(const ShProgram& program,
                         ShStream& dest)
{
  // TODO: Check program type
  
  if (dest.size() != 1) {
    SH_DEBUG_ERROR("ShArb cannot support multiple outputs for stream execution yet.");
    return;
  }

  std::map<ShChannelNodePtr, ShTextureNodePtr> input_map;

  // Do a DFS through the program's control graph.
  StreamInputGatherer gatherer(input_map);
  program->ctrlGraph->dfs(gatherer);

  if (input_map.empty()) {
    SH_DEBUG_ERROR("Stream program does not use any streams!");
    return;
  }

  // First, allocate textures for each input stream.
  // Need to ensure that input stream sizes are the same.
  int input_size = -1;
  int tex_size;
  for (std::map<ShChannelNodePtr, ShTextureNodePtr>::iterator I = input_map.begin();
       I != input_map.end(); ++I) {
    if (input_size < 0) {
      input_size = I->first->count();

      // Pick a size for the texture that just fits the data.
      // TODO: Verify this code!
      int k = 0;
      int n = input_size;
      while (n >= 2) {
        n >>= 2;
        k++;
      }
      tex_size = 2 << k;
    }
    if (input_size != I->first->count()) {
      SH_DEBUG_ERROR("Input lengths of stream program do not match ("
                     << input_size << " != " << I->first->count() << ")");
      return;
    }
    ShDataTextureNodePtr tex = new ShDataTextureNode(SH_TEXTURE_2D, tex_size, tex_size, 1,
                                                     I->first->size());
    ShDataMemoryObjectPtr mem = new ShDataMemoryObject(tex_size, tex_size, 1,
                                                       I->first->size());
    mem->setPartialData(reinterpret_cast<const float*>(I->first->data()), input_size);
    tex->setMem(mem);
    I->second = tex;
  }
  
  // Then, generate fragment program code from program
  //  - replace FETCH with TEX
  //  - need to pass in tex coord
  //     (can use combine for that)

  // Then, bind vertex (pass-through) and fragment program
  // Generate quad geometry
  // Render
  // Read back output
  // 
}

}
