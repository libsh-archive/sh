// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHTOKENIZER_HPP
#define SHTOKENIZER_HPP

#include <stack>
#include <queue>
#include <vector>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShVariable.hpp"
#include "ShBlock.hpp"
#include "ShException.hpp"
#include "ShToken.hpp"

namespace SH {

/** \brief A token argument, e.g. to SH_IF or SH_FOR.
 *
 * Each argument consists of a result variable, containing the value
 * of the argument, and a block list describing how that value is to
 * be computed */
struct 
SH_DLLEXPORT ShTokenArgument {
  ShTokenArgument(const ShVariable& result, const ShBlockListPtr& blockList)
    : result(result), blockList(blockList)
  {
  }
  
  ShVariable result; ///< Contains the value of the argument
  ShBlockListPtr blockList; ///< Specifies how result is computed
};

/** An exception indicating a tokenizer error.
 * Call message() on this exception to determine a detailed report of
 * the error.
 */
class 
SH_DLLEXPORT ShTokenizerException : public ShException {
public:
  ShTokenizerException(const std::string& error);
};
  
/** A tokenizer.
 * This is used during construction of the program, i.e. within a
 * BeginShader/EndShader block, to process control statements in a
 * tokenized fashion.
 *
 * Once the shader has been parsed there is no more need for the
 * tokenizer.
 *
 */
class 
SH_DLLEXPORT ShTokenizer {
public:
  ShTokenizer();

  /** @name Pushing and processing arguments
   * These should always be called together, like so:
   * pushArgQueue() && pushArg() && processArg(foo) && pushArg() &&
   * processArg(bar)
   */
  ///@{
  /// Call this to alllocate an argument queue. Always returns true.
  bool pushArgQueue();
  /// Indicate that an argument is coming. Always returns true.
  bool pushArg();
  /// Tokenize an argument, then add it to the argument queue. Always returns true.
  bool processArg(const ShVariable& result);
  ///@}

  /** @name Retrieving arguments */
  ///@{
  /// Retrieve and remove the oldest parsed argument at the current level
  ShTokenArgument getArgument();
  /// Pop the argument context (call after you've retrieved all your arguments)
  void popArgQueue();
  ///@}

  /// Get the currently active list
  ShBlockListPtr blockList();

private:
  std::stack<ShBlockListPtr> m_listStack;
  std::stack< std::queue<ShTokenArgument> > m_argQueueStack;
};

}

#endif
