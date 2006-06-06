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

/**
 * Description of the version of the program cached
 */
struct ProgramVersion {
  // Dimension of all inputs/outputs
  unsigned int dimension;
  // Recalculate incomming index (for offsets/strides)
  bool index_recalculation;
  // Render a single output at a time
  bool single_output;
};

/**
 * Cache of a pass of a stream program
 */
class SplitProgram {
public:
  virtual ~SplitProgram() {}
  virtual void update_uniforms(const SH::Record& uniforms);
  virtual void update_channels(const SH::Stream& stream,
                               const SH::BaseTexture& dest_tex) = 0;
  virtual SH::ProgramSetPtr program_set() = 0;
protected:
  SH::Program epilogue(const SH::ProgramNodePtr& program);
  std::vector<SH::Variable> m_uniforms;
};

/**
 * Cache of a single stream program compiled for a given ProgramVersion
 */
class ProgramVersionCache {
public:
  typedef std::list<SplitProgram*> SplitProgramList;
  typedef SplitProgramList::const_iterator iterator;

  ProgramVersionCache(FloatExtension float_extension,
                      int max_outputs,
                      const SH::Program& vertex_program,
                      const ProgramVersion& version,
                      const SH::Program::BindingSpec& binding_spec,
                      SH::ProgramNode* program);
  ~ProgramVersionCache();

  iterator begin() const { return m_split_program.begin(); }
  iterator end() const { return m_split_program.end(); }
private:
  void split_program(SH::ProgramNode* program,
                     std::list<SH::ProgramNodePtr>& split_programs,
                     const std::string& target, int chunk_size);
  SplitProgramList m_split_program;
};

/**
 * Map of ProgramVersionCaches based on ProgramVersion
 */
class StreamCache : public SH::Info {
public:
  StreamCache(SH::ProgramNode* stream_program,
              SH::ProgramNodePtr vertex_program,
              int max_outputs, FloatExtension ext);
  ~StreamCache();

  SH::Info* clone() const;

  const ProgramVersionCache& get_program_cache(const ProgramVersion& version,
                                               const SH::Program::BindingSpec& binding_spec);

private:
  
  struct Key {
    Key(const ProgramVersion& v, const SH::Program::BindingSpec& bs)
      : version(v), binding_spec(bs) { }
    ProgramVersion version;
    SH::Program::BindingSpec binding_spec;
    bool operator<(const Key& other) const
    {
      if (version.dimension != other.version.dimension)
        return version.dimension < other.version.dimension;
      if (version.index_recalculation != other.version.index_recalculation)
        return version.index_recalculation < other.version.index_recalculation;
      if (version.single_output != other.version.single_output)
        return version.single_output < other.version.single_output;
      return binding_spec < other.binding_spec;
    }
  };
  typedef std::map<Key, ProgramVersionCache*> Cache;
  Cache m_cache;

  int m_max_outputs;
  FloatExtension m_float_extension;
  SH::ProgramNode* m_stream_program;
  SH::ProgramNodePtr m_vertex_program;
};

std::string get_target_backend(const SH::ProgramNodeCPtr& program);

}

#endif
