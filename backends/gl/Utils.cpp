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
#include "Utils.hpp"
#include "ShTextureNode.hpp"
#include "ShArray.hpp"
#include "sh.hpp"

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

    tex = new ShTextureNode(dims, channel->size(), channel->valueType(),
                            traits, 1, 1, 1, 0);
    tex->memory(channel->memory(), 0);

    channel_map.insert(std::make_pair(channel, tex));
  }
}

TexFetcher::TexFetcher(ChannelMap& channel_map,
                       const ShVariableNodePtr& tc_node,
                       bool indexed,
                       const ShProgramNodePtr& program)
    : channel_map(channel_map),
      tc_node(tc_node),
      indexed(indexed),
      program(program)
{
}

void TexFetcher::operator()(ShCtrlGraphNode* node)
{
  if (!node->block) return;
  for (ShBasicBlock::ShStmtList::iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    ShStatement& stmt = *I;
    if (stmt.op != SH_OP_FETCH && stmt.op != SH_OP_LOOKUP) continue;
      
    if (!stmt.src[0].node()) {
      SH_DEBUG_WARN("FETCH/LOOKUP from null stream");
      continue;
    }
    if (stmt.src[0].node()->kind() != SH_STREAM) {
      SH_DEBUG_WARN("FETCH/LOOKUP from non-stream");
      continue;
    }
      
    ShChannelNodePtr stream_node = shref_dynamic_cast<ShChannelNode>(stmt.src[0].node());
    ChannelMap::const_iterator J = channel_map.find(stream_node);
    if (J == channel_map.end()) {
      SH_DEBUG_WARN("Stream node " << stream_node.object() << ", " << stream_node->name() << " not found in input map");
      continue;
    }

    if (!J->second) {
      SH_DEBUG_WARN("No texture allocated for stream node");
      continue;
    }

    ShVariable texVar(J->second);

    if (stmt.op == SH_OP_FETCH) {
      ShVariable coordsVar(tc_node);
      if (indexed) {
        stmt = ShStatement(stmt.dest, texVar, SH_OP_TEXI, coordsVar);
      } else {
        stmt = ShStatement(stmt.dest, texVar, SH_OP_TEX, coordsVar);
      }
      // The following is useful for debugging
      // stmt = ShStatement(stmt.dest, SH_OP_ASN, coordsVar(0,1,0,1));
    } else {
      // Make sure our actualy index is a temporary in the program.
      ShContext::current()->enter(program);
      ShVariable coordsVar(new ShVariableNode(SH_TEMP, 2, SH_FLOAT));
      ShContext::current()->exit();
        
      ShBasicBlock::ShStmtList new_stmts;
      new_stmts.push_back(ShStatement(coordsVar(0), stmt.src[1], SH_OP_MOD, J->second->texSizeVar()(0)));
      new_stmts.push_back(ShStatement(coordsVar(1), stmt.src[1], SH_OP_DIV, J->second->texSizeVar()(0)));
      new_stmts.push_back(ShStatement(stmt.dest, texVar, SH_OP_TEXI, coordsVar));
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      I--;
    }
  }
}

void split_program(ShProgramNode* program,
                   std::list<ShProgramNodePtr>& programs,
                   const std::string& target)
{
  int i = 0;
  for (ShProgramNode::VarList::const_iterator I = program->begin_outputs();
       I != program->end_outputs(); ++I, ++i) {
    ShProgram p = shSwizzle(i) << ShProgram(program);
    p.node()->target() = target;
    programs.push_back(p.node());
  }
}

}
