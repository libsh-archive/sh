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
SH_DLLEXPORT
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
SH_DLLEXPORT
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
      << offset << " in variable channels:";
    ShProgramNode::print(os, v) << std::endl;
    shError( ShAlgebraException( os.str()));
  }

  result += offset;
  if( result < 0 || result >= static_cast<int>(v.size())) result = OFFSET_RANGE_BAD_OFFSET;
  return result; 
}

}

