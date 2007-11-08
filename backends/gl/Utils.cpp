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

Program SplitProgram::epilogue(const ProgramNodePtr& program)
{
  Program result = SH_BEGIN_PROGRAM() {
    for (ProgramNode::VarList::const_iterator output = program->begin_outputs();
         output != program->end_outputs(); ++output) {

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
  return result;
}

void SplitProgram::update_uniforms(const Record& input_uniforms)
{
  Record::const_iterator input = input_uniforms.begin();
  std::vector<Variable>::iterator uniform = m_uniforms.begin();
  for(; input != input_uniforms.end(); ++input, ++uniform) {
    SH_DEBUG_ASSERT(uniform != m_uniforms.end());
    shASN(*uniform, *input);
  }
}

class SplitProgram2D : public SplitProgram {
public:
  SplitProgram2D(FloatExtension float_extension,
                 const Program::BindingSpec& binding_spec,
                 const Program& vertex_program,
                 const ProgramNodePtr& program);
  void update_channels(const Stream& stream, const BaseTexture& dest_tex);
  ProgramSetPtr program_set() { return m_program_set; }
private:
  std::vector<TextureNodePtr> m_channels;
  ProgramSetPtr m_program_set;
};

SplitProgram2D::SplitProgram2D(FloatExtension float_extension,
                               const Program::BindingSpec& binding_spec,
                               const Program& vertex_program,
                               const ProgramNodePtr& program)
{
  // Reserve enough data outside the program so that it is uniform
  Program::BindingSpec::const_iterator binding = binding_spec.begin();
  ProgramNode::VarList::const_iterator input = program->begin_inputs();
  for (; binding != binding_spec.end(); ++binding, ++input) {
    SH_DEBUG_ASSERT(input != program->end_inputs());
    if (*binding == Program::UNIFORM) {
      m_uniforms.push_back(Variable((*input)->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, false, false)));
    }
  }

  TextureDims dims = SH_TEXTURE_2D;
  if (float_extension == ARB_NV_FLOAT_BUFFER) {
    dims = SH_TEXTURE_RECT;
  }

  Program prologue = SH_BEGIN_PROGRAM() {
    InputAttrib2f SH_DECL(streamcoord);

    std::vector<Variable>::iterator uniform = m_uniforms.begin();
    binding = binding_spec.begin();
    input = program->begin_inputs();
    for (; binding != binding_spec.end(); ++binding, ++input) {
      SH_DEBUG_ASSERT(input != program->end_inputs());
      Variable out((*input)->clone(SH_OUTPUT));
      if (*binding == Program::UNIFORM) {
        shASN(out, *uniform);
        ++uniform;
      }
      else {
        TextureNodePtr tex_node = new TextureNode(dims, (*input)->size(), 
                                                  (*input)->valueType(),
                                                  ArrayTraits(), 1, 1, 1);
        m_channels.push_back(tex_node);
        Variable tex(tex_node);
        stringstream name;
        name << "input.channel[" << m_channels.size()-1 << "]";
        tex.name(name.str());
            
        if (float_extension == ARB_NV_FLOAT_BUFFER) {
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

  Program fragment_program = epilogue(program) << Program(program) << prologue;
  fragment_program.meta("opengl:binding", "generic");  
  fragment_program.node()->name(program->name());
  m_program_set = new ProgramSet(vertex_program, fragment_program);
}

void SplitProgram2D::update_channels(const Stream& input_stream,
                                     const BaseTexture& dest_tex)
{
  Stream::const_iterator input = input_stream.begin();
  std::vector<TextureNodePtr>::iterator channel = m_channels.begin();
  for (; input != input_stream.end(); ++input, ++channel) {
    SH_DEBUG_ASSERT(channel != m_channels.end());
    if (input->node()->dims() == SH_TEXTURE_1D) {
      int tex_size = 1;
      while (tex_size * tex_size < input->node()->width())
        tex_size <<= 1;
      (*channel)->setTexSize(tex_size, tex_size);
    }
    else {
      (*channel)->setTexSize(input->node()->width(), input->node()->height());
    }
    (*channel)->memory(input->node()->memory(0), 0);
  }
}

class SplitProgram1DRecalculate : public SplitProgram {
public:
  SplitProgram1DRecalculate(FloatExtension float_extension,
                            const Program::BindingSpec& binding_spec,
                            const Program& vertex_program,
                            const ProgramNodePtr& program);
  void update_channels(const Stream& stream, const BaseTexture& dest_tex);
  ProgramSetPtr program_set() { return m_program_set; }
private:
  struct ChannelData {
    TextureNodePtr tex[4];
    Attrib4f uniform4[2];
    Attrib4f uniform2[2][2];
  };
  std::vector<ChannelData> m_channels;
  Attrib2f m_dest_size;
  ProgramSetPtr m_program_set;
};

SplitProgram1DRecalculate::SplitProgram1DRecalculate(FloatExtension float_extension,
                                                     const Program::BindingSpec& binding_spec,
                                                     const Program& vertex_program,
                                                     const ProgramNodePtr& program)
{
  // Reserve enough data outside the program so that it is uniform
  Program::BindingSpec::const_iterator binding = binding_spec.begin();
  ProgramNode::VarList::const_iterator input = program->begin_inputs();
  for (int channel = 0; binding != binding_spec.end(); ++binding, ++input) {
    if (*binding == Program::STREAM) {
      if ((channel % 4) == 0) {
        m_channels.push_back(ChannelData());
      }
      ++channel;
    }
    else {
      m_uniforms.push_back(Variable((*input)->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, false, false)));
    }          
  }

  TextureDims dims = SH_TEXTURE_2D;
  if (float_extension == ARB_NV_FLOAT_BUFFER) {
    dims = SH_TEXTURE_RECT;
  }

  Program prologue = SH_BEGIN_PROGRAM() {
    std::vector<ChannelData>::iterator channel_data = m_channels.begin();
    std::vector<Variable>::iterator uniform = m_uniforms.begin();
    Program::BindingSpec::const_iterator binding = binding_spec.begin();
    ProgramNode::VarList::const_iterator input = program->begin_inputs();
    for (; input != program->end_inputs(); ++channel_data) {
      InputAttrib4f SH_DECL(input_index);
      Attrib4f SH_DECL(index);
          
      // Wrap [0..count)
      index = frac(input_index);
      // (index * count * stride + offset) / width
      index = mad(index, channel_data->uniform4[0], channel_data->uniform4[1]);
          
      for (int i = 0; i < 2 && input != program->end_inputs(); ++i) {

        TexCoord4f SH_DECL(coord);
        // x = frac(index)
        coord(0,2) = frac(index(2*i,2*i+1));
        // y = index / width + bias
        coord(1,3) = mad(index(2*i,2*i+1), 
                         channel_data->uniform2[i][0](0,1),
                         channel_data->uniform2[i][0](2,3));
        // y -= x * 2 * bias
        coord(1,3) =  mad(coord(0,2),
                          channel_data->uniform2[i][1](0,1),
                          coord(1,3));

        for (int j = 0; j < 2 && input != program->end_inputs(); ++input, ++binding) {
          Variable out((*input)->clone(SH_OUTPUT));
          if (*binding == Program::UNIFORM) {
            shASN(out, *uniform);
            ++uniform;
          }
          else {
            channel_data->tex[i*2+j] = new TextureNode(dims, (*input)->size(), 
                                                       (*input)->valueType(),
                                                       ArrayTraits(), 1, 1, 1);
            Variable tex(channel_data->tex[i*2+j]);            
            stringstream name;
            name << "stream.input[" << i*2+j << "]";
            tex.name(name.str());
                
            if (float_extension == ARB_NV_FLOAT_BUFFER) {
              coord(0,1) *= m_dest_size;
              Statement stmt(out, tex, OP_TEXI, coord(2*j, 2*j+1));
              Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
            }
            else {
              Statement stmt(out, tex, OP_TEX, coord(2*j, 2*j+1));
              Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
            }
            ++j;
          }
        }
      }
    }
  } SH_END_PROGRAM;

  Program fragment_program = epilogue(program) << Program(program) << prologue;
  fragment_program.meta("opengl:binding", "generic");  
  fragment_program.node()->name(program->name());
  m_program_set = new ProgramSet(vertex_program, fragment_program);
}

void SplitProgram1DRecalculate::update_channels(const Stream& input_stream,
                                                const BaseTexture& dest_tex)
{
  Stream::const_iterator input = input_stream.begin();
  std::vector<ChannelData>::iterator channel_data = m_channels.begin();
  for (; input != input_stream.end(); ++channel_data) {
 
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
        channel_data->tex[2*i+j]->setTexSize(tex_size, tex_size);
        channel_data->tex[2*i+j]->memory(input->node()->memory(0), 0);
          
        // stride and offset calculation
        uniform4[0][2*i+j] = stride * count / (float)tex_size - stride/(2.0*tex_size);
        uniform4[1][2*i+j] = offset / (float)tex_size;
          
        // 1D -> 2D conversion
        float bias = 1.0/(2*tex_size);
        uniform2[0][j]   = 1.0 / tex_size;
        uniform2[0][j+2] = bias;
        uniform2[1][j]   = -2 * bias;
      }
      channel_data->uniform2[i][0].setValues(uniform2[0]);
      channel_data->uniform2[i][1].setValues(uniform2[1]);
    }
    channel_data->uniform4[0].setValues(uniform4[0]);
    channel_data->uniform4[1].setValues(uniform4[1]);
  }
  m_dest_size[0] = dest_tex.node()->width();
  m_dest_size[1] = dest_tex.node()->height();
}

class SplitProgram2DRecalculate : public SplitProgram {
public:
  SplitProgram2DRecalculate(FloatExtension float_extension,
                            const Program::BindingSpec& binding_spec,
                            const Program& vertex_program,
                            const ProgramNodePtr& program);
  void update_channels(const Stream& stream, const BaseTexture& dest_tex);
  ProgramSetPtr program_set() { return m_program_set; }
private:
  struct ChannelData {
    TextureNodePtr tex[2];
    Attrib4f uniform[2];
  };
  std::vector<ChannelData> m_channels;
  FloatExtension m_float_extension;
  ProgramSetPtr m_program_set;
};

SplitProgram2DRecalculate::SplitProgram2DRecalculate(FloatExtension float_extension,
                                                     const Program::BindingSpec& binding_spec,
                                                     const Program& vertex_program,
                                                     const ProgramNodePtr& program)
 : m_float_extension(float_extension)
{
  // Reserve enough data outside the program so that it is uniform
  Program::BindingSpec::const_iterator binding = binding_spec.begin();
  ProgramNode::VarList::const_iterator input = program->begin_inputs();
  for (int channel = 0; binding != binding_spec.end(); ++binding, ++input) {
    if (*binding == Program::STREAM) {
      if ((channel % 2) == 0) {
        m_channels.push_back(ChannelData());
      }
      ++channel;
    }
    else {
      m_uniforms.push_back(Variable((*input)->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, false, false)));
    }          
  }

  TextureDims dims = SH_TEXTURE_2D;
  if (float_extension == ARB_NV_FLOAT_BUFFER) {
    dims = SH_TEXTURE_RECT;
  }

  Program prologue = SH_BEGIN_PROGRAM() {
    std::vector<ChannelData>::iterator channel_data = m_channels.begin();
    std::vector<Variable>::iterator uniform = m_uniforms.begin();
    Program::BindingSpec::const_iterator binding = binding_spec.begin();
    ProgramNode::VarList::const_iterator input = program->begin_inputs();
    for (; input != program->end_inputs(); ++channel_data) {
      InputAttrib4f SH_DECL(input_index);
      Attrib4f SH_DECL(index);

      // Wrap [0..count)
      index = frac(input_index);
      // (index * count * stride + offset) / width
      index = mad(index, channel_data->uniform[0], channel_data->uniform[1]);
          
      for (int i = 0; i < 2 && input != program->end_inputs(); ++input, ++binding) {

        Variable out((*input)->clone(SH_OUTPUT));
        if (*binding == Program::UNIFORM) {
          shASN(out, *uniform);
          ++uniform;
        }
        else {
          channel_data->tex[i] = new TextureNode(dims, (*input)->size(), 
                                                 (*input)->valueType(),
                                                 ArrayTraits(), 1, 1, 1);
          Variable tex(channel_data->tex[i]);
          stringstream name;
          name << "stream.input[" << i << "]";
          tex.name(name.str());

          if (float_extension == ARB_NV_FLOAT_BUFFER) {
            Statement stmt(out, tex, OP_TEXI, index(2*i, 2*i+1));
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
          else {
            Statement stmt(out, tex, OP_TEX, index(2*i, 2*i+1));
            Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
          }
          ++i;
        }
      }
    }
  } SH_END_PROGRAM;

  Program fragment_program = epilogue(program) << Program(program) << prologue;
  fragment_program.meta("opengl:binding", "generic");  
  fragment_program.node()->name(program->name());
  m_program_set = new ProgramSet(vertex_program, fragment_program);
}

void SplitProgram2DRecalculate::update_channels(const Stream& input_stream,
                                                const BaseTexture& dest_tex)
{
  Stream::const_iterator input = input_stream.begin();
  std::vector<ChannelData>::iterator channel_data = m_channels.begin();
  for (; input != input_stream.end(); ++channel_data) {
 
    float uniform[2][4];
    for (int i = 0; i < 2 && input != input_stream.end(); ++i, ++input) {

      int stride[2], repeat[2], offset[2], count[2];
      input->get_stride(stride, 2);
      input->get_repeat(repeat, 2);
      input->get_offset(offset, 2);
      input->get_count(count, 2);

      channel_data->tex[i]->setTexSize(input->node()->width(), input->node()->height());
      channel_data->tex[i]->memory(input->node()->memory(0), 0);
      
      int width = (m_float_extension == ARB_NV_FLOAT_BUFFER ? 1 : input->node()->width());
      int height = (m_float_extension == ARB_NV_FLOAT_BUFFER ? 1 : input->node()->height());
      
      // stride and offset calculation
      uniform[0][2*i+0] = stride[0] * count[0] / (float)width;
      uniform[0][2*i+1] = stride[1] * count[1] / (float)height;
      uniform[1][2*i+0] = offset[0] / (float)width + 
        1.0/(2*dest_tex.node()->width()) - stride[0]/(2.0*dest_tex.node()->width());
      uniform[1][2*i+1] = offset[1] / (float)height +
        1.0/(2*dest_tex.node()->height()) - stride[1]/(2.0*dest_tex.node()->height());
    }
    channel_data->uniform[0].setValues(uniform[0]);
    channel_data->uniform[1].setValues(uniform[1]);
  }
}

ProgramVersionCache::ProgramVersionCache(FloatExtension float_extension,
                                         int max_outputs,
                                         const Program& vertex_program,
                                         const ProgramVersion& version,
                                         const Program::BindingSpec& binding_spec,
                                         ProgramNode* program)
{
  std::list<ProgramNodePtr> split_programs;
  split_program(program, split_programs,
                get_target_backend(program) + "fragment",
                (version.single_output) ? 1 : max_outputs);

  std::list<ProgramNodePtr>::const_iterator I;
  for (I = split_programs.begin(); I != split_programs.end(); ++I) {
    if (version.dimension < 3 && !version.index_recalculation) {
      m_split_program.push_back(new SplitProgram2D(float_extension, binding_spec,
                                                   vertex_program, *I));
    }
    else if (version.dimension == 1 && version.index_recalculation) {
      m_split_program.push_back(new SplitProgram1DRecalculate(float_extension, binding_spec,
                                                              vertex_program, *I));
    }
    else if (version.dimension == 2 && version.index_recalculation) {
      m_split_program.push_back(new SplitProgram2DRecalculate(float_extension, binding_spec,
                                                              vertex_program, *I));
    }
    else {
      assert(!"Stream execution style not implemented yet");
    }
  }
}

ProgramVersionCache::~ProgramVersionCache()
{
  for(iterator I = m_split_program.begin(); I != m_split_program.end(); ++I) {
    delete *I;
  }
}

void ProgramVersionCache::split_program(ProgramNode* program,
                                        std::list<ProgramNodePtr>& split_programs,
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
    p.node()->name(program->name());
    split_programs.push_back(p.node());
  }
}

StreamCache::StreamCache(ProgramNode* stream_program,
                         ProgramNodePtr vertex_program,
                         int max_outputs, FloatExtension float_extension)
  : m_stream_program(stream_program),
    m_vertex_program(vertex_program),
    m_max_outputs(max_outputs),
    m_float_extension(float_extension)
{
}

StreamCache::~StreamCache()
{
  for (Cache::iterator I = m_cache.begin(); I != m_cache.end(); ++I) {
    delete I->second;
  }
}

const ProgramVersionCache& 
StreamCache::get_program_cache(const ProgramVersion& version, 
                               const SH::Program::BindingSpec& binding_spec)
{
  Key key(version, binding_spec);
  if (m_cache.find(key) == m_cache.end()) {
    m_cache[key] = new ProgramVersionCache(m_float_extension, m_max_outputs,
                                           Program(m_vertex_program),
                                           version, binding_spec, m_stream_program);
  }
  return *m_cache[key];
}

Info* StreamCache::clone() const
{
  SH_DEBUG_ASSERT(false);
  return 0;
}

}

