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
#ifndef SHUTILS_HPP
#define SHUTILS_HPP

#include <map>
#include <list>
#include "TextureNode.hpp"
#include "CtrlGraph.hpp"
#include "ProgramNode.hpp"
#include "ProgramSet.hpp"
#include "Attrib.hpp"

namespace shgl {

enum FloatExtension {
  ARB_NV_FLOAT_BUFFER,
  ARB_ATI_PIXEL_FORMAT_FLOAT,
  ARB_NO_FLOAT_EXT
};

class StreamCache {
public:
  StreamCache(SH::ProgramNode* stream_program,
              SH::ProgramNode* vertex_program,
              SH::Program::BindingSpec binding_spec,
              int max_outputs, FloatExtension ext);

  typedef int ProgramVersion;
  static const int OS_NONE_2D           = 0x00;
  static const int OS_NONE_3D           = 0x01;
  static const int OS_1D                = 0x02;
  static const int OS_2D                = 0x03;
  static const int OS_3D                = 0x04;
  static const int SINGLE_OUTPUT        = 0x08;
  static const int NUM_PROGRAM_VERSIONS = 16;

  void generate_programs(ProgramVersion version);
  void update_uniforms(const SH::Record& input_uniforms);
  void update_channels(ProgramVersion version, 
                       const SH::Stream& stream, const SH::BaseTexture& tex,
                       int width, int height, int depth);

  typedef std::list<SH::ProgramSetPtr>::iterator set_iterator;
  typedef std::list<SH::ProgramSetPtr>::const_iterator set_const_iterator;
  typedef std::list<SH::ProgramNodePtr>::iterator program_iterator;
  typedef std::list<SH::ProgramNodePtr>::const_iterator program_const_iterator;

  set_iterator sets_begin(ProgramVersion version);
  set_iterator sets_end(ProgramVersion version);
  set_const_iterator sets_begin(ProgramVersion version) const;
  set_const_iterator sets_end(ProgramVersion version) const;

private:
  
  struct StreamData {
    SH::TextureNodePtr tex[4];
    SH::Attrib4f uniform4[2];
    SH::Attrib4f uniform2[2][2];
  };
  typedef std::vector<StreamData> StreamList;
  StreamList m_streams[NUM_PROGRAM_VERSIONS];
  
  typedef std::vector<SH::Variable> UniformList;
  UniformList m_uniforms;
  
  std::list<SH::ProgramNodePtr> m_programs[NUM_PROGRAM_VERSIONS];
  std::list<SH::ProgramSetPtr> m_program_sets[NUM_PROGRAM_VERSIONS];

  SH::ProgramNode* m_stream_program;
  SH::ProgramNode* m_vertex_program;
  SH::Program::BindingSpec m_binding_spec;

  int m_max_outputs;
  FloatExtension m_float_extension;
  SH::Attrib4f m_output_offset;
  SH::Attrib4f m_output_stride;

  StreamCache(const StreamCache& other);
  StreamCache& operator=(const StreamCache& other);
};

class StreamBindingCache 
  : public SH::Info, public std::map<SH::Program::BindingSpec, StreamCache*> {
public:
  ~StreamBindingCache();

  SH::Info* clone() const;
};


void split_program(SH::ProgramNode* program,
                   std::list<SH::ProgramNodePtr>& programs,
                   const std::string& target, int chunk_size);

std::string get_target_backend(const SH::ProgramNodeCPtr& program);

}

#endif
