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
#include "ShRefCount.hpp"
#include "ShMeta.hpp"
#include "ShPool.hpp"

namespace SH {

/** The various ways variables can be bound.
 */
enum ShBindingType {
  SH_INPUT = 0,
  SH_OUTPUT = 1,
  SH_INOUT = 2,
  SH_TEMP = 3,
  SH_CONST = 4,
  SH_TEXTURE = 5,
  SH_STREAM = 6,
  SH_PALETTE = 7
};

/** The various ways semantic types for variables.
 */
enum ShSemanticType {
  SH_ATTRIB,
  SH_POINT,
  SH_VECTOR,
  SH_NORMAL,
  SH_COLOR,
  SH_TEXCOORD,
  SH_POSITION
};

// ensure these match the BindingType and SemanticType enums
SH_DLLEXPORT extern const char* ShBindingTypeName[];
SH_DLLEXPORT extern const char* ShSemanticTypeName[];

class ShProgramNode;

// Used to hide our use of ShProgramNodePtr, since MSVC7 doesn't allow it.
struct ShVariableNodeEval;

/** A generic n-tuple variable.
 */
class 
SH_DLLEXPORT ShVariableNode : public virtual ShRefCountable,
                       public virtual ShMeta {
public:
  ShVariableNode(ShBindingType kind, int size, ShSemanticType type = SH_ATTRIB);
  virtual ~ShVariableNode();

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  int size() const; ///< Get the number of elements in this variable

  // Don't call this on uniforms!
  void size(int size);

  void lock(); ///< Do not update bound shaders in subsequent setValue calls
  void unlock(); ///< Update bound shader values, and turn off locking
  

  typedef float ValueType; ///< This is not necessarily correct. Oh well.

  // Metadata
  std::string name() const; ///< Get this variable's name
  void name(const std::string& n); ///< Set this variable's name

  /// Set a range of possible values for this variable's elements
  void range(ShVariableNode::ValueType low, ShVariableNode::ValueType high);
  ShVariableNode::ValueType lowBound() const;
  ShVariableNode::ValueType highBound() const;

  ShBindingType kind() const;
  ShSemanticType specialType() const;
  void specialType(ShSemanticType);

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  /// For variables with values only. Sets the value of the i'th entry.
  /// If i is outside [0, size - 1] this is a no-op.
  ///
  ///@see hasValues()
  void setValue(int i, ValueType value);

  /// Retrieve a particular value
  ValueType getValue(int i) const;

  /// Ensure this node has space to store host-side values.
  /// Normally this is not necessary, but when uniforms are given
  /// dependent programs and evaluated all the temporaries will need
  /// to store values during an evaluation.
  void addValues();

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

  void add_dependent(ShVariableNode* dep);
  void remove_dependent(ShVariableNode* dep);

  void update_dependents();
  void detach_dependencies();
  
  bool m_uniform;
  
  ShBindingType m_kind;
  ShSemanticType m_specialType;
  int m_size;
  int m_id;
  int m_locked;

  ValueType* m_values;

  // Metadata (range)
  ValueType m_lowBound, m_highBound;

  // Dependent uniform evaluation
  mutable ShVariableNodeEval* m_eval;
  std::list<ShVariableNode*> m_dependents;
  
  static int m_maxID;

#ifdef SH_USE_MEMORY_POOL
  static ShPool* m_pool;
#endif
};

typedef ShPointer<ShVariableNode> ShVariableNodePtr;
typedef ShPointer<const ShVariableNode> ShVariableNodeCPtr;

}

#endif
