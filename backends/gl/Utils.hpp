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


class StreamCache : public SH::Info {
public:
  StreamCache(SH::ProgramNode* stream_program,
              SH::ProgramNode* vertex_program,
              int max_outputs, FloatExtension ext);

  SH::Info* clone() const;

  typedef int ProgramVersion;
  static const int OS_NONE_2D           = 0x00;
  static const int OS_NONE_3D           = 0x01;
  static const int OS_1D                = 0x02;
  static const int OS_2D                = 0x03;
  static const int OS_3D                = 0x04;
  static const int SINGLE_OUTPUT        = 0x08;
  static const int NUM_PROGRAM_VERSIONS = 16;

  void freeze_inputs(ProgramVersion version, bool state);
  void update_channels(ProgramVersion version, const SH::Stream& stream);
  void update_destination(ProgramVersion version, const SH::BaseTexture& tex,
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
  void generate_programs(ProgramVersion version);
  
  struct InputData {
    SH::TextureNodePtr tex;
    SH::Attrib4f os1, os2;
  };
  typedef std::vector<InputData> InputList;

  InputList m_inputs[NUM_PROGRAM_VERSIONS];
  std::list<SH::ProgramNodePtr> m_programs[NUM_PROGRAM_VERSIONS];
  std::list<SH::ProgramSetPtr> m_program_sets[NUM_PROGRAM_VERSIONS];

  SH::ProgramNode* m_stream_program;
  SH::ProgramNode* m_vertex_program;

  int m_max_outputs;
  FloatExtension m_float_extension;
  SH::Attrib4f m_output_offset;
  SH::Attrib4f m_output_stride;

  StreamCache(const StreamCache& other);
  StreamCache& operator=(const StreamCache& other);
};

void split_program(SH::ProgramNode* program,
                   std::list<SH::ProgramNodePtr>& programs,
                   const std::string& target, int chunk_size);

std::string get_target_backend(const SH::ProgramNodeCPtr& program);

}

#endif
