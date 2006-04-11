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
#ifndef SHFIXEDMANIPULATOR_HPP
#define SHFIXEDMANIPULATOR_HPP

#include <vector>
#include "DllExport.hpp"
#include "Program.hpp"

namespace SH {

typedef ProgramNode::VarList::const_iterator ManipVarIterator;

/** A FixedManipulator is a Program output manipulator. 
 * Fixed size manipulators can be combined with each other
 * to give "wider" fixed size manipulators that handle more 
 * Program outputs.
 */
class
SH_DLLEXPORT FixedManipulatorNode: public RefCountable { 
  public:
    FixedManipulatorNode();
    virtual ~FixedManipulatorNode(); 

    /** Define the manipulator's behaviour on inputs.
     * Consumes a number of inputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an AlgebraException
     */
    virtual Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const = 0;

    /** Defines the manipulator's behaviour on outputs. 
     * Consumes a number of outputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an AlgebraException
     */
    virtual Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const = 0;
};
typedef Pointer<FixedManipulatorNode> FixedManipulator;

class
SH_DLLEXPORT KeepNode: public FixedManipulatorNode {
  public:
    KeepNode(int numChannels); 
    Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const;
    Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const;

  private:
    int m_numChannels;
};
SH_DLLEXPORT
FixedManipulator keep(int numChannels = 1);

class
SH_DLLEXPORT LoseNode: public FixedManipulatorNode {
  public:
    LoseNode(int numChannels); 
    Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const;
    Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const;

  private:
    int m_numChannels;
};
SH_DLLEXPORT
FixedManipulator lose(int numChannels = 1);

class
SH_DLLEXPORT DupNode: public FixedManipulatorNode {
  public:
    DupNode(int numDups);
    Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const;
    Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const;

  private:
    int m_numDups;
};
SH_DLLEXPORT
FixedManipulator dup(int numDups = 2);

// TODO make class names less clunky
// This node can only be created by using the & operator with another fixed manipulator
//
// This is the only manip node that allows number of channels
// not to match when connected to a Program. (extras are handled in the
// default connect way when inputs != outpus)
//
class
SH_DLLEXPORT ProgramManipNode: public FixedManipulatorNode {
  public:
    Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const;
    Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const;

  private:
    Program p;

    ProgramManipNode(const Program &p);

  friend SH_DLLEXPORT
  FixedManipulator operator&(const FixedManipulator &m, const Program &p );
  friend SH_DLLEXPORT
  FixedManipulator operator&( const Program &p, const FixedManipulator &m);
};

// This node can only be created using the & operator with another fixed manipulator
class TreeManipNode: public FixedManipulatorNode {
  public:
    Program applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const;
    Program applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const;
  private:
    FixedManipulator a, b;

    TreeManipNode(const FixedManipulator &a, const FixedManipulator &b); 

    friend SH_DLLEXPORT

    FixedManipulator operator&(const FixedManipulator &m, const FixedManipulator &n);
};

SH_DLLEXPORT
Program operator<<(const Program &p, const FixedManipulator &m); 
SH_DLLEXPORT
Program operator<<(const FixedManipulator &m, const Program &p);
SH_DLLEXPORT
FixedManipulator operator&(const FixedManipulator &m, 
			     const FixedManipulator &n);
SH_DLLEXPORT
FixedManipulator operator&(const FixedManipulator &m, 
			     const Program &p );
SH_DLLEXPORT
FixedManipulator operator&(const Program &p, 
			     const FixedManipulator &m);

}

#endif
