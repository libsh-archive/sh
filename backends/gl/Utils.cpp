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
                         int max_outputs, FloatExtension float_extension)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program),
    m_max_outputs(max_outputs),
    m_float_extension(float_extension)
{
}

void StreamCache::generate_programs(ProgramVersion version)
{
  if (!m_program_sets[version].empty())
    return;

  TextureDims dims = SH_TEXTURE_2D;
  
  switch (version & ~SINGLE_OUTPUT) {
  case OS_NONE_2D:
  case OS_1D:
  case OS_2D:
    switch (m_float_extension) {
    case ARB_NV_FLOAT_BUFFER:
      dims = SH_TEXTURE_RECT;
      break;
    case ARB_ATI_PIXEL_FORMAT_FLOAT:
      dims = SH_TEXTURE_2D;
      break;
    default:
      DEBUG_ASSERT(false);
      break;
    }
    break;
  
  case OS_NONE_3D:
  case OS_3D:
    dims = SH_TEXTURE_3D;
    break;
  }
 
  split_program(m_stream_program, m_programs[version],
                get_target_backend(m_stream_program) + "fragment",
                (version & SINGLE_OUTPUT) ? 1 : m_max_outputs);

  for (std::list<ProgramNodePtr>::iterator I = m_programs[version].begin();
       I != m_programs[version].end(); ++I) {
         
    string target = get_target_backend(m_stream_program) + "fragment";

    for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
         input != (*I)->end_inputs(); ++input) {
      InputData input_data;
      input_data.tex =
        new TextureNode(dims, (*input)->size(), (*input)->valueType(),
                          ArrayTraits(), 1, 1, 1);
      m_inputs[version].push_back(input_data);
    }
    
    Program preamble;
    switch (version & ~SINGLE_OUTPUT) {
    case OS_NONE_2D:
      preamble = SH_BEGIN_PROGRAM(target) {
        InputTexCoord2f DECL(streamcoord);

        InputList::iterator input_data = m_inputs[version].begin();
        for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
             input != (*I)->end_inputs(); ++input, ++input_data) {
          Variable out((*input)->clone(SH_OUTPUT));
          Variable tex(input_data->tex);
          if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
            Statement stmt(out, tex, OP_TEXI, streamcoord);
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
          else {
            Statement stmt(out, tex, OP_TEX, streamcoord);
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
        }
      } SH_END_PROGRAM;
      break;
    
    case OS_NONE_3D:
      DEBUG_ASSERT(false /* TODO */);
      break;
    
    case OS_1D:
      preamble = SH_BEGIN_PROGRAM(target) {
        InputTexCoord4f DECL(streamcoord);
            
        Attrib4f DECL(index);
        // index = (x - bias, y - bias, -bias, 1-bias)
        index = streamcoord - m_output_offset(3,3,3,3);
        // z = (x + y*width + z*dest_offset/bias)/dest_stride/height
        index(2) = index(0,1,2) | m_output_offset(0,1,2);

        Attrib1f DECL(stride);
        // bias = 1/2 * 1/stride
        // stride = z*width*height + bias
        stride = mad(index(2), m_output_stride(0), m_output_stride(1));
        // if (stride - 2*bias < 0) discard
        discard(frac(stride) - m_output_stride(2));

        InputList::iterator input_data = m_inputs[version].begin();
        for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
             input != (*I)->end_inputs(); ++input, ++input_data) {

          TexCoord4f DECL(coord);
          Variable tex(input_data->tex);
          Variable out((*input)->clone(SH_OUTPUT));
          
          // The calculation we are doing here is:
          //
          // i = i*stride + offset + bias
          // x = frac(i)
          // y = i/width + (0.5 - x)*2*bias
          //
          // z = z*s + o + b
          // w = 1
          coord(2,3) = mad(index(2,3), input_data->os1(0,2), input_data->os1(1,3));
          // x = frac z
          coord(0) = frac(coord(2));
          // y = x*-2*b + y*0 + z/wi + w*b
          coord(1) = coord | input_data->os2;
          
          if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
            coord(0,1) *= m_output_stride(3, 3);
            Statement stmt(out, tex, OP_TEXI, coord(0,1));
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
          else {
            Statement stmt(out, tex, OP_TEX, coord(0,1));
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
        }
      } SH_END_PROGRAM;
      break;
      
    case OS_2D:
      preamble = SH_BEGIN_PROGRAM(target) {
        InputTexCoord2f DECL(streamcoord);

        TexCoord2f DECL(index);
        // (x, y) = ((x, y) - bias - offset) / stride
        index = mad(streamcoord, m_output_offset(0,1), m_output_offset(2,3));
        
        Attrib2f DECL(stride);
        // stride = index * size
        stride = index * m_output_stride(0, 1);
        Attrib4f DECL(disc);
        disc = (frac(stride) - m_output_stride(2, 3))(0,1,0,1);
        discard(disc);

        InputList::iterator input_data = m_inputs[version].begin();
        for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
             input != (*I)->end_inputs(); ++input, ++input_data) {
          Variable out((*input)->clone(SH_OUTPUT));
          Variable tex(input_data->tex);
          
          TexCoord2f DECL(coord);
          coord = mad(index, input_data->os1(0,1), input_data->os1(2,3));
          
          if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
            coord *= m_output_stride(0, 1);
            Statement stmt(out, tex, OP_TEXI, coord);
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
          else {
            Statement stmt(out, tex, OP_TEX, coord);
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
        }
      } SH_END_PROGRAM;
      break;
         
    default:
      DEBUG_ASSERT(false);
      break;
      
    }

    Program prog = Program(*I) << preamble;
    *I = prog.node();
  }

  for (std::list<ProgramNodePtr>::iterator I = m_programs[version].begin(); 
       I != m_programs[version].end(); ++I) {
    m_program_sets[version].push_back(new ProgramSet(Program(m_vertex_program),
                                                     Program(*I)));
  }
}

