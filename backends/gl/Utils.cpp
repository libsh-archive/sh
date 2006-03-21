// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include "ShOptimizations.hpp"
#include "sh.hpp"

namespace shgl {

using namespace SH;
using namespace std;

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
    ShTextureTraits traits(0, ShTextureTraits::SH_FILTER_NONE, 
                           ShTextureTraits::SH_WRAP_REPEAT);

    tex = new ShTextureNode(dims, channel->size(), channel->valueType(),
                            traits, 1, 1, 1, 0);
    tex->memory(channel->memory(), 0);
    
    ShVariableNodePtr os1(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));
    ShVariableNodePtr os2(new ShVariableNode(SH_TEMP, 4, SH_FLOAT));

    channel_map.insert(std::make_pair(channel, ChannelData(tex, os1, os2)));
  }
}

TexFetcher::TexFetcher(ChannelMap& channel_map,
                       const ShVariableNodePtr& tc_node,
                       bool indexed, bool os_calculation,
                       const ShProgramNodePtr& program)
    : channel_map(channel_map),
      tc_node(tc_node),
      indexed(indexed),
      os_calculation(os_calculation),
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

    if (!J->second.tex_var) {
      SH_DEBUG_WARN("No texture allocated for stream node");
      continue;
    }

    ShVariable tex_var(J->second.tex_var);
    ShVariable os1_var(J->second.os1_var);
    ShVariable os2_var(J->second.os2_var);

    if (stmt.op == SH_OP_FETCH) {
      ShVariable coords_var(tc_node);
      if (os_calculation) {
        ShContext::current()->enter(program);
        ShVariable result(new ShVariableNode(SH_TEMP, 3, SH_FLOAT));
        ShContext::current()->exit();

        ShBasicBlock::ShStmtList new_stmts;

        //
        // The calculation we are doing here is:
        //
        // x = (((y-bias)*width + (x-bias)) * stride + offset) % width + bias
        // y = (((y-bias)*width + (x-bias)) * stride + offset) / width + bias
        //
        // First subtract bias from the coordinates from the interpolation
        // (x,y,z) = (x - bias, y - bias, 0 - bias)
        //
        new_stmts.push_back(ShStatement(result(0,1,2), coords_var(0,1,2), SH_OP_ADD, os2_var(3,3,3)));
        //
        // y = x*stride + y*(width*stride) + z*(offset*-bias)
        // (note that z = -bias)
        //
        new_stmts.push_back(ShStatement(result(1), result(0,1,2), SH_OP_DOT, os2_var(0,1,2)));
        //
        // the integer part of y is now the actual y value (0 to tex size)
        // y = floor(y)
        //
        new_stmts.push_back(ShStatement(result(1), SH_OP_FLR, result(1)));
        //
        // (x, y) = (x*stride + offset%width + bias, y/width + bias)
        //
        new_stmts.push_back(ShStatement(result(0,1), SH_OP_MAD, result(0,1), os1_var(0,2), os1_var(1,3)));
        //
        // and finally look up the texel
        //
        new_stmts.push_back(ShStatement(stmt.dest, tex_var, SH_OP_TEX, result(0,1)));

        I = node->block->erase(I);
        node->block->splice(I, new_stmts);
        I--;
      }
      else {
        stmt = ShStatement(stmt.dest, tex_var, SH_OP_TEX, coords_var(0,1));
      }
    } else {
      // Make sure our actualy index is a temporary in the program.
      ShContext::current()->enter(program);
      ShVariable coordsVar(new ShVariableNode(SH_TEMP, 2, SH_FLOAT));
      ShContext::current()->exit();
        
      ShBasicBlock::ShStmtList new_stmts;
      new_stmts.push_back(ShStatement(coordsVar(0), stmt.src[1], SH_OP_MOD, J->second.tex_var->texSizeVar()(0)));
      new_stmts.push_back(ShStatement(coordsVar(1), stmt.src[1], SH_OP_DIV, J->second.tex_var->texSizeVar()(0)));
      new_stmts.push_back(ShStatement(stmt.dest, tex_var, SH_OP_TEXI, coordsVar));
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      I--;
    }
  }
}

// Extract the backend name from the target if there is one (including the colon)
string get_target_backend(const ShProgramNodeCPtr& program)
{
  const string& target = program->target();
  string::size_type colon_pos = target.find(":");

  if (target.npos == colon_pos) {
    return "";
  } else {
    return target.substr(0, colon_pos+1); // includes the colon
  }
}

