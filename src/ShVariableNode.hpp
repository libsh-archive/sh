// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHVARIABLENODE_HPP
#define SHVARIABLENODE_HPP

#include <string>
#include <list>
#include "ShDllExport.hpp"
#include "ShVariableType.hpp"
#include "ShRefCount.hpp"
#include "ShMeta.hpp"
#include "ShSwizzle.hpp"
#include "ShPool.hpp"

namespace SH {


// @todo type 
// move these over to ShVariableType.hpp
// ensure these match the BindingType and SemanticType enums
SH_DLLEXPORT extern const char* ShBindingTypeName[];
SH_DLLEXPORT extern const char* ShSemanticTypeName[];

// forward declarations 
class ShVariant;

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
  //given by the typeIndex.
  ShVariableNode(ShBindingType kind, int size, int typeIndex, ShSemanticType type = SH_ATTRIB);

  virtual ~ShVariableNode();

  /// Clones this ShVariableNode, copying all fields and meta data
  // except it uses the specified fields in the place of the originals. 
  //
  // If updateVarList is set to false, then the clone is not added to the
  // current ShProgramNode's variable lists.  You *must* add it in manually 
  // for INPUT/OUTPUT/INOUT types.
  //
  // If keepUniform is set to false, then the new variable  
  // has m_uniform set to false even if the original m_uniform was true. 
  //
  // @todo Currently this does not work if the newKind is SH_CONST
  // and old kind was not CONST/uniform since the old one won't have 
  // a ShVariant node to clone...
  // To fix this, VariableNode should hold the type_index of its data
  // as well, so that it can generate new variants on demand.
  //
  // @todo clean this up...eventually we will need dozens of these functions...
  // @{
  ShVariableNodePtr clone(ShBindingType newKind, 
      bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(ShBindingType newKind, ShSemanticType newType, 
      bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(ShBindingType newKind, ShSemanticType newType, 
      int newSize, bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(int newTypeIndex, 
      bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(ShBindingType newKind, int newSize, int newTypeIndex, 
      bool updateVarList = true, bool keepUniform = true) const;
  // @}

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  int size() const; ///< Get the number of elements in this variable

  // Don't call this on uniforms!
  void size(int size);

  void lock(); ///< Do not update bound shaders in subsequent setValue calls
  void unlock(); ///< Update bound shader values, and turn off locking
  
  int typeIndex() const; ///< Returns index of the data type held in this node 

  // Metadata
  std::string name() const; ///< Get this variable's name
  void name(const std::string& n); ///< Set this variable's name

  /// Set a range of possible values for this variable's elements
  // low and high must be scalar elements (otherwise this just uses the first component)
  void rangeVariant(ShPointer<const ShVariant> low, ShPointer<const ShVariant> high);
  void rangeVariant(ShPointer<const ShVariant> low, ShPointer<const ShVariant> high, bool neg, const ShSwizzle &writemask);

  ShPointer<ShVariant> lowBoundVariant() const;
  ShPointer<ShVariant> highBoundVariant() const;

  ShBindingType kind() const;
  ShSemanticType specialType() const;
  void specialType(ShSemanticType);

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  /// Update elements of a variant applying the given writemask and negation
  void setVariant(ShPointer<const ShVariant> other);
  void setVariant(ShPointer<const ShVariant> other, int index);
  void setVariant(ShPointer<const ShVariant> other, bool neg, const ShSwizzle &writemask);

  /// Retrieve the variant 
  ShPointer<const ShVariant> getVariant() const;
  //ShDataVariantPtr getVariant(); // TODO can't have this function until ShUpdate is implemented, and even then it might not be a good idea

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

  /// Obtain the program defining this uniform, if any.
  const ShPointer<ShProgramNode>& evaluator() const;
  
  /** @} */

#ifdef SH_USE_MEMORY_POOL
  // Memory pool stuff.
  void* operator new(std::size_t size);
  void operator delete(void* d);
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
      ShSemanticType newType, int newSize, int newTypeIndex, 
      bool updateVarList, bool keepUniform);
  // @todo type find a better function name for this
  // (Later this should just update the backends as part of shUpdate 
  // and all calls to this should be replaced with update_dependents) 
  void update_all(); /// Updates a uniform in currently bound shaders and all dependencies

  void programVarListInit(); /// After kind, size and type are set, this 

  void add_dependent(ShVariableNode* dep);
  void remove_dependent(ShVariableNode* dep);

  void update_dependents();
  void detach_dependencies();
  
  bool m_uniform;
  
  ShBindingType m_kind;
  ShSemanticType m_specialType;
  int m_size;
  int m_typeIndex;
  int m_id;
  int m_locked;

  ShPointer<ShVariant> m_variant;

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
