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
  int size() const;


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
