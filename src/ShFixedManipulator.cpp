// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
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

ShVariable makeVariable(const ShVariableNodePtr &v, ShVariableKind kind) {
  ShVariable result(new ShVariableNode(kind, v->size(), v->specialType()));
  result.name(v->name());
  return result;
}

ShKeepNode::ShKeepNode(int numChannels) 
  : m_numChannels(numChannels) {
}

ShProgram ShKeepNode::applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  SH_DEBUG_PRINT( "Applying keep " << m_numChannels  );
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      std::cout << "Keep apply to channel: " << (*finger)->nameOfType() << (*finger)->name() << std::endl;
      if(finger == end) {
        ShError(ShAlgebraException("Not enough ShProgram channels for shKeep manipulator"));
      }
      ShVariable output = makeVariable((*finger), SH_VAR_OUTPUT);
      ShVariable input = makeVariable((*finger), SH_VAR_INPUT);
      ShStatement stmt(output, SH_OP_ASN, input);
      ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
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
  SH_DEBUG_PRINT( "Applying lose " << m_numChannels  );
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        ShError(ShAlgebraException("Not enough ShProgram input channels for shLose manipulator"));
      }
      ShVariable output = makeVariable((*finger), SH_VAR_OUTPUT);
    }
    // TODO  remove this when empty connect/combine bugs fixed
    ShAttrib4f dummy = dummy;
  } SH_END;
  return result;
}

ShProgram ShLoseNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    for(int i = 0; i < m_numChannels; ++i, ++finger) {
      if(finger == end) {
        ShError(ShAlgebraException("Not enough ShProgram output channels for shLose manipulator"));
      }
      ShVariable input = makeVariable((*finger), SH_VAR_INPUT);
    }
    // TODO  remove this when empty connect/combine bugs fixed
    ShAttrib4f dummy = dummy;
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
        ShError(ShAlgebraException("Not enough ShProgram input channels for shDup manipulator"));
      }
      if(i == 0) {
        input = makeVariable((*finger), SH_VAR_INPUT);
      }
      if((*finger)->size() != input.size()) {
        ShError(ShAlgebraException("Duplicating type " + input.node()->nameOfType()
              + " to incompatible type " + (*finger)->nameOfType()));
      }
      ShVariable output = makeVariable((*finger), SH_VAR_OUTPUT);
      ShStatement stmt(output, SH_OP_ASN, input);
      ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    }
    // TODO  remove this when empty connect/combine bugs fixed
    ShAttrib4f dummy = dummy;
  } SH_END;
  return result;
}

ShProgram ShDupNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  ShProgram result = SH_BEGIN_PROGRAM() {
    if(finger == end) {
      ShError(ShAlgebraException("Not enough ShProgram output channels for shDup manipulator"));
    }
    ShVariable input = makeVariable((*finger), SH_VAR_INPUT);

    for(int i = 0; i < m_numDups; ++i) {
      ShVariable output = makeVariable((*finger), SH_VAR_OUTPUT);
      ShStatement stmt(output, SH_OP_ASN, input);
      ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    }
    ++finger;
    // TODO  remove this when empty connect/combine bugs fixed
    ShAttrib4f dummy = dummy;
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
  int i;
  for(i = 0; i < p->outputs.size() && finger != end; ++i, ++finger);
  // allow extra outputs from p 
  return p; 
}

ShProgram ShProgramManipNode::applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const {
  int i;
  for(i = 0; i < p->inputs.size() && finger != end; ++i, ++finger);
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
  ShManipVarIterator finger = p->inputs.begin();
  ShProgram manipulator = m->applyToInputs(finger, p->inputs.end()); 
  return p << manipulator;
}

ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p) {
  ShManipVarIterator finger = p->outputs.begin();
  ShProgram manipulator = m->applyToOutputs(finger, p->outputs.end()); 
  return manipulator << p;
}

ShFixedManipulator operator&(const ShFixedManipulator &m, const ShFixedManipulator &n) {
  return new ShTreeManipNode(m, n);
}

ShFixedManipulator operator&(const ShFixedManipulator &m, const ShProgram &p) {
  return m & new ShProgramManipNode(p);
}
ShFixedManipulator operator&(const ShProgram &p, const ShFixedManipulator &m) {
  return new ShProgramManipNode(p) & m; 
}

}