StreamCache::StreamCache(ShProgramNode* stream_program,
                         ShProgramNode* vertex_program,
                         int tex_size, int max_outputs)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program),
    m_tex_size(tex_size),
    m_max_outputs(max_outputs)
{
  ShTextureDims dims(SH_TEXTURE_2D);

  ChannelGatherer gatherer(m_channel_map, dims);
  stream_program->ctrlGraph->dfs(gatherer);

  for (int i = 0; i < NUM_SET_TYPES; ++i) {
    split_program(stream_program, m_programs[i],
                  get_target_backend(stream_program) + "fragment",
                  i == SINGLE_OUTPUT ? 1 : max_outputs);

    for (std::list<ShProgramNodePtr>::iterator I = m_programs[i].begin();
         I != m_programs[i].end(); ++I) {
      ShProgram with_tc = ShProgram(*I) & lose<ShTexCoord3f>("streamcoord");
  
      TexFetcher fetcher(m_channel_map, with_tc.node()->inputs.back(),
                         dims == SH_TEXTURE_RECT, 
                         i != NO_OFFSET_STRIDE,
                         with_tc.node());
      with_tc.node()->ctrlGraph->dfs(fetcher);

      optimize(with_tc);
    
      *I = with_tc.node();
    }

    for (std::list<ShProgramNodePtr>::iterator I = m_programs[i].begin(); 
         I != m_programs[i].end(); ++I) {
      m_program_sets[i].push_back(new ShProgramSet(ShProgram(vertex_program),
                                                   ShProgram(*I)));
    }
  }

  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ShChannelNode* channel = I->first.object();
    ShTextureNode* texture = I->second.tex_var.object();

    SH_DEBUG_ASSERT(channel);
    SH_DEBUG_ASSERT(texture);

    texture->memory(channel->memory(), 0);
    texture->setTexSize(tex_size, tex_size);
    texture->count(channel->count());
  }
}

ShInfo* StreamCache::clone() const
{
  SH_DEBUG_ASSERT(false);
  return 0;
//  return new FBOStreamCache(m_stream_program, m_vertex_program, m_max_outputs);
}

void StreamCache::update_channels()
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ShChannelNode* channel = I->first.object();
    ShTextureNode* texture = I->second.tex_var.object();

    SH_DEBUG_ASSERT(channel);
    SH_DEBUG_ASSERT(texture);
    
    // TODO: potentially increase the texture size
    texture->memory(channel->memory(), 0);

    ShAttrib4f os1(I->second.os1_var.object(), ShSwizzle(), false);
    ShAttrib4f os2(I->second.os2_var.object(), ShSwizzle(), false);

    //
    // See TexFetcher::operator() for explanation of the two
    // offset/stride parameters
    //
    float os1_val[4];
    float one_over_w = 1/(float)m_tex_size;
    os1_val[0] = channel->stride();
    os1_val[1] = channel->offset()*one_over_w - 
                 floor(channel->offset()*one_over_w) + one_over_w/2;
    os1_val[2] = one_over_w;
    os1_val[3] = one_over_w/2;
    os1.setValues(os1_val);

    float os2_val[4];
    os2_val[0] = channel->stride();
    os2_val[1] = m_tex_size*channel->stride();
    os2_val[2] = -2*channel->offset();
    os2_val[3] = -one_over_w/2;
    os2.setValues(os2_val);
  }
}

void StreamCache::freeze_inputs(bool state)
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    I->second.tex_var->memory(0)->freeze(state);
  }
}

StreamCache::set_iterator StreamCache::sets_begin(SetType type)
{
  SH_DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].begin();
}

StreamCache::set_iterator StreamCache::sets_end(SetType type)
{
  SH_DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].end();
}

StreamCache::set_const_iterator StreamCache::sets_begin(SetType type) const
{
  SH_DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].begin();
}

StreamCache::set_const_iterator StreamCache::sets_end(SetType type) const
{
  SH_DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].end();
}

void split_program(ShProgramNode* program,
                   std::list<ShProgramNodePtr>& programs,
                   const std::string& target, int chunk_size)
{
  SH_DEBUG_ASSERT(chunk_size > 0);
  int var = 0;
  for (ShProgramNode::VarList::const_iterator I = program->begin_outputs();
       I != program->end_outputs(); ) {
    std::vector<int> chunk;
    for (int i = 0; i < chunk_size && I != program->end_outputs(); ++I,++i,++var){
      chunk.push_back(var);
    }
    ShProgram p = shSwizzle(chunk) << ShProgram(program);
    p.node()->target() = target;
    programs.push_back(p.node());
  }
}

}

