// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHTRANSFORMER_HPP
#define SHTRANSFORMER_HPP

#include "ShDllExport.hpp"
#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShCtrlGraph.hpp"
#include "ShInternals.hpp"
#include "ShVariableType.hpp"

namespace SH {

/** Program transformer. 
 * Platform-specific transformations on ShProgram objects.
 *
 * These may change the variable lists (inputs, outputs, etc.)
 * and control graph of the ShProgram, so they should be done
 * on a copy of the ShProgram just before code generation instead
 * of the original. (Otherwise, future algebra operations on 
 * the original may not give the expected results since the variables
 * will no longer be the same)
 *
 * Global requirements for running ShTransformer:
 * ShContext::current()->parsing() == program
 */
class 
SH_DLLEXPORT ShTransformer {
public:
  ShTransformer(const ShProgramNodePtr& program);
  ~ShTransformer();
  bool changed(); //< returns true iff one of the transformations changed the shader

  /**@name Tuple splitting when backend canot support arbitrary 
   * length tuples.
   *
   * If any tuples are split, this adds entries to the splits map
   * to map from original long tuple ShVariableNode
   * to a vector of ShVariableNodes all <= max tuple size of the backend.
   * All long tuples in the intermediate representation are split up into
   * shorter tuples.
   */
  //@{
  typedef std::vector<ShVariableNodePtr> VarNodeVec;
  typedef std::map<ShVariableNodePtr, VarNodeVec> VarSplitMap;
  friend struct VariableSplitter;
  friend struct StatementSplitter;
  void splitTuples(int maxTuple, VarSplitMap &splits);
  //@}
  
  /**@name Input and Output variable to temp convertor
   * In most GPU shading languages/assembly, outputs cannot be used as src
   * variable in computation and inputs cannot be used as a dest, and
   * inout variables are not supported directly.
   *
   * Optionally one can provide a ShVarTransformMap that the function will
   * update with any new variable transformations that it makes.
   *
   * @todo currently all or none approach to conversion.
   * could parameterize this with flags to choose INPUT, OUTPUT, INOUT
   */
  //@{
  friend struct InputOutputConvertor;
  void convertInputOutput(ShVarTransformMap *varTransMap = 0);
  //@}
  
  /**@name Texture lookup conversion
   * Most GPUs only do indexed lookup for rectangular textures, so
   * convert other kinds of lookup with appropriate scales.
   */
  //@{
  void convertTextureLookups();
  //@}

  typedef std::map<ShValueType, ShValueType> ValueTypeMap;  
  /**@name Arithmetic type conversions
   * Converts ops on other basic non-float Sh types to float storage types based on the
   * floatMapping.  
   * For each key k (a type index to convert), in floatMapping, the converter
   * changes it to use floating point type index floatMapping[k].
   *
   * (Note, the converter does not handle cases where floatMapping[j] exists,
   * but j is also floatMapping[k])
   *
   */
  void convertToFloat(ValueTypeMap &typeMap);

  //@todo  use dependent uniforms in conversion and spliting functions
  //instead of separate VarSplitMaps and ShVarMaps

  /** Strips out dummy statements (SECTION in particular) 
   */
  void stripDummyOps();

  /** Replace atan2 by a division followed by an arctan.
   */
  void expand_atan2();

  /** Replace atanh, asinh and acosh with a log.
   */
  void expand_inverse_hyperbolic();

  /** Replace TEXD with TEXLOD ops.
   */
  void texd_to_texlod();

private:
  /// NOT IMPLEMENTED
  ShTransformer(const ShTransformer& other);
  /// NOT IMPLEMENTED
  ShTransformer& operator=(const ShTransformer& other);

  ShProgramNodePtr m_program;
  bool m_changed;
};

/* A default transformer.
 * T can overload any of the methods in ShTransformerParent 
 *
 * - void start(const ShProgramNodePtr&)
 *   Does whatever initialization may be necessary.  Initializes changed() to
 *   false.
 *
 * - void handleVarList(ShProgramNode::VarList &varlist, ShBindingType type); 
 *   This is called first and fixes anything that needs to be fixed in the
 *   variable lists. 
 * - handleTexList, handleChannelList, handlePaletteList are similar
 *
 * - bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, const ShCtrlGraphNodePtr& node);
 *   This performs some kind of per-statement transformation during a dfs
 *   through the cfg.  Returns true iff the transformation has already
 *   incremented I (or deleted I and moved I to the next element). 
 *
 * - void finish();
 *   Does any cleanup necessary afterwards, any other transformations to wrap
 *   things up. 
 *
 *   Note - this allows you to split a set of multipass transformations into 
 *   smaller chunks by chaining transformers together.  Have one transformer's
 *   finish call the next transformer's transform()...
 *
 * It must also implement this method:
 * - bool changed()
 *   Returns whether this transformer changed anything
 *
 * @todo range - this sequence of transformations (varlist, statements, finish),
 * might not be the right abstraction...it's just matches most of what we have
 * used so far - first pass identifies something about the variables,
 * second actually fixes the cfg, and final pass does any general stuff
 * that doesn't fit in.
 */ 
template<typename T>
struct ShDefaultTransformer: public T {
  // Applies transformation to the given ctrl graph node. 
  void operator()(const ShCtrlGraphNodePtr& node); 

  // Applies transformation to the given ShProgram 
  bool transform(const ShProgramNodePtr& p); 
};

struct ShTransformerParent {
 ShTransformerParent() : m_changed(false) {}
 void start(const ShProgramNodePtr& program) { m_program = program; }
 void handleVarList(ShProgramNode::VarList &varlist, ShBindingType type) {}
 void handleTexList(ShProgramNode::TexList &texlist) {}
 void handleChannelList(ShProgramNode::ChannelList &chanlist) {}
 void handlePaletteList(ShProgramNode::PaletteList &palettelist) {}

 bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, const ShCtrlGraphNodePtr& node) { return false; }
 void finish() {}
 bool changed() 
 { return m_changed; }

 protected:
   ShProgramNodePtr m_program;
   bool m_changed;
};

}

#include "ShTransformerImpl.hpp"

#endif
