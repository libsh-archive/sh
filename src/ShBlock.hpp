#ifndef SHBLOCK_HPP
#define SHBLOCK_HPP

#include <vector>
#include <iosfwd>
#include "ShRefCount.hpp"
#include "ShStatement.hpp"

namespace SH {

/** A generic block or token.
 * These are either basic blocks or tokens.
 */
class ShBlock : public ShRefCountable {
public:
  virtual ~ShBlock();

  /// Output a textual representation of this control statement.
  virtual void print(std::ostream& out, int indent) const = 0;
};

typedef ShRefCount<ShBlock> ShBlockPtr;

class ShBlockList : public ShRefCountable {
public:
  ShBlockList(bool isArgument = false);

  /// True iff this block list is an argument (e.g. to sh_IF)
  bool isArgument();

  /// Add a simple "three variable" statement.
  void addStatement(const ShStatement& statement);
  /// Add a generic block.
  void addBlock(const ShBlockPtr& statement);

  /// Return the front block from the list (does not remove it)
  ShBlockPtr ShBlockList::getFront() const;
  
  /// Remove the front block from the list and return it
  ShBlockPtr ShBlockList::removeFront();

  /// Return true iff this list does not contain any blocks.
  bool empty() const;

  /// Output a token list
  friend std::ostream& operator<<(std::ostream& out, const ShBlockList& blockList);

  /// Output a token list at a given indentation
  std::ostream& print(std::ostream& out, int indentation) const;

private:
  bool m_isArgument;
  std::vector<ShBlockPtr> m_blocks;
};

typedef ShRefCount<ShBlockList> ShBlockListPtr;

}

#endif
