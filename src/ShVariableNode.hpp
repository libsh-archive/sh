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
#include "ShRefCount.hpp"

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
  SH_STREAM = 6
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
extern const char* ShBindingTypeName[];
extern const char* ShSemanticTypeName[];

/** A generic n-tuple variable.
 */
class ShVariableNode : public ShRefCountable {
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
  
  std::string name() const; ///< Get this variable's name

  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);

  /// Returns true iff. this variable has been assigned an actual name.
  bool hasName();
  
  typedef float ValueType; ///< This is not necessarily correct. Oh well.

  // Metadata
  void range(ShVariableNode::ValueType low, ShVariableNode::ValueType high);
  ShVariableNode::ValueType lowBound() const;
  ShVariableNode::ValueType highBound() const;
  void internal(bool setting);
  bool internal() const;

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
  
protected:

  bool m_uniform;
  
  ShBindingType m_kind;
  ShSemanticType m_specialType;
  int m_size;
  int m_id;
  std::string m_name;
  int m_locked;

  // Metadata
  ValueType* m_values;
  ValueType m_lowBound, m_highBound;
  bool m_internal;
  
  static int m_maxID;
};

typedef ShPointer<ShVariableNode> ShVariableNodePtr;

}

#endif
