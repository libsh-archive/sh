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
#ifndef SHVARIABLENODE_HPP
#define SHVARIABLENODE_HPP

#include <string>
#include <list>
#include "ShDllExport.hpp"
#include "ShVariableType.hpp"
#include "ShVariant.hpp"
#include "ShRefCount.hpp"
#include "ShMeta.hpp"
#include "ShSwizzle.hpp"
#include "ShPool.hpp"

namespace SH {

class ShVariableNode;
typedef ShPointer<ShVariableNode> ShVariableNodePtr;
typedef ShPointer<const ShVariableNode> ShVariableNodeCPtr;

class ShProgramNode;

// Used to hide our use of ShProgramNodePtr, since MSVC7 doesn't allow it.
struct ShVariableNodeEval;

/** A generic n-tuple variable.
 */
class 
SH_DLLEXPORT ShVariableNode : public virtual ShRefCountable,
                       public virtual ShMeta {
public:
  /// Constructs a VariableNode that holds a tuple of data of type 
  //given by the valueType.
  ShVariableNode(ShBindingType kind, int size, ShValueType valueType, ShSemanticType type = SH_ATTRIB);

  virtual ~ShVariableNode();

  /// Clones this ShVariableNode, copying all fields and meta data
  // except it uses the specified fields in the place of the originals. 
  //
  // If updateVarList is set to false, then the clone is not added to the
  // current ShProgramNode's variable lists.  You *must* add it in manually 
  // for INPUT/OUTPUT/INOUT types.
  //
  // If keepUniform is set to false, then the new variable  
  // has m_uniform set to false even if the original m_uniform was
  // true.
  //
  // TODO: m_uniform should never be true if newKind != TEMP
  //
  // Arguments that are set to their default values means use the same
  // as the old node.
  // @{
  ShVariableNodePtr clone(ShBindingType newKind = SH_BINDINGTYPE_END, 
      int newSize = 0, 
      ShValueType newValueType = SH_VALUETYPE_END,
      ShSemanticType newType = SH_SEMANTICTYPE_END,
      bool updateVarList = true, 
      bool keepUniform = true) const;
  // @}

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  ///< Get the number of elements in this variable
  int size() const { return m_size; }
  // Don't call this on uniforms!
  void size(int s) { SH_DEBUG_ASSERT(!m_variant); m_size = s; }

  void lock(); ///< Do not update bound shaders in subsequent setValue calls
  void unlock(); ///< Update bound shader values, and turn off locking
  
  ///< Returns index of the data type held in this node 
  ShValueType valueType() const { return m_valueType; }

  // Metadata
  std::string name() const; ///< Get this variable's name
  void name(const std::string& n); ///< Set this variable's name

  /// Whether this variable has ranges set
  bool hasRange();
 
  /// Set a range of possible values for this variable's elements
  // low and high must be scalar elements (otherwise this just uses the first component)
  void rangeVariant(const ShVariant* low, const ShVariant* high);
  void rangeVariant(const ShVariant* low, const ShVariant* high, 
      bool neg, const ShSwizzle &writemask);

  /** Generates a new ShVariant holding the lower bound */
  ShVariantPtr lowBoundVariant() const;

  /** Generates a new ShVariant holding the upper bound */
  ShVariantPtr highBoundVariant() const;

  const ShBindingType& kind() const { return m_kind; }
  const ShSemanticType& specialType() const { return m_specialType; }
  void specialType(const ShSemanticType& type) { m_specialType = type; }

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  /// Set the elements of this' variant to those of other 
  // @{
  void setVariant(const ShVariant* other);
  void setVariant(const ShVariantCPtr& other);
  // @}
  
  /// Update indexed element of this' variant 
  // @{
  void setVariant(const ShVariant* other, int index);
  void setVariant(const ShVariantCPtr& other, int index);
  // @}
  
  /// Update elements of this' variant applying the given writemask and negation
  // @{
  void setVariant(const ShVariant* other, bool neg, const ShSwizzle &writemask);
  void setVariant(const ShVariantCPtr& other, bool neg, const ShSwizzle &writemask);
  // @}

  /// Retrieve the variant 
  const ShVariant* getVariant() const;

  /// Retrieve the variant.  This should probably only be used internally.
  // You need to call update_all if you change the values here
  ShVariant* getVariant();

  /// Ensure this node has space to store host-side values.
  /// Normally this is not necessary, but when uniforms are given
  /// dependent programs and evaluated all the temporaries will need
  /// to store values during an evaluation.
  void addVariant();

  /** @group dependent_uniforms
   * This code applies only to uniforms.
   * @{
   */
  
  /// Attaching a null program causes this uniform to no longer become
  /// dependent.
  void attach(const ShPointer<ShProgramNode>& evaluator);

  /// Reevaluate a dependent uniform
  void update();

  // @todo type find a better function name for this
  // (Later this should just update the backends as part of shUpdate 
  // and all calls to this should be replaced with update_dependents) 
  void update_all(); /// Updates a uniform in currently bound shaders and all dependencies

  /// Obtain the program defining this uniform, if any.
  const ShPointer<ShProgramNode>& evaluator() const;
  
  /** @} */

#ifdef SH_USE_MEMORY_POOL
  // Memory pool stuff.
  void* operator new(std::size_t size);
  void operator delete(void* d, std::size_t size);
#endif
  
protected:
  /// Creates a new variable node that holds the same data type as old
  // with the option to alter binding/semantic types and size.
  //
  // When updateVarList is false, the new ShVariableNode does not get entered
  // into the current ShProgram's variable lists.
  //
  // When maintainUniform is true, the old.m_uniform is used instead
  // of setting up the value based on current ShContext state
  ShVariableNode(const ShVariableNode& old, ShBindingType newKind, 
      int newSize, ShValueType newValueType, ShSemanticType newType, 
      bool updateVarList, bool keepUniform);

  // Generates default low bound based on current special type
  ShVariant* makeLow() const;

  // Generates a default high bound baesd on current special type
  ShVariant* makeHigh() const;


  void programVarListInit(); /// After kind, size and type are set, this 

  /** Adds this to the declared temps set. 
   * If current parsing program has cfg node (i.e. this is a transformation,
   * not a new program specification), then adds to programs entry node.
   * Else adds an SH_OP_DECL as a dummy statement into current parsing block.
   */
  void programDeclInit(); 
                          

  void add_dependent(ShVariableNode* dep);
  void remove_dependent(ShVariableNode* dep);

  void update_dependents();
  void detach_dependencies();
  
  
   
  bool m_uniform;
  ShBindingType m_kind;
  ShSemanticType m_specialType;
  ShValueType m_valueType;

  int m_size;
  int m_id;
  int m_locked;

  /** Ref-counted pointer to the variant.  ShVariableNode is always
   * the sole owner of this pointer once the node exists. */
  ShVariantPtr m_variant;

  // Dependent uniform evaluation
  mutable ShVariableNodeEval* m_eval;
  std::list<ShVariableNode*> m_dependents;
  
  static int m_maxID;

#ifdef SH_USE_MEMORY_POOL
  static ShPool* m_pool;
#endif
};

}

#endif
