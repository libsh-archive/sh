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
#include "ShOptimizer.hpp"

namespace ShArb {

using namespace SH;

typedef std::map<ShChannelNodePtr, ShTextureNodePtr> StreamInputMap;

class StreamInputGatherer {
public:
  StreamInputGatherer(StreamInputMap& input_map)
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

      // TODO: ought to complain here
      if (stmt.src[0].node()->kind() != SH_VAR_STREAM) continue;

      ShChannelNodePtr stream_node = stmt.src[0].node();
      input_map.insert(std::make_pair(stream_node, ShTextureNodePtr(0)));
    }
  }

private:
  StreamInputMap& input_map;
};

class TexFetcher {
public:
  TexFetcher(StreamInputMap& input_map,
             ShVariableNodePtr tc_node)
    : input_map(input_map),
      tc_node(tc_node)
  {
  }

  void operator()(ShCtrlGraphNode* node)
  {
    ShVariable coordsVar(tc_node);
    if (!node->block) return;
    for (ShBasicBlock::ShStmtList::iterator I = node->block->begin();
         I != node->block->end(); ++I) {
      ShStatement& stmt = *I;
      if (stmt.op != SH_OP_FETCH) continue;
      
      if (stmt.src[0].node()->kind() != SH_VAR_STREAM) {
        // TODO: complain
        continue;
      }
      
      ShChannelNodePtr stream_node = stmt.src[0].node();
      StreamInputMap::const_iterator I = input_map.find(stream_node);
      if (I == input_map.end()) {
        // TODO: complain
        continue;
      }

      if (!I->second) {
        // TODO: complain
        continue;
      }

      ShVariable texVar(I->second);
      stmt = ShStatement(stmt.dest, texVar, SH_OP_TEX, coordsVar);
    }
  }
  
private:
  StreamInputMap& input_map;
  ShVariableNodePtr tc_node;
};

void ArbBackend::execute(const ShProgram& program,
                         ShStream& dest)
{
  // Check program target
  if (program->target() != "gpu:stream") {
    SH_DEBUG_ERROR("ShArb can only execute ``gpu:stream'' programs.");
    return;
  }

  // Make sure program has no inputs
  if (!program->inputs.empty()) {
    SH_DEBUG_ERROR("Stream program has unbound inputs, and can hence not be executed.");
    return;
  }

  // Only support one output channel right now.
  if (dest.size() != 1) {
    SH_DEBUG_ERROR("ShArb cannot support multiple output channels for stream execution yet.");
    return;
  }

  ShChannelNodePtr output = *dest.begin();

  StreamInputMap input_map;

  // Do a DFS through the program's control graph.
  StreamInputGatherer gatherer(input_map);
  program->ctrlGraph->dfs(gatherer);

  if (input_map.empty()) {
    SH_DEBUG_ERROR("Stream program does not use any streams!");
    return;
  }

  // First, allocate textures for each input stream.
  // Need to ensure that input stream sizes are the same.
  int input_count = -1;
  int tex_size;
  for (StreamInputMap::iterator I = input_map.begin(); I != input_map.end(); ++I) {
    if (input_count < 0) {
      input_count = I->first->count();

      // Pick a size for the texture that just fits the data.
      int t = 1;
      while (t * t < input_count) {
        t <<= 1;
      }
      tex_size = t;
    }
    if (input_count != I->first->count()) {
      SH_DEBUG_ERROR("Input lengths of stream program do not match ("
                     << input_count << " != " << I->first->count() << ")");
      return;
    }
    ShDataTextureNodePtr tex = new ShDataTextureNode(SH_TEXTURE_2D, tex_size, tex_size, 1,
                                                     I->first->size());
    ShDataMemoryObjectPtr mem = new ShDataMemoryObject(tex_size, tex_size, 1,
                                                       I->first->size());
    mem->setPartialData(reinterpret_cast<const float*>(I->first->data()), input_count);
    tex->setMem(mem);
    I->second = tex;
  }

  if (output->count() != input_count) {
    SH_DEBUG_ERROR("Input data count does not match output data count ("
                   << input_count << " != " << output->count() << ")");
    return;
  }

  /*
  SH_DEBUG_PRINT("Debugging info:" << std::endl <<
                 "input_count = " << input_count << std::endl <<
                 "tex_size   = " << tex_size);
  */

  // Add in the texcoord variable
  ShProgram fp = program & keep<ShTexCoord2f>();

  // Make it a fragment program
  fp->target() = "gpu:fragment";
  
  ShVariableNodePtr tc_node = fp->inputs.back(); // there should be only one input anyways

  ShBackendPtr arbBackend(this);

  // replace FETCH with TEX
  TexFetcher texFetcher(input_map, tc_node);
  fp->ctrlGraph->dfs(texFetcher);
  fp->collectVariables();
  
  // optimize
  ShOptimizer optimizer(fp->ctrlGraph);
  optimizer.optimize(ShEnvironment::optimizationLevel);

  
  // generate code
  fp->compile(arbBackend);

  {
    SH_DEBUG_PRINT("Optimized FP:");
    fp->code(arbBackend)->print(std::cerr);
  }
  
  // The (trivial) vertex program
  ShProgram vp = keep<ShPosition4f>() & keep<ShTexCoord2f>();
  vp->target() = "gpu:vertex";
  vp->compile(arbBackend);

  // Then, bind vertex (pass-through) and fragment program
  fp->code(arbBackend)->bind();
  vp->code(arbBackend)->bind();
  
  // TODO: pbuffers stuff

  // Generate quad geometry
  /*
  glBegin(GL_QUADS); {
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(-1.0,  1.0, 0.0);
    glVertex3f( 1.0,  1.0, 0.0);
    glVertex3f( 1.0, -1.0, 0.0);
  } glEnd();
  */
  
  // Render
  // Read back output
}

}
