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
#ifndef SHTRANSFORMER_HPP
#define SHTRANSFORMER_HPP

#include "DllExport.hpp"
#include "Backend.hpp"
#include "Program.hpp"
#include "CtrlGraph.hpp"
#include "Internals.hpp"
#include "VariableType.hpp"

namespace SH {

/** Program transformer. 
 * Platform-specific transformations on Program objects.
 *
 * These may change the variable lists (inputs, outputs, etc.)
 * and control graph of the Program, so they should be done
 * on a copy of the Program just before code generation instead
 * of the original. (Otherwise, future algebra operations on 
 * the original may not give the expected results since the variables
 * will no longer be the same)
 *
 * Global requirements for running Transformer:
 * Context::current()->parsing() == program
 */
class 
SH_DLLEXPORT Transformer {
public:
  Transformer(const ProgramNodePtr& program);
  ~Transformer();
  bool changed(); //< returns true iff one of the transformations changed the shader

  /**@name Tuple splitting when backend canot support arbitrary 
   * length tuples.
   *
   * If any tuples are split, this adds entries to the splits map
   * to map from original long tuple VariableNode
   * to a vector of VariableNodes all <= max tuple size of the backend.
   * All long tuples in the intermediate representation are split up into
   * shorter tuples.
   */
  //@{
  typedef std::vector<VariableNodePtr> VarNodeVec;
  typedef std::map<VariableNodePtr, VarNodeVec> VarSplitMap;
  friend struct VariableSplitter;
  friend struct StatementSplitter;
  void splitTuples(int maxTuple, VarSplitMap &splits);
  //@}
  
  /**@name Input and Output variable to temp convertor
   * In most GPU shading languages/assembly, outputs cannot be used as src
   * variable in computation and inputs cannot be used as a dest, and
   * inout variables are not supported directly.
   *
   * Optionally one can provide a VarTransformMap that the function will
   * update with any new variable transformations that it makes.
   *
   * @todo currently all or none approach to conversion.
   * could parameterize this with flags to choose SH_INPUT, SH_OUTPUT, SH_INOUT
   */
  //@{
  friend struct InputOutputConvertor;
  void convertInputOutput(VarTransformMap *varTransMap = 0);
  //@}
  
  /**@name Texture lookup conversion
   * Most GPUs only do indexed lookup for rectangular textures, so
   * convert other kinds of lookup with appropriate scales.
   */
  //@{
  void convertTextureLookups();
  //@}

  typedef std::map<ValueType, ValueType> ValueTypeMap;  
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
  void convertToFloat(ValueTypeMap &typeMap, bool preserve_casts = false);

  //@todo  use dependent uniforms in conversion and spliting functions
  //instead of separate VarSplitMaps and VarMaps

  /** Strips out dummy statements (SECTION in particular) 
   */
  void stripDummyOps();

  /** Replace atan2 by a division followed by an arctan.
   */
  void expand_atan2();

  /** Replace atanh, asinh and acosh with a log.
   */
  void expand_inverse_hyperbolic();
  
  /** Replace sinh, cosh and tanh with exp
   */
  void expand_hyperbolic();

  /** Replace TEXD with TEXLOD ops.
   */
  void texd_to_texlod();

  /** Replace NORM with DOT, RSQRT and MUL
   */
  void expand_normalize();

  /** Replace SQRT with RCP and RSQRT
   */
  void reciprocate_sqrt();

  /** Replace DIV with RCP and MUL
   */
  void expand_div();

  /** Replace XPD with MUL and MAD
   */
  void expand_xpd();
  
  /** Replace RND with ADD and FLR
   */
  void expand_rnd();
  
  /** Replace LRP
   */
  void expand_lrp();
  
  /** Replace MOD
   */
  void expand_mod();
  
  /** Replace POW with EXP and LOG
   */
  void expand_pow();
  
  /** Replace SGN with SGT and SLT
   */
  void expand_sgn();
  
  void order_dest_swizzles();

  // Assumes order_dest_swizzles has already run
  void remove_writemasks();
  
private:
  /// NOT IMPLEMENTED
  Transformer(const Transformer& other);
  /// NOT IMPLEMENTED
  Transformer& operator=(const Transformer& other);

  ProgramNodePtr m_program;
  bool m_changed;
};

/* A default transformer.
 * T can overload any of the methods in TransformerParent 
 *
 * - void start(const ProgramNodePtr&)
 *   Does whatever initialization may be necessary.  Initializes changed() to
 *   false.
 *
 * - void handleVarList(ProgramNode::VarList &varlist, BindingType type); 
 *   This is called first and fixes anything that needs to be fixed in the
 *   variable lists. 
 * - handleTexList, handleChannelList, handlePaletteList are similar
 *
 * - bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node);
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
struct DefaultTransformer: public T {
  // Applies transformation to the given ctrl graph node. 
  void operator()(CtrlGraphNode* node); 

  // Applies transformation to the given Program 
  bool transform(const ProgramNodePtr& p); 
};

struct TransformerParent {
 TransformerParent() : m_changed(false) {}
 void start(const ProgramNodePtr& program) { m_program = program; }
 void handleVarList(ProgramNode::VarList &varlist, BindingType type) {}
 void handleTexList(ProgramNode::TexList &texlist) {}
 void handlePaletteList(ProgramNode::PaletteList &palettelist) {}

 bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) { return false; }
 void finish() {}
 bool changed() 
 { return m_changed; }

 protected:
   ProgramNodePtr m_program;
   bool m_changed;
};

}

#include "TransformerImpl.hpp"

#endif
