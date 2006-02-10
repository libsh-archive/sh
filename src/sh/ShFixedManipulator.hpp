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
#include "ShDllExport.hpp"
#include "ShProgram.hpp"

namespace SH {

typedef ShProgramNode::VarList::const_iterator ShManipVarIterator;

/** A ShFixedManipulator is a ShProgram output manipulator. 
 * Fixed size manipulators can be combined with each other
 * to give "wider" fixed size manipulators that handle more 
 * ShProgram outputs.
 */
class
SH_DLLEXPORT ShFixedManipulatorNode: public ShRefCountable { 
  public:
    ShFixedManipulatorNode();
    virtual ~ShFixedManipulatorNode(); 

    /** Define the manipulator's behaviour on inputs.
     * Consumes a number of inputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an ShAlgebraException
     */
    virtual ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const = 0;

    /** Defines the manipulator's behaviour on outputs. 
     * Consumes a number of outputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an ShAlgebraException
     */
    virtual ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const = 0;
};
typedef ShPointer<ShFixedManipulatorNode> ShFixedManipulator;

class
SH_DLLEXPORT ShKeepNode: public ShFixedManipulatorNode {
  public:
    ShKeepNode(int numChannels); 
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numChannels;
};
SH_DLLEXPORT
ShFixedManipulator shKeep(int numChannels = 1);

class
SH_DLLEXPORT ShLoseNode: public ShFixedManipulatorNode {
  public:
    ShLoseNode(int numChannels); 
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numChannels;
};
SH_DLLEXPORT
ShFixedManipulator shLose(int numChannels = 1);

class
SH_DLLEXPORT ShDupNode: public ShFixedManipulatorNode {
  public:
    ShDupNode(int numDups);
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numDups;
};
SH_DLLEXPORT
ShFixedManipulator shDup(int numDups = 2);

// TODO make class names less clunky
// This node can only be created by using the & operator with another fixed manipulator
//
// This is the only manip node that allows number of channels
// not to match when connected to a ShProgram. (extras are handled in the
// default connect way when inputs != outpus)
//
class
SH_DLLEXPORT ShProgramManipNode: public ShFixedManipulatorNode {
  public:
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    ShProgram p;

    ShProgramManipNode(const ShProgram &p);

  friend SH_DLLEXPORT
  ShFixedManipulator operator&(const ShFixedManipulator &m, const ShProgram &p );
  friend SH_DLLEXPORT
  ShFixedManipulator operator&( const ShProgram &p, const ShFixedManipulator &m);
};

// This node can only be created using the & operator with another fixed manipulator
class ShTreeManipNode: public ShFixedManipulatorNode {
  public:
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
  private:
    ShFixedManipulator a, b;

    ShTreeManipNode(const ShFixedManipulator &a, const ShFixedManipulator &b); 

    friend SH_DLLEXPORT

    ShFixedManipulator operator&(const ShFixedManipulator &m, const ShFixedManipulator &n);
};

SH_DLLEXPORT
ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m); 
SH_DLLEXPORT
ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p);
SH_DLLEXPORT
ShFixedManipulator operator&(const ShFixedManipulator &m, 
			     const ShFixedManipulator &n);
SH_DLLEXPORT
ShFixedManipulator operator&(const ShFixedManipulator &m, 
			     const ShProgram &p );
SH_DLLEXPORT
ShFixedManipulator operator&(const ShProgram &p, 
			     const ShFixedManipulator &m);

}

#endif
