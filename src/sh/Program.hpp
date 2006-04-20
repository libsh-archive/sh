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

#include "DllExport.hpp"
#include "ProgramNode.hpp"
#include "Backend.hpp"
#include "Stream.hpp"

namespace SH {

class Record;
class BaseTexture;

/** Thin wrapper around ProgramNode.
 */
class
SH_DLLEXPORT Program : public MetaForwarder {
public:
  Program();
  Program(const Program& other);
  Program(const std::string& target);
  Program(const ProgramNodePtr& node);

  Program& operator=(const Program& other);
  
  /// Obtain the node which this Program wraps
  ProgramNodeCPtr node() const { return m_node; }

  /// Obtain the node which this Program wraps
  ProgramNodePtr node() { return m_node; }
  
  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code.  This operation will fail if this program does not
   * have a particular target. */
  void compile(const Pointer<Backend>& backend) { m_node->compile(backend); }

  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code. */
  void compile(const std::string& target, const Pointer<Backend>& backend)
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
  Pointer<BackendCode> code() { return m_node->code(); }
  
  /** Obtain the code for a particular backend. Generates it if necessary.
   * This operation will fail if this program does not have a
   * particular target. */
  Pointer<BackendCode> code(const Pointer<Backend>& backend) {
    return m_node->code(backend);
  }

  /// Obtain the code for a particular backend. Generates it if necessary.
  Pointer<BackendCode> code(const std::string& target, const Pointer<Backend>& backend)
  {
    return m_node->code(target, backend);
  }

  /// Notify this program that a uniform variable has changed.
  void updateUniform(const VariableNodePtr& uniform)
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

  ProgramNode::VarList::const_iterator begin_inputs() const         { return m_node->begin_inputs(); }
  ProgramNode::VarList::const_iterator end_inputs() const           { return m_node->end_inputs(); }
  ProgramNode::VarList::const_iterator begin_outputs() const        { return m_node->begin_outputs(); }
  ProgramNode::VarList::const_iterator end_outputs() const          { return m_node->end_outputs(); }
  ProgramNode::VarList::const_iterator begin_temps() const          { return m_node->begin_temps(); }
  ProgramNode::VarList::const_iterator end_temps() const            { return m_node->end_temps(); }
  ProgramNode::VarList::const_iterator begin_constants() const      { return m_node->begin_constants(); }
  ProgramNode::VarList::const_iterator end_constants() const        { return m_node->end_constants(); }
  ProgramNode::VarList::const_iterator begin_parameters() const     { return m_node->begin_parameters(); }
  ProgramNode::VarList::const_iterator end_parameters() const       { return m_node->end_parameters(); }
  ProgramNode::VarList::const_iterator begin_all_parameters() const { return m_node->begin_all_parameters(); }
  ProgramNode::VarList::const_iterator end_all_parameters() const   { return m_node->end_all_parameters(); }
  ProgramNode::TexList::const_iterator begin_textures() const       { return m_node->begin_textures(); }
  ProgramNode::TexList::const_iterator end_textures() const         { return m_node->end_textures(); }
  ProgramNode::PaletteList::const_iterator begin_palettes() const   { return m_node->begin_palettes(); }
  ProgramNode::PaletteList::const_iterator end_palettes() const     { return m_node->end_palettes(); }

  void append_input(const BaseTexture& input) { m_stream_inputs.append(input); }
  const Stream& stream_inputs() const { return m_stream_inputs; }

  // Call operators for channels and streams.
  // Equivalent to operator<< invocations.
  Program operator()(const BaseTexture& t0) const;
  Program operator()(const Stream& s0) const;
  Program operator()(const BaseTexture& t0,
                        const BaseTexture& t1) const;
  Program operator()(const Stream& s0,
                        const Stream& s1) const;
  Program operator()(const BaseTexture& t0,
                        const BaseTexture& t1,
                        const BaseTexture& t2) const;
  Program operator()(const Stream& s0,
                        const Stream& s1,
                        const Stream& s2) const;
  Program operator()(const BaseTexture& t0,
                        const BaseTexture& t1,
                        const BaseTexture& t2,
                        const BaseTexture& t3) const;
  Program operator()(const Stream& s0,
                        const Stream& s1,
                        const Stream& s2,
                        const Stream& s3) const;
  Program operator()(const BaseTexture& t0,
                        const BaseTexture& t1,
                        const BaseTexture& t2,
                        const BaseTexture& t3,
                        const BaseTexture& t4) const;
  Program operator()(const Stream& s0,
                        const Stream& s1,
                        const Stream& s2,
                        const Stream& s3,
                        const Stream& s4) const;
  
  // Call operators for records
  // May want to merge these with above in the long term. 
  Program operator()(const Record &rec) const;
  Program operator()(const Variable &v0) const;
  Program operator()(const Variable &v0, 
                       const Variable &v1) const;
  Program operator()(const Variable &v0, 
                       const Variable &v1, 
                       const Variable &v2) const;
  Program operator()(const Variable &v0, 
                       const Variable &v1, 
                       const Variable &v2, 
                       const Variable &v3) const;
  
private:
  Stream m_stream_inputs;
  ProgramNodePtr m_node;
};

}

#endif

