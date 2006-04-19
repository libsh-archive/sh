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
#include <cassert>
#include <algorithm>
#include "VariableNode.hpp"
#include "Debug.hpp"
#include "Context.hpp"
#include "Variant.hpp"
#include "VariantFactory.hpp"
#include "ProgramNode.hpp"
#include "Evaluate.hpp"

namespace SH {

struct VariableNodeEval {
  Pointer<ProgramNode> value;

#ifdef USE_MEMORY_POOL
  // Memory pool stuff.
  void* operator new(std::size_t size)
  {
    if (!m_pool) m_pool = new Pool(sizeof(VariableNodeEval), 32768);
    return m_pool->alloc();
  }
  void operator delete(void* ptr)
  {
    m_pool->free(ptr);
  }

  static Pool* m_pool;
#endif
};

#ifdef USE_MEMORY_POOL
Pool* VariableNodeEval::m_pool = 0;
#endif

VariableNode::VariableNode(BindingType kind, int size, ValueType valueType, SemanticType type)
  : m_uniform(!Context::current()->parsing() && kind == SH_TEMP),
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

VariableNode::VariableNode(const VariableNode& old, BindingType newKind,
          int newSize, ValueType newValueType, SemanticType newType, 
          bool updateVarList, bool keepUniform)
  : Meta(old), 
    m_uniform(old.m_uniform), m_kind(newKind), m_specialType(newType),
    m_valueType(newValueType), 
    m_size(newSize), 
    m_id(m_maxID++), m_locked(0),
    m_variant(0),
    m_eval(new VariableNodeEval)
{
  if(!keepUniform) {
    m_uniform = !Context::current()->parsing() && m_kind == SH_TEMP;
  }

  if(m_uniform || m_kind == SH_CONST) addVariant(); 
  if(updateVarList) programVarListInit();
  programDeclInit();
}

VariableNode::~VariableNode()
{
  detach_dependencies();
  /* TODO using a smart pointer now - probably shouldn't be though, so leave
   * these here just in case
  delete m_variant; 
  m_variant = 0;
  */
  if(m_eval) delete m_eval;
}

VariableNodePtr VariableNode::clone(BindingType newKind, 
      int newSize, 
      ValueType newValueType, 
      SemanticType newType, 
      bool updateVarList, 
      bool keepUniform) const
{
  SH_DEBUG_ASSERT(newValueType != 0);
  BindingType kind = (newKind == BINDINGTYPE_END ? m_kind : newKind);
  int size = (newSize == 0 ? m_size : newSize);
  ValueType valueType = (newValueType == VALUETYPE_END ? m_valueType : newValueType);
  SemanticType type = (newType == SEMANTICTYPE_END ? m_specialType : newType);

  return new VariableNode(*this, kind, size, valueType, type, 
      updateVarList, keepUniform);
}

bool VariableNode::uniform() const
{
  return m_uniform;
}

bool VariableNode::hasValues() const
{
  return (m_variant);
}

void VariableNode::lock()
{
  m_locked++;
}

void VariableNode::unlock()
{
  m_locked--;
  update_all();
}

ValueType VariableNode::valueType() const {
  return m_valueType;
}

int VariableNode::size() const
{
  return m_size;
}

void VariableNode::size(int s)
{
  SH_DEBUG_ASSERT(!m_variant);
  m_size = s;
}

void VariableNode::name(const std::string& n)
{
  this->Meta::name(n);
}

std::string VariableNode::name() const
{

  if (has_name()) return this->Meta::name();
  
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

bool VariableNode::hasRange()
{
  return has_meta("lowBound") && has_meta("highBound");
}

void VariableNode::rangeVariant(const Variant* low, const Variant* high) 
{
  meta("lowBound", low->encode(0, m_size));
  meta("highBound", high->encode(0, m_size));
}

void VariableNode::rangeVariant(const Variant* low, const Variant* high, 
    bool neg, const Swizzle &writemask)
{
  const VariantFactory* factory = variantFactory(m_valueType);

  std::string oldLo, oldHi;
  oldLo = meta("lowBound");
  oldHi = meta("highBound");

  Variant *newLo, *newHi; 
  if(oldLo.empty() || oldHi.empty()) { // TODO they should be either both empty or both not
    newLo = makeLow(); 
    newHi = makeHigh(); 
  } else {
    newLo = factory->generate(oldLo);
    newHi = factory->generate(oldHi);
  }

  // @todo type slow temporary solution
  for(int i = 0; i < writemask.size(); ++i) {
    newLo->set(low, neg, Swizzle(m_size, writemask[i]));
    newHi->set(high, neg, Swizzle(m_size, writemask[i]));
  }

  meta("lowBound", newLo->encode());
  meta("highBound", newHi->encode());

  delete newLo;
  delete newHi;
}

VariantPtr VariableNode::lowBoundVariant() const
{
  const VariantFactory* factory = variantFactory(m_valueType);
  std::string metaLow = meta("lowBound");

  return (metaLow.empty() ? makeLow() : factory->generate(metaLow));
}

VariantPtr VariableNode::highBoundVariant() const
{
  const VariantFactory* factory = variantFactory(m_valueType);
  std::string metaHigh = meta("highBound");

  return (metaHigh.empty() ? makeHigh() : factory->generate(metaHigh));
}

BindingType VariableNode::kind() const
{
  return m_kind;
}

SemanticType VariableNode::specialType() const
{
  return m_specialType;
}

std::string VariableNode::nameOfType() const {
  std::ostringstream os;
  // TODO indicate ValueType properly
  os << "Sh" << bindingTypeName[ m_kind ] << semanticTypeName[ m_specialType ] 
    << m_size << typeInfo(m_valueType)->name();
  return os.str();
}

void VariableNode::specialType(SemanticType type)
{
  m_specialType = type;
}

// TODO: also have an n-length set value, since updating the uniforms
// will otherwise be horribly inefficient.
void VariableNode::setVariant(const Variant* other)
{
  assert(m_variant);
  m_variant->set(other);
  update_all();
}

void VariableNode::setVariant(const VariantCPtr& other)
{
  setVariant(other.object());
}

void VariableNode::setVariant(const Variant* other, int index) 
{
  assert(m_variant);
  m_variant->set(other, index); 
  update_all();
}

void VariableNode::setVariant(const VariantCPtr& other, int index) 
{
  setVariant(other.object(), index);
}

void VariableNode::setVariant(const Variant* other, bool neg, const Swizzle &writemask)
{
  assert(m_variant);
  m_variant->set(other, neg, writemask);
  update_all();
}

void VariableNode::setVariant(const VariantCPtr& other, bool neg, const Swizzle &writemask)
{
  setVariant(other.object(), neg, writemask);
}

const Variant* VariableNode::getVariant() const
{
  return m_variant.object();
}


Variant* VariableNode::getVariant() 
{
  return m_variant.object();
}

Variant* VariableNode::makeLow() const
{
  const VariantFactory* factory = variantFactory(m_valueType);
  Variant* result;

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

Variant* VariableNode::makeHigh() const
{
  const VariantFactory* factory = variantFactory(m_valueType);
  return factory->generateOne(m_size); 
}


void VariableNode::update_all() 
{
  if (m_uniform && !m_locked) {
    const BoundIterator begin = beginBound();
    const BoundIterator end   = endBound();
    for (BoundIterator i = begin; i != end; ++i) {
      // TODO: Maybe pass in the backend unit to updateUniform
      if (i->second.node()) i->second.updateUniform(this);
    }

    update_dependents();
  }
}

void VariableNode::programVarListInit() 
{
  ProgramNodePtr prog = Context::current()->parsing();
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

void VariableNode::programDeclInit() 
{
  if (m_kind != SH_TEMP || m_uniform) return;
  ProgramNodePtr prog = Context::current()->parsing();
  if (prog) {
    if (prog->ctrlGraph) { // already has ctrlGraph, add decl to entry
      prog->addDecl(this);
    } else {
      prog->tokenizer.blockList()->addStatement(Statement(Variable(this), 
                                                            OP_DECL, 
                                                            Variable(this)));
    }
  }
}

void VariableNode::addVariant()
{
  if (m_variant) return;
  SH_DEBUG_ASSERT(m_valueType != VALUETYPE_END);
  m_variant = variantFactory(m_valueType)->generate(m_size);
}

void VariableNode::attach(const ProgramNodePtr& evaluator)
{
  SH_DEBUG_ASSERT(uniform());
  if (!m_eval) m_eval = new VariableNodeEval;
  // TODO: Check that the program really evaluates this variable.

  detach_dependencies();

  m_eval->value = evaluator;

  if (m_eval->value) {
    for (ProgramNode::VarList::const_iterator I = m_eval->value->begin_parameters();
         I != m_eval->value->end_parameters(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->add_dependent(this);
    }
    
    update();
  }
}

void VariableNode::update()
{
  if (!m_eval) m_eval = new VariableNodeEval;
  if (!m_eval->value) return;

  evaluate(m_eval->value);
}

const Pointer<ProgramNode>& VariableNode::evaluator() const
{
  if (!m_eval) m_eval = new VariableNodeEval;
  return m_eval->value;
}

#ifdef USE_MEMORY_POOL
void* VariableNode::operator new(std::size_t size)
{
  if (size != sizeof(VariableNode)) return ::operator new(size);
  if (!m_pool) m_pool = new Pool(sizeof(VariableNode), 32768);
  return m_pool->alloc();
}

void VariableNode::operator delete(void* ptr, std::size_t size)
{
  if (size != sizeof(VariableNode)) ::operator delete(ptr);
  else {
    // We won't have a pool if the object is an instance of a class
    // derived from VariableNode (ChannelNode for example)
    if (m_pool) m_pool->free(ptr);
  }
}
#endif

void VariableNode::add_dependent(VariableNode* dep)
{
  if (std::find(m_dependents.begin(), m_dependents.end(), dep) != m_dependents.end()) return;
  m_dependents.push_back(dep);
}

void VariableNode::remove_dependent(VariableNode* dep)
{
  m_dependents.remove(dep);
}

void VariableNode::update_dependents()
{
  for (std::list<VariableNode*>::iterator I = m_dependents.begin();
       I != m_dependents.end(); ++I) {
    (*I)->update();
  }
}

void VariableNode::detach_dependencies()
{
  if (!m_eval) return;
  if (m_eval->value) {
    for (ProgramNode::VarList::const_iterator I = m_eval->value->begin_parameters();
         I != m_eval->value->end_parameters(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->remove_dependent(this);
    }
    m_eval->value = 0;
  }
}

int VariableNode::m_maxID = 0;
#ifdef USE_MEMORY_POOL
Pool* VariableNode::m_pool = 0;
#endif
}
