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
#ifndef SHPROGRAMNODE_HPP
#define SHPROGRAMNODE_HPP

#include <list>
#include <map>
#include <utility>
#include <string>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Tokenizer.hpp"
#include "VariableNode.hpp"
#include "CtrlGraph.hpp"
#include "TextureNode.hpp"
#include "PaletteNode.hpp"
#include "ChannelNode.hpp"
#include "Meta.hpp"

namespace SH {

class BackendCode;
class Backend;

/** A particular Sh program.
 */
class
DLLEXPORT ProgramNode : public virtual RefCountable, public virtual Meta, public virtual InfoHolder {
public:
  ProgramNode(const std::string& target);

  ~ProgramNode();
  
  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code.  This operation will fail if this program does not
   * have a particular target. */
  void compile(const Pointer<Backend>& backend);
  /** Forcefully compile this program for a particular backend, even
   * if it has been compiled previously. Use code() to obtain the
   * actual code. */
  void compile(const std::string& target, const Pointer<Backend>& backend);

  /** Check whether the program has been compiled for the default
   * backend.  This operation will fail if this program does not have
   * particular target. */
  bool is_compiled() const;
  /** Check whether the program has been compiled for the default
   * backend and the given target. */
  bool is_compiled(const std::string& target) const;
  /** Check whether the program has been compiled for the given
   * backend and the given target. */
  bool is_compiled(const std::string& target, const Pointer<Backend>& backend) const;

  /// Describe the inputs and outputs of this program.
  std::string describe_interface() const;
  /// Describes all the variables in this program.
  std::string describe_vars() const;
  /// Describes all the temps declared in this program 
  std::string describe_decls() const;
  /// Describe the binding of the inputs under the current backend
  std::string describe_bindings();
  /// Describe the binding of the inputs for the given target
  std::string describe_bindings(const std::string& target);

  /** Describes all of the above to the file filename.vars and
   * dumps a DOT version of the cfg to filename.ps */
  void dump(std::string filename) const;
  
  /** Obtain the code for currently active backend.  This operation
   * will fail if this program does not have a particular target. */
  Pointer<BackendCode> code(); 
  /** Obtain the code for a particular backend. Generates it if
   * necessary.  This operation will fail if this program does not
   * have a particular target. */
  Pointer<BackendCode> code(const Pointer<Backend>& backend);
  /** Obtain the code for a particular backend. Generates it if
   * necessary.*/
  Pointer<BackendCode> code(const std::string& target, const Pointer<Backend>& backend);

  /// Notify this program that a uniform variable has changed.
  void updateUniform(const VariableNodePtr& uniform);

  /** The tokenizer for this program's body. Used only during
   * construction of the program (before parsing) */
  Tokenizer tokenizer;

  /** The control graph (the parsed form of the token
   * list). Constructed during the parsing step, when endProgram()
   * is called. */
  Pointer<CtrlGraph> ctrlGraph;

  /// Called right before optimization to collect temporary declarations 
  void collectDecls();

  /// Called right after optimization to make lists of all the variables used in the program.
  void collectVariables();

  /// Returns whether a temporary variable is declared in this program
  bool hasDecl(const VariableNodePtr& node) const;

  /** Add a declaration.  This is for use when a program is
   * transformed and new temporaries are added.
   *
   * The first version adds to a specified cfg node in addition to this.
   * The second adds to this and the entry node.
   *  
   * @todo range check that adding to the cfg node doesn't screw anything up...
   * (i.e. if somewhere we don't copy cfg when manipulating program...) */
  //@{
  void addDecl(const VariableNodePtr& node, const CtrlGraphNodePtr&);
  void addDecl(const VariableNodePtr& node);
  //@}

  typedef std::set<VariableNodePtr> VarSet;
  typedef std::list<VariableNodePtr> VarList;
  typedef std::list<TextureNodePtr> TexList;
  typedef std::list<ChannelNodePtr> ChannelList;
  typedef std::list<PaletteNodePtr> PaletteList;

  VarList::const_iterator begin_inputs() const         { return inputs.begin(); }
  VarList::const_iterator end_inputs() const           { return inputs.end(); }
  VarList::const_iterator begin_outputs() const        { return outputs.begin(); }
  VarList::const_iterator end_outputs() const          { return outputs.end(); }
  VarList::const_iterator begin_temps() const          { return temps.begin(); }
  VarList::const_iterator end_temps() const            { return temps.end(); }
  VarList::const_iterator begin_constants() const      { return constants.begin(); }
  VarList::const_iterator end_constants() const        { return constants.end(); }
  VarList::const_iterator begin_parameters() const     { return uniforms.begin(); }
  VarList::const_iterator end_parameters() const       { return uniforms.end(); }
  VarList::const_iterator begin_all_parameters() const { return all_uniforms.begin(); }
  VarList::const_iterator end_all_parameters() const   { return all_uniforms.end(); }
  TexList::const_iterator begin_textures() const       { return textures.begin(); }
  TexList::const_iterator end_textures() const         { return textures.end(); }
  ChannelList::const_iterator begin_channels() const   { return channels.begin(); }
  ChannelList::const_iterator end_channels() const     { return channels.end(); }
  PaletteList::const_iterator begin_palettes() const   { return palettes.begin(); }
  PaletteList::const_iterator end_palettes() const     { return palettes.end(); }
  
  VarList inputs; ///< Input variables used in this program
  VarList outputs; ///< Output variables used in this program
  VarList temps; ///< Temporary variables used in this program
  VarSet tempDecls; ///< Temporary variables declared in this program
  VarList constants; ///< Constants used in this program
  VarList uniforms; ///< Uniform variables used in the optimized program
  VarList all_uniforms; ///< All uniform variables including recursively dependent ones
  TexList textures; ///< Textures used in this program
  ChannelList channels; ///< Channels used in FETCH instructions in this program
  PaletteList palettes; ///< Palettes used in PAL instructions in this program

  /** Return the name of the backend for which the program is
   * currently compiled.  Returns an empty string if the program is
   * not yet compiled. */
  std::string backend_name() const { return m_backend_name; }

  /// Can be empty, if there is no target associated with this program.
  std::string target() const { return m_target; }

  /// It may be useful to change a program's target sometimes.
  std::string& target() { return m_target; }

  /// Make a copy of this program.
  Pointer<ProgramNode> clone() const;

  /// Print a description of a list of variables
  static std::ostream& print(std::ostream& out, const VarList& list);

  /// True if this program has been completed with SH_END.
  bool finished() const { return m_finished; }

  /// @internal Set finished to true. Only endShader() needs to call this.
  void finish();

  /** @internal Indicate that we have been assigned to a uniform
   * during construction. This is so we can call back that uniform
   * when the program is finished constructing. */
  void assign(const VariableNodePtr& var) const { m_assigned_var = var; }
  
private:
  std::string m_backend_name; ///< Can be empty if the program is not yet compiled for a backend.
  std::string m_target; ///< Can be empty, if there is no target associated with this program.

  void collect_node_decls(const Pointer<CtrlGraphNode>& node);
  void collect_node_vars(const Pointer<CtrlGraphNode>& node);
  void collect_var(const VariableNodePtr& node);
  void collect_dependent_uniform(const VariableNodePtr& var);

  typedef std::map< std::pair< std::string, Pointer<Backend> >,
                    Pointer<BackendCode> > CodeMap;
  CodeMap m_code; ///< Compiled code is cached here.

  bool m_finished; ///< True if this program has been constructed
                   /// completely.

  mutable VariableNodePtr m_assigned_var;
};

typedef Pointer<ProgramNode> ProgramNodePtr;
typedef Pointer<const ProgramNode> ProgramNodeCPtr;

}

#endif

