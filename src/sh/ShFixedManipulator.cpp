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
#include "ShDebug.hpp"
#include "ShVariableNode.hpp"
#include "ShError.hpp"
#include "ShAlgebra.hpp"
#include "ShFixedManipulator.hpp"

namespace SH {

ShFixedManipulatorNode::ShFixedManipulatorNode() {
}

ShFixedManipulatorNode::~ShFixedManipulatorNode() {
}

ShKeepNode::ShKeepNode(int numChannels) 
  : m_numChannels(numChannels) {
}

ShProgram ShKeepNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        shError(ShAlgebraException("Not enough ShProgram channels for shKeep manipulator"));
      }
      ShVariable inout = (*finger)->clone(SH_INOUT);
    }
  } SH_END;
  return result;
}

ShProgram ShKeepNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  return applyToInputs(finger, end);
}

ShFixedManipulator shKeep(int numChannels) {
  return new ShKeepNode(numChannels);
}


ShLoseNode::ShLoseNode(int numChannels) 
  : m_numChannels(numChannels) {
}

ShProgram ShLoseNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  //SH_DEBUG_PRINT( "Applying lose " << m_numChannels  );
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        shError(ShAlgebraException("Not enough ShProgram input channels for shLose manipulator"));
      }
      ShVariable output((*finger)->clone(SH_OUTPUT));
    }
  } SH_END;
  return result;
}

ShProgram ShLoseNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        shError(ShAlgebraException("Not enough ShProgram output channels for shLose manipulator"));
      }
      ShVariable input((*finger)->clone(SH_INPUT));
    }
  } SH_END;
  return result;
}

ShFixedManipulator shLose(int numChannels) {
  return new ShLoseNode(numChannels);
}

ShDupNode::ShDupNode(int numDups) 
  : m_numDups(numDups) {
}

ShProgram ShDupNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariable input;
    for(int i = 0; i < m_numDups; ++i, ++finger) {
      if(finger == end) {
        shError(ShAlgebraException("Not enough ShProgram input channels for shDup manipulator"));
      }
      if(i == 0) {
        input = (*finger)->clone(SH_INPUT);
      }
      if((*finger)->size() != input.size()) {
        shError(ShAlgebraException("Duplicating type " + input.node()->nameOfType()
              + " to incompatible type " + (*finger)->nameOfType()));
      }
      ShVariable output((*finger)->clone(SH_OUTPUT));
      shASN(output, input);
    }
  } SH_END;
  return result;
}

ShProgram ShDupNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    if(finger == end) {
      shError(ShAlgebraException("Not enough ShProgram output channels for shDup manipulator"));
    }
    ShVariable input((*finger)->clone(SH_INPUT));

    for(int i = 0; i < m_numDups; ++i) {
      ShVariable output((*finger)->clone(SH_OUTPUT));
      shASN(output, input);
    }
    ++finger;
  } SH_END;
  return result;
}

ShFixedManipulator shDup(int numDups) {
  return new ShDupNode(numDups);
}

ShProgramManipNode::ShProgramManipNode(const ShProgram &p) 
  : p(p) {
}

ShProgram ShProgramManipNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  std::size_t i;
  for(i = 0; i < p.node()->outputs.size() && finger != end; ++i, ++finger);
  // allow extra outputs from p 
  return p; 
}

ShProgram ShProgramManipNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  std::size_t i;
  for(i = 0; i < p.node()->inputs.size() && finger != end; ++i, ++finger);
  // allow extra inputs from p 
  return p; 
}

ShTreeManipNode::ShTreeManipNode(const ShFixedManipulator &a, const ShFixedManipulator &b) 
  : a(a), b(b) {
  SH_DEBUG_ASSERT(a);
  SH_DEBUG_ASSERT(b);
}

ShProgram ShTreeManipNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram aProgram = a->applyToInputs(finger, end);
  ShProgram bProgram = b->applyToInputs(finger, end);
  return aProgram & bProgram; 
}

ShProgram ShTreeManipNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram aProgram = a->applyToOutputs(finger, end);
  ShProgram bProgram = b->applyToOutputs(finger, end);
  return aProgram & bProgram; 
}

ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m) {
  ShManipVarIterator finger = p.node()->inputs.begin();
  ShProgram manipulator = m->applyToInputs(finger, p.node()->inputs.end()); 
  return p << manipulator;
}

ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p) {
  ShManipVarIterator finger = p.node()->outputs.begin();
  ShProgram manipulator = m->applyToOutputs(finger, p.node()->outputs.end()); 
  return manipulator << p;
}

ShFixedManipulator operator&(const ShFixedManipulator &m, const ShFixedManipulator &n) {
  if (!m) {
    return n;
  }
  if (!n) {
    return m;
  }
  return new ShTreeManipNode(m, n);
}

ShFixedManipulator operator&(const ShFixedManipulator &m, const ShProgram &p) {
  return m & new ShProgramManipNode(p);
}
ShFixedManipulator operator&(const ShProgram &p, const ShFixedManipulator &m) {
  return new ShProgramManipNode(p) & m; 
}

}

