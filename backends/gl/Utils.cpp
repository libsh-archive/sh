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
#include "TextureNode.hpp"
#include "Array.hpp"
#include "Optimizations.hpp"
#include "sh.hpp"

namespace shgl {

using namespace SH;
using namespace std;

void ChannelGatherer::operator()(const CtrlGraphNode* node)
{
  if (!node) return;

  BasicBlockPtr block = node->block;
  if (!block) return;

  for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
    const Statement& stmt = *I;
    if (stmt.op != OP_FETCH) continue;

    // TODO: ought to complain here
    if (stmt.src[0].node()->kind() != STREAM) continue;
    
    ChannelNodePtr channel = shref_dynamic_cast<ChannelNode>(stmt.src[0].node());

    TextureNodePtr tex;
    TextureTraits traits = ArrayTraits();

    tex = new TextureNode(dims, channel->size(), channel->valueType(),
                            traits, 1, 1, 1, 0);
    tex->memory(channel->memory(), 0);
    
    VariableNodePtr os1(new VariableNode(TEMP, 4, FLOAT));
    VariableNodePtr os2(new VariableNode(TEMP, 4, FLOAT));

    channel_map.insert(std::make_pair(channel, ChannelData(tex, os1, os2)));
  }
}

TexFetcher::TexFetcher(ChannelMap& channel_map,
                       const VariableNodePtr& tc_node,
                       bool indexed, bool os_calculation,
                       const VariableNodePtr& tex_size_node,
                       const ProgramNodePtr& program)
    : channel_map(channel_map),
      tc_node(tc_node),
      indexed(indexed),
      os_calculation(os_calculation),
      tex_size_node(tex_size_node),
      program(program)
{
}

void TexFetcher::operator()(CtrlGraphNode* node)
{
  if (!node->block) return;
  for (BasicBlock::StmtList::iterator I = node->block->begin();
       I != node->block->end(); ++I) {
    Statement& stmt = *I;
    if (stmt.op != OP_FETCH && stmt.op != OP_LOOKUP) continue;
      
    if (!stmt.src[0].node()) {
      DEBUG_WARN("FETCH/LOOKUP from null stream");
      continue;
    }
    if (stmt.src[0].node()->kind() != STREAM) {
      DEBUG_WARN("FETCH/LOOKUP from non-stream");
      continue;
    }
      
    ChannelNodePtr stream_node = shref_dynamic_cast<ChannelNode>(stmt.src[0].node());
    ChannelMap::const_iterator J = channel_map.find(stream_node);
    if (J == channel_map.end()) {
      DEBUG_WARN("Stream node " << stream_node.object() << ", " << stream_node->name() << " not found in input map");
      continue;
    }

    if (!J->second.tex_var) {
      DEBUG_WARN("No texture allocated for stream node");
      continue;
    }

    Variable tex_var(J->second.tex_var);
    Variable os1_var(J->second.os1_var);
    Variable os2_var(J->second.os2_var);

    if (stmt.op == OP_FETCH) {
      Variable coords_var(tc_node);
      if (os_calculation) {
        Context::current()->enter(program);
        Variable result(new VariableNode(TEMP, 4, FLOAT));
        Context::current()->exit();

        BasicBlock::StmtList new_stmts;

        //
        // The calculation we are doing here is:
        //
        // i = i*stride + offset + bias
        // x = frac(i)
        // y = i/width + (0.5 - x)*2*bias
        //
        // z = z*s + o + b
        // w = 1
        new_stmts.push_back(Statement(result(2,3), OP_MAD, coords_var(2,3), os1_var(0,2), os1_var(1,3)));
        // x = frac z
        new_stmts.push_back(Statement(result(0), OP_FRAC, result(2)));
        // y = x*-2*b + y*0 + z/wi + w*b
        new_stmts.push_back(Statement(result(1), result, OP_DOT, os2_var));

        if (indexed) {
          Variable tex_size_var(tex_size_node);
          new_stmts.push_back(Statement(result(0,1), result(0,1), OP_MUL, tex_size_var(2,2)));
          new_stmts.push_back(Statement(stmt.dest, tex_var, OP_TEXI, result(0,1)));
        }
        else {
          new_stmts.push_back(Statement(stmt.dest, tex_var, OP_TEX, result(0,1)));
        }

        I = node->block->erase(I);
        node->block->splice(I, new_stmts);
        I--;
      }
      else {
        if (indexed) {
          stmt = Statement(stmt.dest, tex_var, OP_TEXI, coords_var(0,1));
        }
        else {
          stmt = Statement(stmt.dest, tex_var, OP_TEX, coords_var(0,1));
        }
      }
    } else {
      // Make sure our actualy index is a temporary in the program.
      Context::current()->enter(program);
      Variable coordsVar(new VariableNode(TEMP, 2, FLOAT));
      Context::current()->exit();
        
      BasicBlock::StmtList new_stmts;
      new_stmts.push_back(Statement(coordsVar(0), stmt.src[1], OP_MOD, J->second.tex_var->texSizeVar()(0)));
      new_stmts.push_back(Statement(coordsVar(1), stmt.src[1], OP_DIV, J->second.tex_var->texSizeVar()(0)));
      new_stmts.push_back(Statement(stmt.dest, tex_var, OP_TEXI, coordsVar));
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      I--;
    }
  }
}

