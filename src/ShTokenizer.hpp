#ifndef SHTOKENIZER_HPP
#define SHTOKENIZER_HPP

#include <stack>
#include <queue>
#include <vector>
#include <iosfwd>
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
struct ShTokenArgument {
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
class ShTokenizerException : public ShException {
public:
  ShTokenizerException(const std::string& error);
};
  
/** A tokenizer.
 * Mostly a stack of ShBlockLists, with some argument separation facilities.
 */
class ShTokenizer {
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
