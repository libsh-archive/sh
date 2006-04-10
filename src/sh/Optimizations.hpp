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
#ifndef SHOPTIMIZATIONS_HPP
#define SHOPTIMIZATIONS_HPP

#include <iosfwd>
#include <vector>
#include <set>
#include <map>
#include "ShProgram.hpp"
#include "ShInfo.hpp"
#include "ShStatement.hpp"

// Uncomment this to turn on optimizer debugging using dot.
// Warning: This is very verbose!
//#define SH_DEBUG_OPTIMIZER

namespace SH {

/// Optimize the program with the given optimization level.
SH_DLLEXPORT
void optimize(ShProgram& p, int level);
SH_DLLEXPORT
void optimize(const ShProgramNodePtr& p, int level);

/// Optimize the program with the current context's default
/// optimization level.
SH_DLLEXPORT
void optimize(ShProgram& p);
SH_DLLEXPORT
void optimize(const ShProgramNodePtr& p);

// Internal stuff.

// Add value tracking information to the given program's CFG
// statements.
// If it already exists, overwrite it.
SH_DLLEXPORT
void add_value_tracking(ShProgram& prg);

/// Insert instructions representing each conditional branch
SH_DLLEXPORT
void insert_branch_instructions(ShProgram& prg);

/// Remove instructions representing conditional branches
SH_DLLEXPORT
void remove_branch_instructions(ShProgram& prg);

/// Merge blocks with redundant edges
SH_DLLEXPORT
void straighten(ShProgram& p, bool& changed);

/// Remove code that serves no purpose in the given program
SH_DLLEXPORT
void remove_dead_code(ShProgram& p, bool& changed);

/// Propagate constants and lift uniform computations
SH_DLLEXPORT
void propagate_constants(ShProgram& p);

/* per statement uddu chains */
struct 
SH_DLLEXPORT
ValueTracking : public ShInfo {
  ValueTracking(ShStatement* stmt);

  ShInfo* clone() const;

  
  struct Def {
    enum Kind {
      INPUT,
      STMT
    };

    Def(ShStatement* stmt, int index)
      : kind(STMT), node(0), stmt(stmt), index(index)
    {
    }

    Def(const ShVariableNodePtr& node, int index)
      : kind(INPUT), node(node), stmt(0), index(index) 
    {}
    
    Kind kind;
    ShVariableNodePtr node;
    ShStatement* stmt;
    int index; ///< represents swizzled index (no swizzling for INPUTS)

    bool operator<(const Def& other) const
    {
      return kind < other.kind ||
             (kind == other.kind && 
             (node < other.node ||
             (node == other.node &&
             (stmt < other.stmt || 
             (stmt == other.stmt && index < other.index)))));
    }

    int absIndex() const
    {
      if(kind == INPUT) return index;
      return stmt->dest.swizzle()[index];
    }

    friend std::ostream& operator<<(std::ostream& out, const Def& def);
  };

  struct Use {
    enum Kind {
      OUTPUT,
      STMT
    };

    Use(ShStatement* stmt, int source, int index)
      : kind(STMT), node(0), stmt(stmt), source(source), index(index)
    {
    }

    Use(const ShVariableNodePtr& node, int index)
      : kind(OUTPUT), node(node), stmt(0), source(0), index(index)
    {
    }

    bool operator<(const Use& other) const
    {
      return kind < other.kind ||
             (kind == other.kind && 
             (node < other.node ||
             (node == other.node &&
             (stmt < other.stmt || 
             (stmt == other.stmt && 
             (source < other.source ||
             (source == other.source && index < other.index)))))));
    }

    int absIndex() const
    {
      if(kind == OUTPUT) return index;
      return stmt->src[source].swizzle()[index];
    }

    friend std::ostream& operator<<(std::ostream& out, const Use& use);

    Kind kind;
    ShVariableNodePtr node;
    ShStatement* stmt;
    int source; // source variable
    int index; // tuple index (swizzled)
  };

  // For each tuple element, track all of the uses of our definition.
  typedef std::set<Use> DefUseChain;
  typedef std::vector<DefUseChain> TupleDefUseChain;
  TupleDefUseChain uses;

  friend std::ostream& operator<<(std::ostream& out, const TupleDefUseChain& tdu);
  
  // For each tuple element in each of our sources, track all the
  // definition points.
  typedef std::set<Def> UseDefChain;
  typedef std::vector<UseDefChain> TupleUseDefChain;
  typedef std::vector<TupleUseDefChain> TupleUseDefChainVec;
  TupleUseDefChainVec defs;

  friend std::ostream& operator<<(std::ostream& out, const TupleUseDefChain& tud);
};

/* du chains for all inputs in a program */
struct InputValueTracking: public ShInfo 
{
  ShInfo* clone() const;
  
  friend std::ostream& operator<<(std::ostream& out, const InputValueTracking& ivt);

  typedef std::map<ShVariableNodePtr, ValueTracking::TupleDefUseChain> InputTupleDefUseChain; 
  InputTupleDefUseChain inputUses;
};

/* ud chains for all outputs in a program */
struct OutputValueTracking: public ShInfo 
{
  ShInfo* clone() const;

  friend std::ostream& operator<<(std::ostream& out, const OutputValueTracking& ovt);

  typedef std::map<ShVariableNodePtr, ValueTracking::TupleUseDefChain> OutputTupleUseDefChain; 
  OutputTupleUseDefChain outputDefs;
};

}

#endif
