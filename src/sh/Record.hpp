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
#ifndef SHRECORD_HPP
#define SHRECORD_HPP

#include <list>
#include "ShDllExport.hpp"
#include "ShVariable.hpp"

/** @file ShRecord.hpp
 * Basically a duplicate of ShStream.  May want to merge the two in the future if Attribs and Channels gain equivalence.
 */
namespace SH {

/** Dynamic list of variables.
 * The record keep track (by reference) of an ordered lists of 
 * data variables, to be used as inputs and outputs of record operations.
 * @see ShChannel
 */
class
SH_DLLEXPORT ShRecord {
public:
  typedef std::list<ShVariable> VarList; 
  typedef VarList::iterator iterator;
  typedef VarList::const_iterator const_iterator;

  ShRecord();
  ShRecord(const ShVariable &var);
  
  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  size_t size() const;

  void append(const ShVariable& variable);
  void prepend(const ShVariable& variable);

  void append(const ShRecord& rec);
  void prepend(const ShRecord& rec);

  // Assigns one record to another.
  // In retained mode, this inserts assignment statements into the current
  // ShProgram.  
  ShRecord& operator=(const ShRecord &other);

  // Execute fully bound record program and place results in record.
  ShRecord& operator=(const ShProgram& program);

  // Converts a ShRecord containing only uniforms into a stream
  // (Must be unswizzled uniforms (and not constants), as this uses the variable's
  // variant arrays directly as the channel memory
  //
  // If you change any of the values here, you must call 
  // update_all on the nodes
  //ShStream toStream();
  
private:
  VarList m_vars;
};

/** Combine two records.
 * This concatenates the list of variables in the component records.
 */
SH_DLLEXPORT
ShRecord combine(const ShVariable &left, const ShVariable &right);

/** Combine a record and a variable.
 * This concatenates the given variable to the end of the list of
 * variables in the record.
 */
SH_DLLEXPORT
ShRecord combine(const ShRecord& left, const ShVariable& right);

/** Combine a variable and a record.
 * This concatenates the given variable to the start of the list of
 * variables in the record.
 */
SH_DLLEXPORT
ShRecord combine(const ShVariable& left, const ShRecord& right);

SH_DLLEXPORT
ShRecord combine(const ShRecord& left, const ShRecord& right);

/** An operator alias for combine between variables.
 */
SH_DLLEXPORT
ShRecord operator&(const ShVariable& left, const ShVariable& right);

/** An operator alias for combine between a record and a variable.
 */
SH_DLLEXPORT
ShRecord operator&(const ShRecord& left, const ShVariable& right);

/** An operator alias for combine between a variable and a record.
 */
SH_DLLEXPORT
ShRecord operator&(const ShVariable& left, const ShRecord& right);

/** An operator alias for combine between two records.
 */
SH_DLLEXPORT
ShRecord operator&(const ShRecord& left, const ShRecord& right);

/** Apply a program to a record. 
 * This function connects records onto the output of programs
 * TODO: is this right?  why is the record argument first?
 */
SH_DLLEXPORT
ShProgram connect(const ShRecord& rec, const ShProgram& program);

/** An operator alias for connect(p,s).
 */
SH_DLLEXPORT
ShProgram operator<<(const ShProgram& program, const ShRecord& rec);

/** Send program outputs to a record 
 * This should only be used internally as the program the gets generated
 * will be turned into dust by dead code elimination in general usage.
 */
SH_DLLEXPORT
ShProgram connect(const ShProgram& program, const ShRecord& rec);

/** An operator alias for connect(
 */
SH_DLLEXPORT
ShProgram operator<<(const ShRecord& rec, const ShProgram& program);

}

#endif
