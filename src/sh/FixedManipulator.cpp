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

#include <sstream>
#include "Debug.hpp"
#include "VariableNode.hpp"
#include "Error.hpp"
#include "Algebra.hpp"
#include "FixedManipulator.hpp"

namespace SH {

FixedManipulatorNode::FixedManipulatorNode() {
}

FixedManipulatorNode::~FixedManipulatorNode() {
}

KeepNode::KeepNode(int numChannels) 
  : m_numChannels(numChannels) {
}

Program KeepNode::applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        error(AlgebraException("Not enough Program channels for keep manipulator"));
      }
      Variable inout = (*finger)->clone(SH_INOUT);
    }
  } SH_END;
  return result;
}

Program KeepNode::applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const {
  return applyToInputs(finger, end);
}

FixedManipulator keep(int numChannels) {
  return new KeepNode(numChannels);
}


LoseNode::LoseNode(int numChannels) 
  : m_numChannels(numChannels) {
}

Program LoseNode::applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const {
  //DEBUG_PRINT( "Applying lose " << m_numChannels  );
  Program result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        error(AlgebraException("Not enough Program input channels for lose manipulator"));
      }
      Variable output((*finger)->clone(SH_OUTPUT));
    }
  } SH_END;
  return result;
}

Program LoseNode::applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        error(AlgebraException("Not enough Program output channels for lose manipulator"));
      }
      Variable input((*finger)->clone(SH_INPUT));
    }
  } SH_END;
  return result;
}

FixedManipulator lose(int numChannels) {
  return new LoseNode(numChannels);
}

DupNode::DupNode(int numDups) 
  : m_numDups(numDups) {
}

Program DupNode::applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program result = SH_BEGIN_PROGRAM() {
    Variable input;
    for(int i = 0; i < m_numDups; ++i, ++finger) {
      if(finger == end) {
        error(AlgebraException("Not enough Program input channels for dup manipulator"));
      }
      if(i == 0) {
        input = (*finger)->clone(SH_INPUT);
      }
      if((*finger)->size() != input.size()) {
        error(AlgebraException("Duplicating type " + input.node()->nameOfType()
              + " to incompatible type " + (*finger)->nameOfType()));
      }
      Variable output((*finger)->clone(SH_OUTPUT));
      shASN(output, input);
    }
  } SH_END;
  return result;
}

Program DupNode::applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program result = SH_BEGIN_PROGRAM() {
    if(finger == end) {
      error(AlgebraException("Not enough Program output channels for dup manipulator"));
    }
    Variable input((*finger)->clone(SH_INPUT));

    for(int i = 0; i < m_numDups; ++i) {
      Variable output((*finger)->clone(SH_OUTPUT));
      shASN(output, input);
    }
    ++finger;
  } SH_END;
  return result;
}

FixedManipulator dup(int numDups) {
  return new DupNode(numDups);
}

ProgramManipNode::ProgramManipNode(const Program &p) 
  : p(p) {
}

Program ProgramManipNode::applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const {
  std::size_t i;
  for(i = 0; i < p.node()->outputs.size() && finger != end; ++i, ++finger);
  // allow extra outputs from p 
  return p; 
}

Program ProgramManipNode::applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const {
  std::size_t i;
  for(i = 0; i < p.node()->inputs.size() && finger != end; ++i, ++finger);
  // allow extra inputs from p 
  return p; 
}

TreeManipNode::TreeManipNode(const FixedManipulator &a, const FixedManipulator &b) 
  : a(a), b(b) {
  DEBUG_ASSERT(a);
  DEBUG_ASSERT(b);
}

Program TreeManipNode::applyToInputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program aProgram = a->applyToInputs(finger, end);
  Program bProgram = b->applyToInputs(finger, end);
  return aProgram & bProgram; 
}

Program TreeManipNode::applyToOutputs(ManipVarIterator &finger, ManipVarIterator end) const {
  Program aProgram = a->applyToOutputs(finger, end);
  Program bProgram = b->applyToOutputs(finger, end);
  return aProgram & bProgram; 
}

Program operator<<(const Program &p, const FixedManipulator &m) {
  ManipVarIterator finger = p.node()->inputs.begin();
  Program manipulator = m->applyToInputs(finger, p.node()->inputs.end()); 
  return p << manipulator;
}

Program operator<<(const FixedManipulator &m, const Program &p) {
  ManipVarIterator finger = p.node()->outputs.begin();
  Program manipulator = m->applyToOutputs(finger, p.node()->outputs.end()); 
  return manipulator << p;
}

FixedManipulator operator&(const FixedManipulator &m, const FixedManipulator &n) {
  if (!m) {
    return n;
  }
  if (!n) {
    return m;
  }
  return new TreeManipNode(m, n);
}

FixedManipulator operator&(const FixedManipulator &m, const Program &p) {
  return m & new ProgramManipNode(p);
}
FixedManipulator operator&(const Program &p, const FixedManipulator &m) {
  return new ProgramManipNode(p) & m; 
}

}

