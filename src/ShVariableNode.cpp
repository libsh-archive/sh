// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <cassert>
#include <algorithm>
#include "ShVariableNode.hpp"
#include "ShDebug.hpp"
#include "ShContext.hpp"
#include "ShVariant.hpp"
#include "ShVariantFactory.hpp"
#include "ShProgramNode.hpp"
#include "ShEvaluate.hpp"

namespace SH {

struct ShVariableNodeEval {
  ShPointer<ShProgramNode> value;

#ifdef SH_USE_MEMORY_POOL
  // Memory pool stuff.
  void* operator new(std::size_t size)
  {
    if (!m_pool) m_pool = new ShPool(sizeof(ShVariableNodeEval), 32768);
    return m_pool->alloc();
  }
  void operator delete(void* ptr)
  {
    m_pool->free(ptr);
  }

  static ShPool* m_pool;
#endif
};

#ifdef SH_USE_MEMORY_POOL
ShPool* ShVariableNodeEval::m_pool = 0;
#endif

ShVariableNode::ShVariableNode(ShBindingType kind, int size, ShValueType valueType, ShSemanticType type)
  : m_uniform(!ShContext::current()->parsing() && kind == SH_TEMP),
    m_kind(kind), m_specialType(type),
    m_valueType(valueType), 
    m_size(size), 
    m_id(m_maxID++), m_locked(0),
    m_variant(0),
    m_eval(0)
{
  if (m_uniform || m_kind == SH_CONST) addVariant();
  programVarListInit();
  programDeclInit();
}

ShVariableNode::ShVariableNode(const ShVariableNode& old, ShBindingType newKind,
          int newSize, ShValueType newValueType, ShSemanticType newType, 
          bool updateVarList, bool keepUniform)
  : ShMeta(old), 
    m_uniform(old.m_uniform), m_kind(newKind), m_specialType(newType),
    m_valueType(newValueType), 
    m_size(newSize), 
    m_id(m_maxID++), m_locked(0),
    m_variant(0),
    m_eval(new ShVariableNodeEval)
{
  if(!keepUniform) {
    m_uniform = !ShContext::current()->parsing() && m_kind == SH_TEMP;
  }

  if(m_uniform || m_kind == SH_CONST) addVariant(); 
  if(updateVarList) programVarListInit();
  programDeclInit();
}

ShVariableNode::~ShVariableNode()
{
  detach_dependencies();
  /* TODO using a smart pointer now - probably shouldn't be though, so leave
   * these here just in case
  delete m_variant; 
  m_variant = 0;
  */
  if(m_eval) delete m_eval;
}

ShVariableNodePtr ShVariableNode::clone(ShBindingType newKind, 
      int newSize, 
      ShValueType newValueType, 
      ShSemanticType newType, 
      bool updateVarList, 
      bool keepUniform) const
{
  SH_DEBUG_ASSERT(newValueType != 0);
  ShBindingType kind = (newKind == SH_BINDINGTYPE_END ? m_kind : newKind);
  int size = (newSize == 0 ? m_size : newSize);
  ShValueType valueType = (newValueType == SH_VALUETYPE_END ? m_valueType : newValueType);
  ShSemanticType type = (newType == SH_SEMANTICTYPE_END ? m_specialType : newType);

  return new ShVariableNode(*this, kind, size, valueType, type, 
      updateVarList, keepUniform);
}

bool ShVariableNode::uniform() const
{
  return m_uniform;
}

bool ShVariableNode::hasValues() const
{
  return (m_variant);
}

void ShVariableNode::lock()
{
  m_locked++;
}

void ShVariableNode::unlock()
{
  m_locked--;
  update_all();
}

ShValueType ShVariableNode::valueType() const {
  return m_valueType;
}

int ShVariableNode::size() const
{
  return m_size;
}

void ShVariableNode::size(int s)
{
  SH_DEBUG_ASSERT(!m_variant);
  m_size = s;
}

void ShVariableNode::name(const std::string& n)
{
  this->ShMeta::name(n);
}

std::string ShVariableNode::name() const
{

  if (has_name()) return this->ShMeta::name();
  
  std::ostringstream stream;
  
  // Special case for constants
  if (m_kind == SH_CONST) {
    return m_variant->encodeArray();
  }

  switch (m_kind) {
  case SH_INPUT:
    stream << "i";
    break;
  case SH_OUTPUT:
    stream << "o";
    break;
  case SH_INOUT:
    stream << "io";
    break;
  case SH_TEMP:
    if(uniform()) stream << "ut";
    else stream << "t";
    break;
  case SH_CONST:
    stream << "c";
    break;
  case SH_TEXTURE:
    stream << "tex";
    break;
  case SH_STREAM:
    stream << "str";
    break;
  case SH_PALETTE:
    stream << "pal";
    break;
  default:
    stream << "??";
  }

  stream << m_id;

  return stream.str();
}

bool ShVariableNode::hasRange()
{
  return has_meta("lowBound") && has_meta("highBound");
}

void ShVariableNode::rangeVariant(const ShVariant* low, const ShVariant* high) 
{
  meta("lowBound", low->encode(0, m_size));
  meta("highBound", high->encode(0, m_size));
}

void ShVariableNode::rangeVariant(const ShVariant* low, const ShVariant* high, 
    bool neg, const ShSwizzle &writemask)
{
  const ShVariantFactory* factory = shVariantFactory(m_valueType);

  std::string oldLo, oldHi;
  oldLo = meta("lowBound");
  oldHi = meta("highBound");

  ShVariant *newLo, *newHi; 
  if(oldLo.empty() || oldHi.empty()) { // TODO they should be either both empty or both not
    newLo = makeLow(); 
    newHi = makeHigh(); 
  } else {
    newLo = factory->generate(oldLo);
    newHi = factory->generate(oldHi);
  }

  // @todo type slow temporary solution
  for(int i = 0; i < writemask.size(); ++i) {
    newLo->set(low, neg, ShSwizzle(m_size, writemask[i]));
    newHi->set(high, neg, ShSwizzle(m_size, writemask[i]));
  }

  meta("lowBound", newLo->encode());
  meta("highBound", newHi->encode());

  delete newLo;
  delete newHi;
}

ShVariantPtr ShVariableNode::lowBoundVariant() const
{
  const ShVariantFactory* factory = shVariantFactory(m_valueType);
  std::string metaLow = meta("lowBound");

  return (metaLow.empty() ? makeLow() : factory->generate(metaLow));
}

ShVariantPtr ShVariableNode::highBoundVariant() const
{
  const ShVariantFactory* factory = shVariantFactory(m_valueType);
  std::string metaHigh = meta("highBound");

  return (metaHigh.empty() ? makeHigh() : factory->generate(metaHigh));
}

ShBindingType ShVariableNode::kind() const
{
  return m_kind;
}

ShSemanticType ShVariableNode::specialType() const
{
  return m_specialType;
}

std::string ShVariableNode::nameOfType() const {
  std::ostringstream os;
  // TODO indicate ValueType properly
  os << "Sh" << bindingTypeName[ m_kind ] << semanticTypeName[ m_specialType ] 
    << m_size << shTypeInfo(m_valueType)->name();
  return os.str();
}

void ShVariableNode::specialType(ShSemanticType type)
{
  m_specialType = type;
}

// TODO: also have an n-length set value, since updating the uniforms
// will otherwise be horribly inefficient.
void ShVariableNode::setVariant(const ShVariant* other)
{
  assert(m_variant);
  m_variant->set(other);
  update_all();
}

void ShVariableNode::setVariant(ShVariantCPtr other)
{
  setVariant(other.object());
}

void ShVariableNode::setVariant(const ShVariant* other, int index) 
{
  assert(m_variant);
  m_variant->set(other, index); 
  update_all();
}

void ShVariableNode::setVariant(ShVariantCPtr other, int index) 
{
  setVariant(other.object(), index);
}

void ShVariableNode::setVariant(const ShVariant* other, bool neg, const ShSwizzle &writemask)
{
  assert(m_variant);
  m_variant->set(other, neg, writemask);
  update_all();
}

void ShVariableNode::setVariant(ShVariantCPtr other, bool neg, const ShSwizzle &writemask)
{
  setVariant(other.object(), neg, writemask);
}

const ShVariant* ShVariableNode::getVariant() const
{
  return m_variant.object();
}


ShVariant* ShVariableNode::getVariant() 
{
  return m_variant.object();
}

ShVariant* ShVariableNode::makeLow() const
{
  const ShVariantFactory* factory = shVariantFactory(m_valueType);
  ShVariant* result;

  switch(m_specialType) {
    case SH_POINT:
    case SH_VECTOR:
    case SH_NORMAL:
    case SH_POSITION:
      result = factory->generateOne(m_size);
      // @todo types handle unsigned types
      result->negate();
      break;
    default:
      result = factory->generateZero(m_size);
  }
  return result;
}

ShVariant* ShVariableNode::makeHigh() const
{
  const ShVariantFactory* factory = shVariantFactory(m_valueType);
  return factory->generateOne(m_size); 
}


void ShVariableNode::update_all() 
{
  if (m_uniform && !m_locked) {
    for (ShBoundIterator I = shBeginBound(); I != shEndBound(); ++I) {
      // TODO: Maybe pass in the backend unit to updateUniform
      if (I->second.node()) I->second.updateUniform(this);
    }

    update_dependents();
  }
}

void ShVariableNode::programVarListInit() 
{
  ShProgramNodePtr prog = ShContext::current()->parsing();
  switch (m_kind) {
  case SH_INPUT:
    assert(prog);
    prog->inputs.push_back(this);
    break;
  case SH_OUTPUT:
    assert(prog);
    prog->outputs.push_back(this);
    break;
  case SH_INOUT:
    assert(prog);
    prog->outputs.push_back(this);
    prog->inputs.push_back(this);
    break;
  case SH_TEMP:
    break;
  default:
    // Do nothing
    break;
  }
}

void ShVariableNode::programDeclInit() 
{
  if (m_kind != SH_TEMP || m_uniform) return;
  ShProgramNodePtr prog = ShContext::current()->parsing();
  if (prog) {
    if (prog->ctrlGraph) { // already has ctrlGraph, add decl to entry
      prog->addDecl(this);
    } else {
      prog->tokenizer.blockList()->addStatement(ShStatement(ShVariable(this), 
                                                            SH_OP_DECL, 
                                                            ShVariable(this)));
    }
  }
}

void ShVariableNode::addVariant()
{
  if (m_variant) return;
  SH_DEBUG_ASSERT(m_valueType != SH_VALUETYPE_END);
  m_variant = shVariantFactory(m_valueType)->generate(m_size);
}

void ShVariableNode::attach(const ShProgramNodePtr& evaluator)
{
  SH_DEBUG_ASSERT(uniform());
  if (!m_eval) m_eval = new ShVariableNodeEval;
  // TODO: Check that the program really evaluates this variable.

  detach_dependencies();

  m_eval->value = evaluator;

  if (m_eval->value) {
    for (ShProgramNode::VarList::const_iterator I = m_eval->value->uniforms_begin();
         I != m_eval->value->uniforms_end(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->add_dependent(this);
    }
    
    update();
  }
}

void ShVariableNode::update()
{
  if (!m_eval) m_eval = new ShVariableNodeEval;
  if (!m_eval->value) return;

  evaluate(m_eval->value);
}

const ShPointer<ShProgramNode>& ShVariableNode::evaluator() const
{
  if (!m_eval) m_eval = new ShVariableNodeEval;
  return m_eval->value;
}

#ifdef SH_USE_MEMORY_POOL
void* ShVariableNode::operator new(std::size_t size)
{
  if (size != sizeof(ShVariableNode)) return ::operator new(size);
  if (!m_pool) m_pool = new ShPool(sizeof(ShVariableNode), 32768);
  return m_pool->alloc();
}

void ShVariableNode::operator delete(void* ptr)
{
  // We won't have a pool if the object is an instance of a class
  // derived from ShVariableNode (ShChannelNode for example)
  if (m_pool) m_pool->free(ptr);
}
#endif

void ShVariableNode::add_dependent(ShVariableNode* dep)
{
  if (std::find(m_dependents.begin(), m_dependents.end(), dep) != m_dependents.end()) return;
  m_dependents.push_back(dep);
}

void ShVariableNode::remove_dependent(ShVariableNode* dep)
{
  m_dependents.remove(dep);
}

void ShVariableNode::update_dependents()
{
  for (std::list<ShVariableNode*>::iterator I = m_dependents.begin();
       I != m_dependents.end(); ++I) {
    (*I)->update();
  }
}

void ShVariableNode::detach_dependencies()
{
  if (!m_eval) return;
  if (m_eval->value) {
    for (ShProgramNode::VarList::const_iterator I = m_eval->value->uniforms_begin();
         I != m_eval->value->uniforms_end(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->remove_dependent(this);
    }
    m_eval->value = 0;
  }
}

int ShVariableNode::m_maxID = 0;
#ifdef SH_USE_MEMORY_POOL
ShPool* ShVariableNode::m_pool = 0;
#endif
}
