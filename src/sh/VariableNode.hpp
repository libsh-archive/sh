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
#include "DllExport.hpp"
#include "VariableType.hpp"
#include "Variant.hpp"
#include "RefCount.hpp"
#include "Meta.hpp"
#include "Swizzle.hpp"
#include "Pool.hpp"

namespace SH {

class VariableNode;
typedef Pointer<VariableNode> VariableNodePtr;
typedef Pointer<const VariableNode> VariableNodeCPtr;

class ProgramNode;

// Used to hide our use of ProgramNodePtr, since MSVC7 doesn't allow it.
struct VariableNodeEval;

/** A generic n-tuple variable.
 */
class 
SH_DLLEXPORT VariableNode : public virtual RefCountable,
                       public virtual Meta {
public:
  /// Constructs a VariableNode that holds a tuple of data of type 
  //given by the valueType.
  VariableNode(BindingType kind, int size, ValueType valueType, SemanticType type = SH_ATTRIB);

  virtual ~VariableNode();

  /// Clones this VariableNode, copying all fields and meta data
  // except it uses the specified fields in the place of the originals. 
  //
  // If updateVarList is set to false, then the clone is not added to the
  // current ProgramNode's variable lists.  You *must* add it in manually 
  // for SH_INPUT/SH_OUTPUT/SH_INOUT types.
  //
  // If keepUniform is set to false, then the new variable  
  // has m_uniform set to false even if the original m_uniform was true. 
  //
  // Arguments that are set to their default values means use the same
  // as the old node.
  // @{
  VariableNodePtr clone(BindingType newKind = BINDINGTYPE_END, 
      int newSize = 0, 
      ValueType newValueType = VALUETYPE_END,
      SemanticType newType = SEMANTICTYPE_END,
      bool updateVarList = true, 
      bool keepUniform = true) const;
  // @}

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  int size() const; ///< Get the number of elements in this variable

  // Don't call this on uniforms!
  void size(int size);

  void lock(); ///< Do not update bound shaders in subsequent setValue calls
  void unlock(); ///< Update bound shader values, and turn off locking
  
  ValueType valueType() const; ///< Returns index of the data type held in this node 

  // Metadata
  std::string name() const; ///< Get this variable's name
  void name(const std::string& n); ///< Set this variable's name

  /// Whether this variable has ranges set
  bool hasRange();
 
  /// Set a range of possible values for this variable's elements
  // low and high must be scalar elements (otherwise this just uses the first component)
  void rangeVariant(const Variant* low, const Variant* high);
  void rangeVariant(const Variant* low, const Variant* high, 
      bool neg, const Swizzle &writemask);

  /** Generates a new Variant holding the lower bound */
  VariantPtr lowBoundVariant() const;

  /** Generates a new Variant holding the upper bound */
  VariantPtr highBoundVariant() const;

  BindingType kind() const;
  SemanticType specialType() const;
  void specialType(SemanticType);

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  /// Set the elements of this' variant to those of other 
  // @{
  void setVariant(const Variant* other);
  void setVariant(const VariantCPtr& other);
  // @}
  
  /// Update indexed element of this' variant 
  // @{
  void setVariant(const Variant* other, int index);
  void setVariant(const VariantCPtr& other, int index);
  // @}
  
  /// Update elements of this' variant applying the given writemask and negation
  // @{
  void setVariant(const Variant* other, bool neg, const Swizzle &writemask);
  void setVariant(const VariantCPtr& other, bool neg, const Swizzle &writemask);
  // @}

  /// Retrieve the variant 
  const Variant* getVariant() const;

  /// Retrieve the variant.  This should probably only be used internally.
  // You need to call update_all if you change the values here
  Variant* getVariant();

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
  void attach(const Pointer<ProgramNode>& evaluator);

  /// Reevaluate a dependent uniform
  void update();

  // @todo type find a better function name for this
  // (Later this should just update the backends as part of update 
  // and all calls to this should be replaced with update_dependents) 
  void update_all(); /// Updates a uniform in currently bound shaders and all dependencies

  /// Obtain the program defining this uniform, if any.
  const Pointer<ProgramNode>& evaluator() const;
  
  /** @} */

#ifdef USE_MEMORY_POOL
  // Memory pool stuff.
  void* operator new(std::size_t size);
  void operator delete(void* d, std::size_t size);
#endif
  
protected:
  /// Creates a new variable node that holds the same data type as old
  // with the option to alter binding/semantic types and size.
  //
  // When updateVarList is false, the new VariableNode does not get entered
  // into the current Program's variable lists.
  //
  // When maintainUniform is true, the old.m_uniform is used instead
  // of setting up the value based on current Context state
  VariableNode(const VariableNode& old, BindingType newKind, 
      int newSize, ValueType newValueType, SemanticType newType, 
      bool updateVarList, bool keepUniform);

  // Generates default low bound based on current special type
  Variant* makeLow() const;

  // Generates a default high bound baesd on current special type
  Variant* makeHigh() const;


  void programVarListInit(); /// After kind, size and type are set, this 

  /** Adds this to the declared temps set. 
   * If current parsing program has cfg node (i.e. this is a transformation,
   * not a new program specification), then adds to programs entry node.
   * Else adds an OP_DECL as a dummy statement into current parsing block.
   */
  void programDeclInit(); 
                          

  void add_dependent(VariableNode* dep);
  void remove_dependent(VariableNode* dep);

  void update_dependents();
  void detach_dependencies();
  
  
   
  bool m_uniform;
  BindingType m_kind;
  SemanticType m_specialType;
  ValueType m_valueType;

  int m_size;
  int m_id;
  int m_locked;

  /** Ref-counted pointer to the variant.  VariableNode is always
   * the sole owner of this pointer once the node exists. */
  VariantPtr m_variant;

  // Dependent uniform evaluation
  mutable VariableNodeEval* m_eval;
  std::list<VariableNode*> m_dependents;
  
  static int m_maxID;

#ifdef USE_MEMORY_POOL
  static Pool* m_pool;
#endif
};

}

#endif
