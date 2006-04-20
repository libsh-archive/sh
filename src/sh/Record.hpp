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
#include "DllExport.hpp"
#include "Variable.hpp"

/** @file Record.hpp
 * Basically a duplicate of Stream.  May want to merge the two in the future if Attribs and Channels gain equivalence.
 */
namespace SH {

/** Dynamic list of variables.
 * The record keep track (by reference) of an ordered lists of 
 * data variables, to be used as inputs and outputs of record operations.
 * @see Channel
 */
class
SH_DLLEXPORT Record {
public:
  typedef std::list<Variable> VarList; 
  typedef VarList::iterator iterator;
  typedef VarList::const_iterator const_iterator;

  Record();
  Record(const Variable &var);
  
  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  size_t size() const;

  void append(const Variable& variable);
  void prepend(const Variable& variable);

  void append(const Record& rec);
  void prepend(const Record& rec);

  // Assigns one record to another.
  // In retained mode, this inserts assignment statements into the current
  // Program.  
  Record& operator=(const Record &other);

  // Execute fully bound record program and place results in record.
  Record& operator=(const Program& program);

  // Converts a Record containing only uniforms into a stream
  // (Must be unswizzled uniforms (and not constants), as this uses the variable's
  // variant arrays directly as the channel memory
  //
  // If you change any of the values here, you must call 
  // update_all on the nodes
  //Stream toStream();
  
private:
  VarList m_vars;
};

/** Combine two records.
 * This concatenates the list of variables in the component records.
 */
SH_DLLEXPORT
Record combine(const Variable &left, const Variable &right);

/** Combine a record and a variable.
 * This concatenates the given variable to the end of the list of
 * variables in the record.
 */
SH_DLLEXPORT
Record combine(const Record& left, const Variable& right);

/** Combine a variable and a record.
 * This concatenates the given variable to the start of the list of
 * variables in the record.
 */
SH_DLLEXPORT
Record combine(const Variable& left, const Record& right);

SH_DLLEXPORT
Record combine(const Record& left, const Record& right);

/** An operator alias for combine between variables.
 */
SH_DLLEXPORT
Record operator&(const Variable& left, const Variable& right);

/** An operator alias for combine between a record and a variable.
 */
SH_DLLEXPORT
Record operator&(const Record& left, const Variable& right);

/** An operator alias for combine between a variable and a record.
 */
SH_DLLEXPORT
Record operator&(const Variable& left, const Record& right);

/** An operator alias for combine between two records.
 */
SH_DLLEXPORT
Record operator&(const Record& left, const Record& right);

/** Apply a program to a record. 
 * This function connects records onto the output of programs
 */
SH_DLLEXPORT
Program connect(const Record& rec, const Program& program);
// TODO: is this right?  why is the record argument first?

/** An operator alias for connect(p,s).
 */
SH_DLLEXPORT
Program operator<<(const Program& program, const Record& rec);

/** Send program outputs to a record 
 * This should only be used internally as the program the gets generated
 * will be turned into dust by dead code elimination in general usage.
 */
SH_DLLEXPORT
Program connect(const Program& program, const Record& rec);

/** An operator alias for connect(
 */
SH_DLLEXPORT
Program operator<<(const Record& rec, const Program& program);

}

#endif
