#ifndef SHOPTIMIZATIONS_HPP
#define SHOPTIMIZATIONS_HPP

#include "ShProgram.hpp"
#include "ShStatement.hpp"
#include <vector>
#include <set>

namespace SH {

/// Optimize the program with the given optimization level.
void optimize(ShProgram& p, int level);
void optimize(const ShProgramNodePtr& p, int level);

/// Optimize the program with the current context's default
/// optimization level.
void optimize(ShProgram& p);
void optimize(const ShProgramNodePtr& p);

// Internal stuff.

// Add value tracking information to the given program's CFG
// statements.
// If it already exists, overwrite it.
void add_value_tracking(ShProgram& prg);

/// Insert instructions representing each conditional branch
void insert_branch_instructions(ShProgram& prg);

/// Remove instructions representing conditional branches
void remove_branch_instructions(ShProgram& prg);

/// Merge blocks with redundant edges
void straighten(ShProgram& p, bool& changed);

/// Remove code that serves no purpose in the given program
void remove_dead_code(ShProgram& p, bool& changed);

struct ValueTracking : public ShStatementInfo {
  ValueTracking(ShStatement* stmt);

  ShStatementInfo* clone() const;
  
  struct Def {
    Def(ShStatement* stmt, int index)
      : stmt(stmt), index(index)
    {
    }
    
    ShStatement* stmt;
    int index;

    bool operator<(const Def& other) const
    {
      return stmt < other.stmt || (stmt == other.stmt && index < other.index);
    }
  };
  struct Use {
    Use(ShStatement* stmt, int source, int index)
      : stmt(stmt), source(source), index(index)
    {
    }

    bool operator<(const Use& other) const
    {
      return stmt < other.stmt
        || (stmt == other.stmt && (source < other.source
                                   || (source == other.source && index < other.index)));
    }

    ShStatement* stmt;
    int source; // source variable
    int index; // tuple index
  };

  // For each tuple element, track all of the uses of our definition.
  typedef std::set<Use> DefUseChain;
  typedef std::vector<DefUseChain> TupleDefUseChain;
  TupleDefUseChain uses;
  
  // For each tuple element in each of our sources, track all the
  // definition points.
  typedef std::set<Def> UseDefChain;
  typedef std::vector<UseDefChain> TupleUseDefChain;
  TupleUseDefChain defs[3];
};

}

#endif