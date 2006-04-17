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
#include "Error.hpp"
#include "Context.hpp"
#include "CfgBlock.hpp"
#include "Program.hpp"
#include "Instructions.hpp"
#include "Syntax.hpp"
#include "Algebra.hpp"
#include "Memory.hpp"
#include "Stream.hpp"
#include "Record.hpp"
#include "BaseTexture.hpp"

namespace SH {

Record::Record()
{
}

Record::Record(const Variable& var) 
{
  m_vars.push_back(var);
}

Record::VarList::const_iterator Record::begin() const 
{
  return m_vars.begin();
}

Record::VarList::const_iterator Record::end() const 
{
  return m_vars.end();
}


Record::VarList::iterator Record::begin() 
{
  return m_vars.begin();
}

Record::VarList::iterator Record::end() 
{
  return m_vars.end();
}

size_t Record::size() const 
{
  return m_vars.size();
}

void Record::append(const Variable& var) 
{
  m_vars.push_back(var);
}

void Record::prepend(const Variable& var) 
{
  m_vars.push_front(var);
}

void Record::append(const Record& rec) 
{
  m_vars.insert(m_vars.end(), rec.begin(), rec.end());
}

void Record::prepend(const Record& rec) 
{
  m_vars.insert(m_vars.begin(), rec.begin(), rec.end());
}

Record combine(const Variable& left, const Variable& right)
{
  Record result(left);
  result.append(right);
  return result;
}

Record combine(const Record& left, const Variable& right)
{
  Record result = left;
  result.append(right);
  return result;
}

Record combine(const Variable& left, const Record& right)
{
  Record result = right;
  result.prepend(left);
  return result;
}

Record combine(const Record& left, const Record& right)
{
  Record result = left;
  result.append(right);
  return result;
}

Record operator&(const Variable& left, const Variable& right)
{
  return combine(left, right);
}

Record operator&(const Record& left, const Variable& right)
{
  return combine(left, right);
}

Record operator&(const Variable& left, const Record& right)
{
  return combine(left, right);
}

Record operator&(const Record& left, const Record& right)
{
  return combine(left, right);
}

Record& Record::operator=(const Record &other)
{
  VarList::iterator I = begin();
  VarList::const_iterator O = other.begin();

  for(;I != end() && O != other.end(); ++I, ++O) {
    shASN(*I, *O);
  }
  return *this;
}

Record& Record::operator=(const Program& program)
{
  //DEBUG_PRINT("Assigning program to record");
  if(!program.node()->inputs.empty()) {
    std::ostringstream out;
    out << "Insufficient arguments for calling an Program." << std::endl; 
    out << "Expected arguments for:" << std::endl;
    ProgramNode::print(out, program.node()->inputs);

    error(Exception(out.str()));
  }

  if(Context::current()->parsing()) { 
    Program outputMappedProgram = (*this) << program;
    Context::current()->parsing()->tokenizer.blockList()->addBlock(
        new CfgBlock(outputMappedProgram, false));
  } else { //immediate mode
    // @todo range immediate mode program calls should execute the 
    // Eval interpreter.
    DEBUG_PRINT("Should call an interpreter here");
  }
  return *this;
}

Program connect(const Record& rec, const Program& program)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    for(Record::VarList::const_iterator I = rec.begin(); I != rec.end(); ++I) {
      Variable out(I->node()->clone(SH_OUTPUT, I->size(), VALUETYPE_END, SEMANTICTYPE_END, 
        true, false));
      //DEBUG_PRINT("connect Record output size = " << I->size()); 
      Context::current()->parsing()->tokenizer.blockList()->addStatement(
        Statement(out, OP_ASN, *I));  
    }
  } SH_END;
  return connect(nibble, program);
}

Program operator<<(const Program& program, const Record& rec)
{
  return connect(rec, program);
}

Program connect(const Program& program, const Record& rec)
{
  int oldOpt = Context::current()->optimization();
  Context::current()->optimization(0);

  Program nibble = SH_BEGIN_PROGRAM() {
    for(Record::VarList::const_iterator I = rec.begin(); I != rec.end(); ++I) {
      Variable in(I->node()->clone(SH_INPUT, I->size(), VALUETYPE_END, SEMANTICTYPE_END, 
            true, false));
      //DEBUG_PRINT("connect Record input size = " << I->size()); 
      Context::current()->parsing()->tokenizer.blockList()->addStatement(
         Statement(*I, OP_ASN, in));  
    }
  } SH_END;
  Program result = connect(program, nibble);
  Context::current()->optimization(oldOpt);
  return result;
}

Program operator<<(const Record& rec, const Program &program)
{
  return connect(program, rec);
}

}
