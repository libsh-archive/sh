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
#include "ShError.hpp"
#include "ShContext.hpp"
#include "ShCfgBlock.hpp"
#include "ShProgram.hpp"
#include "ShInstructions.hpp"
#include "ShSyntax.hpp"
#include "ShAlgebra.hpp"
#include "ShMemory.hpp"
#include "ShChannelNode.hpp"
#include "ShStream.hpp"
#include "ShRecord.hpp"

namespace SH {

ShRecord::ShRecord()
{
}

ShRecord::ShRecord(const ShVariable& var) 
{
  m_vars.push_back(var);
}

ShRecord::VarList::const_iterator ShRecord::begin() const 
{
  return m_vars.begin();
}

ShRecord::VarList::const_iterator ShRecord::end() const 
{
  return m_vars.end();
}


ShRecord::VarList::iterator ShRecord::begin() 
{
  return m_vars.begin();
}

ShRecord::VarList::iterator ShRecord::end() 
{
  return m_vars.end();
}

size_t ShRecord::size() const 
{
  return m_vars.size();
}

void ShRecord::append(const ShVariable& var) 
{
  m_vars.push_back(var);
}

void ShRecord::prepend(const ShVariable& var) 
{
  m_vars.push_front(var);
}

void ShRecord::append(const ShRecord& rec) 
{
  m_vars.insert(m_vars.end(), rec.begin(), rec.end());
}

void ShRecord::prepend(const ShRecord& rec) 
{
  m_vars.insert(m_vars.begin(), rec.begin(), rec.end());
}

ShRecord combine(const ShVariable& left, const ShVariable& right)
{
  ShRecord result(left);
  result.append(right);
  return result;
}

ShRecord combine(const ShRecord& left, const ShVariable& right)
{
  ShRecord result = left;
  result.append(right);
  return result;
}

ShRecord combine(const ShVariable& left, const ShRecord& right)
{
  ShRecord result = right;
  result.prepend(left);
  return result;
}

ShRecord combine(const ShRecord& left, const ShRecord& right)
{
  ShRecord result = left;
  result.append(right);
  return result;
}

ShRecord operator&(const ShVariable& left, const ShVariable& right)
{
  return combine(left, right);
}

ShRecord operator&(const ShRecord& left, const ShVariable& right)
{
  return combine(left, right);
}

ShRecord operator&(const ShVariable& left, const ShRecord& right)
{
  return combine(left, right);
}

ShRecord operator&(const ShRecord& left, const ShRecord& right)
{
  return combine(left, right);
}

ShRecord& ShRecord::operator=(const ShRecord &other)
{
  if(size() != other.size()) {
    SH_DEBUG_WARN("ShRecord assignment of different sizes");
  }
  VarList::iterator I = begin();
  VarList::const_iterator O = other.begin();

  for(;I != end() && O != other.end(); ++I, ++O) {
    shASN(*I, *O);
  }
  return *this;
}

ShRecord& ShRecord::operator=(const ShProgram& program)
{
  SH_DEBUG_PRINT("Assigning program to record");
  if(!program.node()->inputs.empty()) {
    std::ostringstream out;
    out << "Insufficient arguments for calling an ShProgram." << std::endl; 
    out << "Expected arguments for:" << std::endl;
    ShProgramNode::print(out, program.node()->inputs);

    shError(ShException(out.str()));
  }

  if(ShContext::current()->parsing()) { 
    ShProgram outputMappedProgram = (*this) << program;
    ShContext::current()->parsing()->tokenizer.blockList()->addBlock(
        new ShCfgBlock(outputMappedProgram, false));
  } else { //immediate mode
    // @todo range immediate mode program calls should execute the 
    // ShEval interpreter.
    SH_DEBUG_PRINT("Should call an interpreter here");
  }
  return *this;
}

ShProgram connect(const ShRecord& rec, const ShProgram& program)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    for(ShRecord::VarList::const_iterator I = rec.begin(); I != rec.end(); ++I) {
      ShVariable out(I->node()->clone(SH_OUTPUT, I->size(), SH_VALUETYPE_END, SH_SEMANTICTYPE_END, 
            true, false));
      SH_DEBUG_PRINT("connect ShRecord output size = " << I->size()); 
     ShContext::current()->parsing()->tokenizer.blockList()->addStatement(
         ShStatement(out, SH_OP_ASN, *I));  
    }
  } SH_END;
  return connect(nibble, program);
}

ShProgram operator<<(const ShProgram& program, const ShRecord& rec)
{
  return connect(rec, program);
}

ShProgram connect(const ShProgram& program, const ShRecord& rec)
{
  int oldOpt = ShContext::current()->optimization();
  ShContext::current()->optimization(0);

  ShProgram nibble = SH_BEGIN_PROGRAM() {
    for(ShRecord::VarList::const_iterator I = rec.begin(); I != rec.end(); ++I) {
      ShVariable in(I->node()->clone(SH_INPUT, I->size(), SH_VALUETYPE_END, SH_SEMANTICTYPE_END, 
            true, false));
      SH_DEBUG_PRINT("connect ShRecord input size = " << I->size()); 
      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(
         ShStatement(*I, SH_OP_ASN, in));  
    }
  } SH_END;
  ShProgram result = connect(program, nibble);
  ShContext::current()->optimization(oldOpt);
  return result;
}

ShProgram operator<<(const ShRecord& rec, const ShProgram &program)
{
  return connect(program, rec);
}

}