// Extract the backend name from the target if there is one (including the colon)
string get_target_backend(const ProgramNodeCPtr& program)
{
  const string& target = program->target();
  string::size_type colon_pos = target.find(":");

  if (target.npos == colon_pos) {
    return "";
  } else {
    return target.substr(0, colon_pos+1); // includes the colon
  }
}

StreamCache::StreamCache(ProgramNode* stream_program,
                         ProgramNode* vertex_program,
                         int tex_size, int max_outputs,
                         FloatExtension float_extension)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program),
    m_tex_size(tex_size),
    m_max_outputs(max_outputs),
    m_float_extension(float_extension)
{
  TextureDims dims(TEXTURE_2D);
  
  switch (m_float_extension) {
  case ARB_NV_FLOAT_BUFFER:
    dims = TEXTURE_RECT;
    break;
  case ARB_ATI_PIXEL_FORMAT_FLOAT:
    dims = TEXTURE_2D;
    break;
  default:
    DEBUG_ASSERT(false);
    break;
  }

  ChannelGatherer gatherer(m_channel_map, dims);
  stream_program->ctrlGraph->dfs(gatherer);

  for (int i = 0; i < NUM_SET_TYPES; ++i) {
    split_program(stream_program, m_programs[i],
                  get_target_backend(stream_program) + "fragment",
                  i == SINGLE_OUTPUT ? 1 : max_outputs);

    for (std::list<ProgramNodePtr>::iterator I = m_programs[i].begin();
         I != m_programs[i].end(); ++I) {
      Program with_tc = Program(*I) & lose<TexCoord4f>("streamcoord");
      TexFetcher fetcher(m_channel_map, with_tc.node()->inputs.back(),
                         dims == TEXTURE_RECT, 
                         i != NO_OFFSET_STRIDE,
                         m_output_stride.node(),
                         with_tc.node());
      with_tc.node()->ctrlGraph->dfs(fetcher);

      optimize(with_tc);
    
      if (i != NO_OFFSET_STRIDE) {
        string target = get_target_backend(stream_program) + "fragment";
        Program preamble = SH_BEGIN_PROGRAM(target) {
          InputTexCoord4f DECL(streamcoord);
          OutputTexCoord4f DECL(index);
          // index = (x - bias, y - bias, -bias, 1-bias)
          index = streamcoord - m_output_offset(3,3,3,3);
          // z = (x + y*width + z*dest_offset/bias)/dest_stride
          index(2) = index(0,1,2) | m_output_offset(0,1,2);

          Attrib1f DECL(stride);
          // stride = z*width
          stride = index(2) * m_output_stride(0);
          discard(frac(stride) - m_output_stride(1));
        } SH_END_PROGRAM;
        with_tc = with_tc << preamble;
      }
      
      *I = with_tc.node();
    }

    for (std::list<ProgramNodePtr>::iterator I = m_programs[i].begin(); 
         I != m_programs[i].end(); ++I) {
      m_program_sets[i].push_back(new ProgramSet(Program(vertex_program),
                                                   Program(*I)));
    }
  }

  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ChannelNode* channel = I->first.object();
    TextureNode* texture = I->second.tex_var.object();

    DEBUG_ASSERT(channel);
    DEBUG_ASSERT(texture);

    texture->memory(channel->memory(), 0);
    texture->setTexSize(tex_size, tex_size);
    texture->count(channel->count());
  }
}

