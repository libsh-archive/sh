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
#ifndef SHCONTEXT_HPP
#define SHCONTEXT_HPP

#include <string>
#include <map>
#include "ShProgram.hpp"

namespace SH {

// forward declarations
class ShTypeInfo;

class ShContext {
public:
  static ShContext* current();

  /// 0 means no optimizations. The default level is 2.
  int optimization() const;
  void optimization(int level);

  /// Whether exceptions are being thrown instead of error messages
  /// printed to stdout. The default is to throw exceptions.
  bool throw_errors() const;
  void throw_errors(bool on);

  typedef std::map<std::string, ShProgram> BoundProgramMap;

  BoundProgramMap::iterator begin_bound();
  BoundProgramMap::iterator end_bound();

  /// \internal
  void set_binding(const std::string& unit, ShProgram program);

  /// The program currently being constructed. May be null.
  ShProgramNodePtr parsing();

  /// Start constructing the given program
  void enter(const ShProgramNodePtr& program);

  /// Finish constructing the current program
  void exit();

  /// Returns a unique positive integer corresponding to a string type name 
  /// Generates a unique number the first time a type is passed to this function.
  int type_index(const std::string &typeName) const;

  /// Returns number of types currently registered.
  // All type indices lie in the range [1, result]
  int num_types() const;

  ShPointer<ShTypeInfo> type_info(int typeIndex) const;

  /// utility function to add a ShTypeInfo object to the two maps
  /// You must register any custom types here with this function
  // TODO (should be done automatically somehow...)
  void addTypeInfo(ShPointer<ShTypeInfo> typeInfo); 
  
private:
  ShContext();

  int m_optimization;
  bool m_throw_errors;
  
  BoundProgramMap m_bound;
  std::stack<ShProgramNodePtr> m_parsing;
  
  static ShContext* m_instance;

  /*  TODO might want to move all these properties out into a separate holding
   *  class */
  typedef std::map<std::string, int> TypeNameIndexMap;
  typedef std::vector<ShPointer<ShTypeInfo> > TypeInfoVec;

  TypeNameIndexMap m_type_index;
  TypeInfoVec m_type_info;

  // NOT IMPLEMENTED
  ShContext(const ShContext& other);
  ShContext& operator=(const ShContext& other);
};

typedef ShContext::BoundProgramMap::iterator ShBoundIterator;

/// Get beginning of bound program map for current context
ShBoundIterator shBeginBound();

/// Get end of bound program map for current context
ShBoundIterator shEndBound();

/// Returns the ShTypeInfo object for the given type index in
// the current context.
ShPointer<ShTypeInfo> shTypeInfo(int type_index);  

/// Returns the type index of type T in the current context
template<typename T>
int shTypeIndex();

}
#include "ShContextImpl.hpp" // include this higher and things screw up

#endif
