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
#include <cstdarg>
#include <cassert>
#include <sstream>
#include "ShVariableNode.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShAlgebra.hpp"
#include "ShManipulator.hpp"

namespace SH {

/* TODO - clean up string 
 * Currently these are hacked so that strings can encode some extra offset info other than just the name
 * When the string manipulators are applied, they are converted to int positional manipulators
 * (more code reuse) by finding which input/output channel has the given name.
 * "name" - use position that matches
 * "-name" - use position that matches - 1 (unless it's at beginning, in which case range is ignored)
 * "+name" - use position that matches + 1 (unless it's at end, in which case range is ignored)
 * "\0" - use beginnig
 * "\1" - use end
 */
/* int versions */
template<>
int OffsetRange<int>::absIndex( int index, int offset, const ShProgramNode::VarList &v ) const {
  if( offset == _FIRST ) return 0;
  else if( offset == _SECOND ) return 1;
  else if( offset == _LAST ) return v.size() - 1;
  if( index < 0 ) index += v.size();
  if( index < 0 || index >= static_cast<int>(v.size()) ) return OFFSET_RANGE_BAD_INDEX; 

  index += offset;
  if( index < 0 || index >= static_cast<int>(v.size()) ) return OFFSET_RANGE_BAD_OFFSET; 
  return index;
}

/* string versions */
template<>
int OffsetRange<std::string>::absIndex( std::string index, int offset, 
    const ShProgramNode::VarList &v ) const {
  if( offset == _FIRST ) return 0;
  else if( offset == _SECOND ) return 1;
  else if( offset == _LAST ) return v.size() - 1;

  int result;
  result = OFFSET_RANGE_BAD_INDEX;

  // match first occurrence of the index name
  ShProgramNode::VarList::const_iterator it = v.begin();
  for( int i = 0; it != v.end(); ++it, ++i ) {
    if( (*it)->name() == index && result == OFFSET_RANGE_BAD_INDEX ) {
      result = i; 
      break;
    }
  }

  if( result == OFFSET_RANGE_BAD_INDEX ) {
    std::ostringstream os;
    os << "Could not find index " << index << (offset >= 0 ? "+" : "")
      << offset << " in variable channels:" << v << std::endl;
    shError( ShAlgebraException( os.str()));
  }

  result += offset;
  if( result < 0 || result >= static_cast<int>(v.size())) result = OFFSET_RANGE_BAD_OFFSET;
  return result; 
}

}

