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
      SH_DEBUG_ASSERT(false);
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
    
    Program prologue;
    switch (version & ~SINGLE_OUTPUT) {
    case OS_NONE_2D: {
      // Create the appropriate amount of InputData
      // (need to do this outside the program so that it is uniform)
      ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
      for (int i = 0; input != (*I)->end_inputs(); ++input, ++i) {
        if ((i % 4) == 0) {
          m_inputs[version].push_back(InputData());
        }
      }
      
      prologue = SH_BEGIN_PROGRAM(target) {
        InputAttrib2f DECL(streamcoord);

        InputList::iterator input_data = m_inputs[version].begin();
        for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
             input != (*I)->end_inputs(); ++input_data) {
          for (int i = 0; i < 4 && input != (*I)->end_inputs(); ++input, ++i) {
            Variable out((*input)->clone(SH_OUTPUT));
            input_data->tex[i] = new TextureNode(dims, (*input)->size(), 
                                                 (*input)->valueType(),
                                                 ArrayTraits(), 1, 1, 1);
            Variable tex(input_data->tex[i]);
            if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
              Statement stmt(out, tex, OP_TEXI, streamcoord);
              Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
            }
            else {
              Statement stmt(out, tex, OP_TEX, streamcoord);
              Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
            }
          }
        }
      } SH_END_PROGRAM;
      break;
    }
      
    case OS_NONE_3D:
      SH_DEBUG_ASSERT(false /* TODO */);
      break;
    
    case OS_1D: {
      // Create the appropriate amount of InputData
      // (need to do this outside the program so that it is uniform)
      ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
      for (int i = 0; input != (*I)->end_inputs(); ++input, ++i) {
        if ((i % 4) == 0) {
          m_inputs[version].push_back(InputData());
        }
      }
    
      prologue = SH_BEGIN_PROGRAM(target) {
        InputList::iterator data = m_inputs[version].begin();
        for (ProgramNode::VarList::const_iterator input = (*I)->begin_inputs();
             input != (*I)->end_inputs(); ++data) {
          InputAttrib4f DECL(input_index);
          Attrib4f DECL(index);
          
          // Wrap [0..count)
          index = frac(input_index);
          // (index * count * stride + offset) / width
          index = mad(index, data->uniform4[0], data->uniform4[1]);
          
          for (int i = 0; i < 2 && input != (*I)->end_inputs(); ++i) {

            TexCoord4f DECL(coord);
            // x = frac(index)
            coord(0,2) = frac(index(2*i,2*i+1));
            // y = index / width + bias
            coord(1,3) = mad(index(2*i,2*i+1), 
                             data->uniform2[i][0](0,1),
                             data->uniform2[i][0](2,3));
            // y -= x * 2 * bias
            coord(1,3) =  mad(coord(0,2),
                              data->uniform2[i][1](0,1),
                              coord(1,3));

            for (int j = 0; j < 2 && input != (*I)->end_inputs(); ++j, ++input) {

              data->tex[i*2+j] = new TextureNode(dims, (*input)->size(), 
                                                 (*input)->valueType(),
                                                 ArrayTraits(), 1, 1, 1);
              Variable tex(data->tex[i*2+j]);
              Variable out((*input)->clone(SH_OUTPUT));
            
              if (m_float_extension == ARB_NV_FLOAT_BUFFER) {
                coord(0,1) *= m_output_stride(3, 3);
                Statement stmt(out, tex, OP_TEXI, coord(2*j, 2*j+1));
                Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
              }
              else {
                Statement stmt(out, tex, OP_TEX, coord(2*j, 2*j+1));
                Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
              }
            }
          }
        }
      } SH_END_PROGRAM;
      break;
    }
