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
#ifndef SHPROGRAM_HPP
#define SHPROGRAM_HPP

#include "ShDllExport.hpp"
#include "ShProgramNode.hpp"
#include "ShBackend.hpp"

namespace SH {

class ShStream;
template<typename T> class ShChannel;
class ShRecord;

/** Thin wrapper around ShProgramNode.
 */
class
SH_DLLEXPORT ShProgram : public ShMetaForwarder {
public:
  ShProgram();
  ShProgram(const ShProgram& other);
  ShProgram(const std::string& target);
  ShProgram(const ShProgramNodePtr& node);

  ShProgram& operator=(const ShProgram& other);
  
  /// Obtain the node which this ShProgram wraps
  ShProgramNodeCPtr node() const { return m_node; }

  /// Obtain the node which this ShProgram wraps
  ShProgramNodePtr node() { return m_node; }
  
  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code.  This operation will fail if this program does not
   * have a particular target. */
  void compile(const ShPointer<ShBackend>& backend) { m_node->compile(backend); }

  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code. */
  void compile(const std::string& target, const ShPointer<ShBackend>& backend)
  {
    m_node->compile(target, backend);
  }

  /** Check whether the program has been compiled for the default
   * backend.  This operation will fail if this program does not have
   * particular target. */
  bool is_compiled() const { return m_node->is_compiled(); }

  /** Check whether the program has been compiled for the default
   * backend and the given target. */
  bool is_compiled(const std::string& target) const
  { 
    return m_node->is_compiled(target);
  }

  /** Obtain a listing of the inputs, outputs and uniforms used by
   * this program. */
  std::string describe_interface() const
  {
    return m_node->describe_interface();
  }

  /** Obtain a listing stating which attributes will get bound to
      which program inputs under the current backend. */
  std::string describe_bindings() const
  {
    return m_node->describe_bindings();
  }
  
  /** Obtain a listing stating which attributes will get bound to
      which program inputs for the given target. */
  std::string describe_bindings(const std::string& target) const
  {
    return m_node->describe_bindings(target);
  }
  
  /** Obtain the code for currently active backend. 
   * This operation will fail if this program does not have a
   * particular target. */
  ShPointer<ShBackendCode> code() { return m_node->code(); }
  
  /** Obtain the code for a particular backend. Generates it if necessary.
   * This operation will fail if this program does not have a
   * particular target. */
  ShPointer<ShBackendCode> code(const ShPointer<ShBackend>& backend) {
    return m_node->code(backend);
  }

  /// Obtain the code for a particular backend. Generates it if necessary.
  ShPointer<ShBackendCode> code(const std::string& target, const ShPointer<ShBackend>& backend)
  {
    return m_node->code(target, backend);
  }

  /// Notify this program that a uniform variable has changed.
  void updateUniform(const ShVariableNodePtr& uniform)
  {
    m_node->updateUniform(uniform);
  }

  /** Return the name of the backend for which the program is
   * currently compiled.  Returns an empty string if the program is
   * not yet compiled. */
  std::string backend_name() const { return m_node->backend_name(); }

  std::string target() const { return m_node->target(); }

  /// It may be useful to change a program's target sometimes.
  std::string& target() { return m_node->target(); }

  ShProgramNode::VarList::const_iterator begin_inputs() const         { return m_node->begin_inputs(); }
  ShProgramNode::VarList::const_iterator end_inputs() const           { return m_node->end_inputs(); }
  ShProgramNode::VarList::const_iterator begin_outputs() const        { return m_node->begin_outputs(); }
  ShProgramNode::VarList::const_iterator end_outputs() const          { return m_node->end_outputs(); }
  ShProgramNode::VarList::const_iterator begin_temps() const          { return m_node->begin_temps(); }
  ShProgramNode::VarList::const_iterator end_temps() const            { return m_node->end_temps(); }
  ShProgramNode::VarList::const_iterator begin_constants() const      { return m_node->begin_constants(); }
  ShProgramNode::VarList::const_iterator end_constants() const        { return m_node->end_constants(); }
  ShProgramNode::VarList::const_iterator begin_parameters() const     { return m_node->begin_parameters(); }
  ShProgramNode::VarList::const_iterator end_parameters() const       { return m_node->end_parameters(); }
  ShProgramNode::VarList::const_iterator begin_all_parameters() const { return m_node->begin_all_parameters(); }
  ShProgramNode::VarList::const_iterator end_all_parameters() const   { return m_node->end_all_parameters(); }
  ShProgramNode::TexList::const_iterator begin_textures() const       { return m_node->begin_textures(); }
  ShProgramNode::TexList::const_iterator end_textures() const         { return m_node->end_textures(); }
  ShProgramNode::ChannelList::const_iterator begin_channels() const   { return m_node->begin_channels(); }
  ShProgramNode::ChannelList::const_iterator end_channels() const     { return m_node->end_channels(); }
  ShProgramNode::PaletteList::const_iterator begin_palettes() const   { return m_node->begin_palettes(); }
  ShProgramNode::PaletteList::const_iterator end_palettes() const     { return m_node->end_palettes(); }

  // Call operators for channels and streams.
  // Equivalent to operator<< invocations.
  // Note that the template functions are implemented in
  // ShChannelImpl.hpp
  template<typename T0>
  ShProgram operator()(const ShChannel<T0>& t0) const;
  ShProgram operator()(const ShStream& s0) const;
  template<typename T0, typename T1>
  ShProgram operator()(const ShChannel<T0>& t0,
                        const ShChannel<T1>& t1) const;
  ShProgram operator()(const ShStream& s0,
                        const ShStream& s1) const;
  template<typename T0, typename T1, typename T2>
  ShProgram operator()(const ShChannel<T0>& t0,
                        const ShChannel<T1>& t1,
                        const ShChannel<T2>& t2) const;
  ShProgram operator()(const ShStream& s0,
                        const ShStream& s1,
                        const ShStream& s2) const;
  template<typename T0, typename T1, typename T2, typename T3>
  ShProgram operator()(const ShChannel<T0>& t0,
                        const ShChannel<T1>& t1,
                        const ShChannel<T2>& t2,
                        const ShChannel<T3>& t3) const;
  ShProgram operator()(const ShStream& s0,
                        const ShStream& s1,
                        const ShStream& s2,
                        const ShStream& s3) const;
  template<typename T0, typename T1, typename T2, typename T3,
           typename T4>
  ShProgram operator()(const ShChannel<T0>& t0,
                        const ShChannel<T1>& t1,
                        const ShChannel<T2>& t2,
                        const ShChannel<T3>& t3,
                        const ShChannel<T4>& t4) const;
  ShProgram operator()(const ShStream& s0,
                        const ShStream& s1,
                        const ShStream& s2,
                        const ShStream& s3,
                        const ShStream& s4) const;
  
  // Call operators for records
  // May want to merge these with above in the long term. 
  ShProgram operator()(const ShRecord &rec) const;
  ShProgram operator()(const ShVariable &v0) const;
  ShProgram operator()(const ShVariable &v0, 
                       const ShVariable &v1) const;
  ShProgram operator()(const ShVariable &v0, 
                       const ShVariable &v1, 
                       const ShVariable &v2) const;
  ShProgram operator()(const ShVariable &v0, 
                       const ShVariable &v1, 
                       const ShVariable &v2, 
                       const ShVariable &v3) const;
  
private:

  ShProgramNodePtr m_node;
};

}

#endif