Info* StreamCache::clone() const
{
  DEBUG_ASSERT(false);
  return 0;
//  return new FBOStreamCache(m_stream_program, m_vertex_program, m_max_outputs);
}

void StreamCache::update_destination(int dest_offset, int dest_stride)
{
  if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
    float bias = 0.5;
    float z = (float)dest_offset/m_tex_size/dest_stride/bias;
    m_output_offset = Attrib4f(1.0/m_tex_size/dest_stride, 
                                 1.0/dest_stride, z, bias);
    m_output_stride = Attrib3f((float)m_tex_size, 
                                 0.5/dest_stride, m_tex_size);
  }
  else {
    float bias = 1.0/(2*m_tex_size);
    float z = (float)dest_offset/m_tex_size/dest_stride/bias;
    m_output_offset = Attrib4f(1.0/dest_stride, 
                                 (float)m_tex_size/dest_stride, z, bias);
    m_output_stride = Attrib3f((float)m_tex_size,
                                 0.5/dest_stride, m_tex_size);
  }
}

void StreamCache::update_channels()
{
  for (ChannelMap::iterator I = m_channel_map.begin(); I != m_channel_map.end(); ++I) {
    ChannelNode* channel = I->first.object();
    TextureNode* texture = I->second.tex_var.object();

    DEBUG_ASSERT(channel);
    DEBUG_ASSERT(texture);
    
    // TODO: potentially increase the texture size
    texture->memory(channel->memory(), 0);

    Attrib4f os1(I->second.os1_var.object(), Swizzle(), false);
    Attrib4f os2(I->second.os2_var.object(), Swizzle(), false);

    //
    // See TexFetcher::operator() for explanation of the two
    // offset/stride parameters
    //
    float os1_val[4];
    float one_over_w = 1/(float)m_tex_size;
    os1_val[0] = (float)channel->stride();
    os1_val[1] = channel->offset() * one_over_w + one_over_w/2;
    os1_val[2] = 0;
    os1_val[3] = 1;
    os1.setValues(os1_val);

    float os2_val[4];
    os2_val[0] = -one_over_w;
    os2_val[1] = 0;
    os2_val[2] = one_over_w;
    os2_val[3] = one_over_w/2;
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
  DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].begin();
}

StreamCache::set_iterator StreamCache::sets_end(SetType type)
{
  DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].end();
}

StreamCache::set_const_iterator StreamCache::sets_begin(SetType type) const
{
  DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].begin();
}

StreamCache::set_const_iterator StreamCache::sets_end(SetType type) const
{
  DEBUG_ASSERT(type >= NO_OFFSET_STRIDE && type <= SINGLE_OUTPUT);
  return m_program_sets[type].end();
}

void split_program(ProgramNode* program,
                   std::list<ProgramNodePtr>& programs,
                   const std::string& target, int chunk_size)
{
  DEBUG_ASSERT(chunk_size > 0);
  int var = 0;
  for (ProgramNode::VarList::const_iterator I = program->begin_outputs();
       I != program->end_outputs(); ) {
    std::vector<int> chunk;
    for (int i = 0; i < chunk_size && I != program->end_outputs(); ++I,++i,++var){
      chunk.push_back(var);
    }
    Program p = swizzle(chunk) << Program(program);
    p.node()->target() = target;
    programs.push_back(p.node());
  }
}

}