/*      
    case OS_2D:
      prologue = SH_BEGIN_PROGRAM(target) {
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
*/         
    default:
      SH_DEBUG_ASSERT(false);
      break;
      
    }

    Program epilogue = SH_BEGIN_PROGRAM(target) {
      for (ProgramNode::VarList::const_iterator output = (*I)->begin_outputs();
           output != (*I)->end_outputs(); ++output) {

        Variable in((*output)->clone(SH_INPUT));
        if ((*output)->size() == 2) {
          Variable out((*output)->clone(SH_OUTPUT, 4));
          shASN(out, in(0, 1, 0, 1));
        }
        else {
          Variable out((*output)->clone());
          shASN(out, in);
        }
      }
    } SH_END_PROGRAM;

    Program prog = epilogue << Program(*I) << prologue;
    prog.meta("opengl:binding", "generic");
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
  SH_DEBUG_ASSERT(false);
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

  if ((version & ~SINGLE_OUTPUT) == OS_1D) {
    Stream::const_iterator input = input_stream.begin();
    for (InputList::iterator I = m_inputs[version].begin();
         input != input_stream.end(); ++I) {
      float uniform4[2][4];
      for (int i = 0; i < 2 && input != input_stream.end(); ++i) {
        float uniform2[2][4];
        for (int j = 0; j < 2 && input != input_stream.end(); ++j, ++input) {

          int stride, repeat, offset, count;
          input->get_stride(&stride, 1);
          input->get_repeat(&repeat, 1);
          input->get_offset(&offset, 1);
          input->get_count(&count, 1);

          // Find out and set the texture size
          int tex_size = 1;
          while (tex_size * tex_size < input->node()->width())
            tex_size <<= 1;
          I->tex[2*i+j]->setTexSize(tex_size, tex_size);
          I->tex[2*i+j]->memory(input->node()->memory(0), 0);
          
          // stride and offset calculation
          uniform4[0][2*i+j] = stride * count / (float)tex_size;
          uniform4[1][2*i+j] = offset / (float)tex_size;
          
          // 1D -> 2D conversion
          float bias = 1.0/(2*tex_size);
          uniform2[0][j]   = 1.0 / tex_size;
          uniform2[0][j+2] = bias;
          uniform2[1][j]   = -2 * bias;
        }
        I->uniform2[i][0].setValues(uniform2[0]);
        I->uniform2[i][1].setValues(uniform2[1]);
      }
      I->uniform4[0].setValues(uniform4[0]);
      I->uniform4[1].setValues(uniform4[1]);
    }
  }

  if ((version & ~SINGLE_OUTPUT) == OS_NONE_2D) {
    Stream::const_iterator input = input_stream.begin();
    for (InputList::iterator I = m_inputs[version].begin();
         input != input_stream.end(); ++I) {
      for (int i = 0; i < 4 && input != input_stream.end(); ++i, ++input) {
        if (input->node()->dims() == SH_TEXTURE_1D) {
          int tex_size = 1;
          while (tex_size * tex_size < input->node()->width())
            tex_size <<= 1;
          I->tex[i]->setTexSize(tex_size, tex_size);
        }
        else {
          I->tex[i]->setTexSize(input->node()->width(), input->node()->height());
        }
        I->tex[i]->memory(input->node()->memory(0), 0);
      }
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
  SH_DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  SH_DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].begin();
}

StreamCache::set_iterator StreamCache::sets_end(ProgramVersion version)
{
  SH_DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  SH_DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].end();
}

StreamCache::set_const_iterator StreamCache::sets_begin(ProgramVersion version) const
{
  SH_DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  SH_DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].begin();
}

StreamCache::set_const_iterator StreamCache::sets_end(ProgramVersion version) const
{
  SH_DEBUG_ASSERT(version >= 0 && version < NUM_PROGRAM_VERSIONS);
  SH_DEBUG_ASSERT(!m_program_sets[version].empty());
  return m_program_sets[version].end();
}

void split_program(ProgramNode* program,
                   std::list<ProgramNodePtr>& programs,
                   const std::string& target, int chunk_size)
{
  SH_DEBUG_ASSERT(chunk_size > 0);
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

