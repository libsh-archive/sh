#ifndef SHOPTIMIZATIONS_HPP
#define SHOPTIMIZATIONS_HPP

#include "ShProgram.hpp"
#include "ShStatement.hpp"
#include <vector>
#include <set>

namespace SH {

// Add value tracking information to the given program's CFG
// statements.
// If it already exists, overwrite it.
void add_value_tracking(ShProgram& prg);

struct ValueTracking : public ShStatementInfo {
  ValueTracking(ShStatement* stmt);

  struct Def {
    int index;
    ShStatement* stmt;
  };
  struct Use {
    int source;
    int index;
    ShStatement* stmt;
  };

  // For each tuple element, track all of the uses of our definition.
  std::vector< std::set<Use> > uses;
  // For each tuple element in each of our sources, track all the
  // definition points.
  std::vector< std::set<Def> > defs[3];
};

}

#endif
