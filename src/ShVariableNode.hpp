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

enum ShVariableKind {
  SH_VAR_INPUT = 0,
  SH_VAR_OUTPUT = 1,
  SH_VAR_TEMP = 2,
  SH_VAR_CONST = 3,
  SH_VAR_TEXTURE = 4,
  SH_VAR_STREAM = 5
};

enum ShVariableSpecialType {
  SH_VAR_ATTRIB,
  SH_VAR_POINT,
  SH_VAR_VECTOR,
  SH_VAR_NORMAL,
  SH_VAR_COLOR,
  SH_VAR_TEXCOORD,
  SH_VAR_POSITION
};

// ensure these match the Kind and SpecialType enums
extern const char* ShVariableKindName[];
extern const char* ShVariableSpecialTypeName[];

/** A generic n-tuple variable.
 */
class ShVariableNode : public ShRefCountable {
public:
  ShVariableNode(ShVariableKind kind, int size, ShVariableSpecialType type = SH_VAR_ATTRIB);
  virtual ~ShVariableNode();

  bool uniform() const; ///< Is this a uniform (non-shader specific) variable?
  bool hasValues() const; ///< Does this variable have values in the
                          ///host, e.g. for constants and uniforms.
  int size() const; ///< Get the number of elements in this variable
  std::string name() const; ///< Get this variable's name

  /// Set this variable's name. If set to the empty string, defaults
  /// to the type and id of the variable.
  void name(const std::string& name);

  ShVariableKind kind() const;
  ShVariableSpecialType specialType() const;
  void specialType(ShVariableSpecialType);

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  typedef float ValueType; ///< This is not necessarily correct. Oh well.
  
  /// For variables with values only. Sets the value of the i'th entry.
  /// If i is outside [0, size - 1] this is a no-op.
  ///
  ///@see hasValues()
  void setValue(int i, ValueType value);

  /// Retrieve a particular value
  ValueType getValue(int i) const;
  
protected:

  bool m_uniform;
  
  ShVariableKind m_kind;
  ShVariableSpecialType m_specialType;
  int m_size;
  int m_id;
  std::string m_name;

  ValueType* m_values;
  
  static int m_maxID;
};

typedef ShRefCount<ShVariableNode> ShVariableNodePtr;

}

#endif
