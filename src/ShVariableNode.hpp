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
#include "ShVariableType.hpp"
#include "ShRefCount.hpp"
#include "ShMeta.hpp"
#include "ShSwizzle.hpp"

namespace SH {


// ensure these match the BindingType and SemanticType enums
extern const char* ShBindingTypeName[];
extern const char* ShSemanticTypeName[];

// forward declarations 
class ShCloak;

class ShVariableNode;
typedef ShPointer<ShVariableNode> ShVariableNodePtr;
typedef ShPointer<const ShVariableNode> ShVariableNodeCPtr;

/** A generic n-tuple variable.
 */
class ShVariableNode : public virtual ShRefCountable,
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
  // TODO Currently this does not work if the newKind is SH_CONST
  // and old kind was not CONST/uniform since the old one won't have 
  // a ShCloak node to clone...
  // To fix this, VariableNode should hold the type_index of its data
  // as well, so that it can generate new cloaks on demand.
  // @{
  ShVariableNodePtr clone(ShBindingType newKind, 
      bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(ShBindingType newKind, ShSemanticType newType, 
      bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(ShBindingType newKind, ShSemanticType newType, 
      int newSize, bool updateVarList = true, bool keepUniform = true) const;

  ShVariableNodePtr clone(int newTypeIndex, 
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
  void rangeCloak(ShPointer<const ShCloak> low, ShPointer<const ShCloak> high);
  void rangeCloak(ShPointer<const ShCloak> low, ShPointer<const ShCloak> high, bool neg, const ShSwizzle &writemask);

  ShPointer<ShCloak> lowBoundCloak() const;
  ShPointer<ShCloak> highBoundCloak() const;

  ShBindingType kind() const;
  ShSemanticType specialType() const;
  void specialType(ShSemanticType);

  std::string nameOfType() const; ///< Get a string of this var's specialType, kind, & size

  /// Update elements of a cloak applying the given writemask and negation
  void setCloak(ShPointer<const ShCloak> other);
  void setCloak(ShPointer<const ShCloak> other, bool neg, const ShSwizzle &writemask);

  /// Retrieve the cloak 
  ShPointer<const ShCloak> cloak() const;
  //ShDataCloakPtr cloak(); // TODO can't have this function until ShUpdate is implemented, and even then it might not be a good idea
  
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
  void uniformUpdate(); /// Updates a uniform in currently bound shaders
  void programVarListInit(); /// After kind, size and type are set, this 

  bool m_uniform;
  
  ShBindingType m_kind;
  ShSemanticType m_specialType;
  int m_size;
  int m_typeIndex;
  int m_id;
  int m_locked;

  ShPointer<ShCloak> m_cloak;
  static int m_maxID;
};


}

#endif