Info* StreamCache::clone() const
{
  DEBUG_ASSERT(false);
  return 0;
//  return new FBOStreamCache(m_stream_program, m_vertex_program, m_max_outputs);
}

void StreamCache::update_channels(ProgramVersion version, 
                                  const Stream& input_stream,
                                  const BaseTexture& dest_tex,
                                  int width, int height, int depth)
{
  if (m_program_sets[version].empty())
    generate_programs(version);

  DEBUG_ASSERT(stream.size() == m_inputs[version].size());
  InputList::iterator I = m_inputs[version].begin();
  Stream::const_iterator J = input_stream.begin();
  for (; I != m_inputs[version].end(); ++I, ++J) {
    I->tex->memory(J->node()->memory(0), 0);
    
    if (J->node()->dims() == SH_TEXTURE_1D) {
      int tex_size = 1;
      while (tex_size * tex_size < J->node()->width())
        tex_size <<= 1;
      I->tex->setTexSize(tex_size, tex_size);
    }
    else {
      I->tex->setTexSize(J->node()->width(), J->node()->height());
    }
    
    if ((version & ~SINGLE_OUTPUT) == OS_1D) {
      //
      // See TexFetcher::operator() for explanation of the two
      // offset/stride parameters
      //
      int stride, offset;
      J->get_stride(&stride, 1);
      J->get_offset(&offset, 1);
      float os1_val[4];
      float one_over_w = 1/(float)I->tex->width();
      os1_val[0] = stride*(width * height)*one_over_w;
      os1_val[1] = offset * one_over_w + one_over_w/2;
      os1_val[2] = 0;
      os1_val[3] = 1;
      I->os1.setValues(os1_val);

      float os2_val[4];
      os2_val[0] = -one_over_w;
      os2_val[1] = 0;
      os2_val[2] = one_over_w;
      os2_val[3] = one_over_w/2;
      I->os2.setValues(os2_val);
    }
    
    if ((version & ~SINGLE_OUTPUT) == OS_2D) {
      int stride[2], offset[2];
      J->get_stride(stride, 2);
      J->get_offset(offset, 2);
      
      float os_val[4];
      os_val[0] = stride[0];
      os_val[1] = stride[1];
      os_val[2] = offset[0] + 0.5;
      os_val[3] = offset[1] + 0.5;
      if (m_float_extension != ARB_NV_FLOAT_BUFFER) {
        os_val[2] /= J->node()->width();
        os_val[3] /= J->node()->height();
      }
      I->os1.setValues(os_val);
    } 
  }

  if ((version & ~SINGLE_OUTPUT) == OS_1D) {
    int stride, offset;
    dest_tex.get_stride(&stride, 1);
    dest_tex.get_offset(&offset, 1);
    if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
      float bias = 0.5;
      float z = (float)offset/width/height/stride/bias;
      m_output_offset = Attrib4f(1.0/width/height/stride, 1.0/height/stride, z, bias);
      m_output_stride = Attrib4f((float)width*height, 0.5/stride, 1.0/stride, width);
    }
    else {
      float bias = 1.0/(2*width);
      float z = (float)offset/width/height/stride/bias;
      m_output_offset = Attrib4f(1.0/height/stride, (float)width/height/stride, z, bias);
      m_output_stride = Attrib4f((float)width*height, 0.5/stride, 1.0/stride, width);
    }
  }
  
  if ((version & ~SINGLE_OUTPUT) == OS_2D) {
    int stride[2], offset[2];
    dest_tex.get_stride(stride, 2);
    dest_tex.get_offset(offset, 2);
    if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
      m_output_offset = Attrib4f(1/(float)stride[0], 1/(float)stride[1],
                                   -(offset[0] + 0.5)/stride[0],
                                   -(offset[1] + 0.5)/stride[1]);
      m_output_stride = Attrib4f(width, height, 
                                   0.5/stride[0], 0.5/stride[1]);
    }
    else {
      m_output_offset = Attrib4f(1/(float)stride[0], 1/(float)stride[1],
                                   -(offset[0] + 0.5)/width/stride[0],
                                   -(offset[1] + 0.5)/height/stride[1]);
      m_output_stride = Attrib4f(width, height,
                                   0.5/stride[0], 0.5/stride[1]);
    }
  }
}

StreamCache::set_iterator StreamCache::sets_begin(ProgramVersion version)
{
  DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].begin();
}

StreamCache::set_iterator StreamCache::sets_end(ProgramVersion version)
{
  DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].end();
}

StreamCache::set_const_iterator StreamCache::sets_begin(ProgramVersion version) const
{
  DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].begin();
}

StreamCache::set_const_iterator StreamCache::sets_end(ProgramVersion version) const
{
  DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].end();
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

