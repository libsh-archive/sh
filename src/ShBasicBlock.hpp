#ifndef SHBASICBLOCK_HPP
#define SHBASICBLOCK_HPP

#include "ShBlock.hpp"
#include "ShStatement.hpp"

namespace SH {

/** A basic block.
 * A basic block is a block composed of * simpler statements.
 */
class ShBasicBlock : public ShBlock {
public:
  ~ShBasicBlock();

  void print(std::ostream& out, int indent) const;
  void graphvizDump(std::ostream& out) const;
  
  void addStatement(const ShStatement& stmt);

  typedef std::vector<ShStatement> ShStmtList;

  ShStmtList::const_iterator begin() const;
  ShStmtList::const_iterator end() const;
  
private:
  ShStmtList m_statements;
};

typedef ShRefCount<ShBasicBlock> ShBasicBlockPtr;

}

#endif
