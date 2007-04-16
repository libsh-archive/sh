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
#include <iostream>
#include "Debug.hpp"
#include "Instructions.hpp"
#include "VariableNode.hpp"
#include "Attrib.hpp"
#include "AaVariable.hpp"

#ifdef DBG_AA
#define DBG_AAVARIABLE
#endif
// #define DBG_AAVARIABLE
  
#ifdef DBG_AAVARIABLE                                                               
#define SH_DEBUG_PRINT_AV(x) { SH_DEBUG_PRINT(x); }                                    
#else                                                                                   
#define SH_DEBUG_PRINT_AV(x) {}
#endif  

//@todo range
// There's a number of checks in place here for zero-length noise symbol
// assignments.  verify correctness and improve "elegantness".


using namespace std;

namespace SH {

//@todo range refactor code in these three constructors
AaVariableNode::AaVariableNode(VariableNodeCPtr node, const AaSyms& syms,
    BindingType kind, const char* suffix)
  : m_packed(syms.isSingles()),
    m_syms(syms),
    m_center(node->clone(kind, syms.size(), regularValueType(node->valueType()), 
                       SEMANTICTYPE_END, false, false)),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(node->name() + suffix)
{
  SH_DEBUG_PRINT_AV("  creating AaVariableNode for Variable " << m_name); 
  init();
}

AaVariableNode::AaVariableNode(const string& name, BindingType kind, 
      ValueType valueType, SemanticType type,  const AaSyms& syms)
  : m_packed(syms.isSingles()), 
    m_syms(syms),
    m_center(Variable(new VariableNode(kind, syms.size(), 
            regularValueType(valueType), type))),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(name)
{
  SH_DEBUG_PRINT_AV("  creating AaVariableNode from scratch named " << m_name); 
  init();
}


AaVariableNode::AaVariableNode(const AaVariableNode& other, const AaSyms &syms) 
  : m_packed(syms.isSingles()), 
    m_syms(syms),
    m_center(other.m_center.node()->clone(SH_TEMP, syms.size(), 
          VALUETYPE_END, SEMANTICTYPE_END, true, false)),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(other.m_name + "_saClone")
{
  SH_DEBUG_PRINT_AV("  cloning AaVariableNode from " << other.name() << " syms=" << syms); 
  init();
}

void AaVariableNode::varlistInsert(ProgramNode::VarList& varlist, ProgramNode::VarList::iterator pos)
{
  SH_DEBUG_ASSERT(m_center.node()->kind() == SH_INPUT || 
      m_center.node()->kind() == SH_OUTPUT);
  varlist.insert(pos, m_center.node());
  for(VarVec::iterator V = m_symvar.begin(); V != m_symvar.end(); ++V) {
    if(V->size() > 0) {
      varlist.insert(pos, V->node());
    }
  }
}

Variable AaVariableNode::radius() {
  Variable result = makeTemp(size(), "_radius");
  ConstAttrib1f ZERO(0.f);
  shASN(result, ZERO.repeat(size())); 
  if(m_packed) {
    shABS(result, m_symvar[0]);
    return result;
  } else {
    for(int i = 0; i < size(); ++i) { 
      if(m_symvar[i].size() == 0) {
        continue;
      } else {
        Variable resulti = result(i);
        ostringstream eout;
        eout << "_radius_err" << i;
        Variable symAbs = makeTemp(m_symvar[i].size(), eout.str()); 
        SH_DEBUG_PRINT(symAbs.size() << " " << m_symvar[i].size());
        shABS(symAbs, m_symvar[i]);
        shCSUM(resulti, symAbs); 
      }
    }
  }
}

Variable AaVariableNode::makeTemp(int size, const string& suffix) {
  Variable result(m_center.node()->clone(SH_TEMP, size,
        VALUETYPE_END, SEMANTICTYPE_END, true, false));
  result.name(m_name + suffix);
  return result;
}

Record AaVariableNode::record() {
  Record result;
  result.append(m_center);
  if(m_packed) {
    result.append(m_symvar[0]);
  } else {
    for(int i = 0; i < size(); ++i) {
      if(m_syms[i].empty()) continue;
      result.append(m_symvar[i]);
    }
  }
  return result;
}

void AaVariableNode::init()
{
  m_center.name(m_name + "_center");

  if(m_packed) {
    m_symvar[0] = Variable(m_center.node()->clone(BINDINGTYPE_END, 
          0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
    ostringstream out;
    out << m_name << "_pk_" << m_syms;
    m_symvar[0].name(out.str());
  } else {
    for(int i = 0; i < size(); ++i) {
      if(m_syms[i].empty()) continue;

      m_symvar[i] = Variable(m_center.node()->clone(BINDINGTYPE_END, 
            m_syms[i].size(), VALUETYPE_END, SEMANTICTYPE_END, true, false));
      ostringstream out;
      out << m_name << "[" << i << "]" << m_syms[i];
      m_symvar[i].name(out.str());
    }
  }
}


AaVariable::AaVariable()
  : m_node(0)
{}

AaVariable::AaVariable(AaVariableNodePtr node)
  : m_node(node),
    m_swizzle(Swizzle(node->size())),
    m_neg(false),
    m_used(node->syms())
{}
 
AaVariable::AaVariable(AaVariableNodePtr node, const Swizzle& swizzle, bool neg, const AaSyms& use)
  : m_node(node), m_swizzle(swizzle), m_neg(neg), m_used(use) 
{}

AaVariable AaVariable::clone() const
{
  AaVariableNodePtr cloneNode(new AaVariableNode(*m_node, m_used));
  AaVariable result(cloneNode); 
  return result.ASN(*this);
}

AaVariable AaVariable::operator()(int i0) const
{
  return operator()(Swizzle(size(), i0)); 
}

AaVariable AaVariable::operator()(int i0, int i1) const
{
  return operator()(Swizzle(size(), i0, i1)); 
}

AaVariable AaVariable::operator()(int i0, int i1, int i2) const
{
  return operator()(Swizzle(size(), i0, i1, i2)); 
}

AaVariable AaVariable::operator()(int i0, int i1, int i2, int i3) const
{
  return operator()(Swizzle(size(), i0, i1, i2, i3)); 
}

AaVariable AaVariable::operator()(int swizSize, int indices[]) const
{
  return operator()(Swizzle(size(), swizSize, indices)); 
}

AaVariable AaVariable::operator()(const Swizzle& swizzle) const
{
  // Construct a new set of syms
  return AaVariable(m_node, m_swizzle * swizzle, m_neg, m_used.swizSyms(swizzle));
}

AaVariable AaVariable::repeat(int n) const
{
  Swizzle swiz(size());
  return operator()(Swizzle(swiz, n));
}

Variable AaVariable::center() const {
  Variable result = m_node->center()(m_swizzle);
  if(m_neg) return -result;
  return result;
}

Variable AaVariable::err(int i) const {
  Variable result = (*m_node)[m_swizzle[i]](getSymSwiz(i)); 
  if(m_neg) return -result;
  return result;
}

Variable AaVariable::err(int i, const AaIndexSet& use) const {
  Variable result = (*m_node)[m_swizzle[i]](getSymSwiz(i, use));
  if(m_neg) return -result;
  return result;
}

Variable AaVariable::err(int i, int idx) const {
  Variable result;

  if(!m_used[i].contains(idx)) {
    result = ConstAttrib1f(0.0f);
  } else {
    result = (*m_node)[m_swizzle[i]](getSymSwiz(i, idx));
    if(m_neg) return -result;
  }
  return result;
}

const AaSyms& AaVariable::use() const {
  return m_used;
}

const AaIndexSet& AaVariable::use(int i) const {
  return m_used[i];
}

AaVariable AaVariable::NEG() const {
  return AaVariable(m_node, m_swizzle, !m_neg, m_used);
}

// @todo range Also make sure that if one of the normal Variable args
// is say the center of this that things don't screw up
AaVariable& AaVariable::ASN(const AaVariable &other) {
  return ASN(other, other.use());
}

AaVariable& AaVariable::ASN(const AaVariable &other, const AaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());

  if(changeCenter) {
    Variable c = center();
    shASN(c, other.center());
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    Variable erri = err(i, used[i]);
    shASN(erri, other.err(i, used[i]));
  }
  return *this;
}

AaVariable& AaVariable::ASN(const Variable& other, const AaSyms& used) {
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    Variable erri = err(i, used[i]);
    shASN(erri, other(i));
  }
  return *this;
}

AaVariable& AaVariable::ZERO() {
  // @todo range make this ASN
  ConstAttrib1f ZERO(0.0f);
  Variable c = center();
  shASN(c, Variable(ZERO.node(), Swizzle(ZERO.swizzle(), size()), false));
  for(int i = 0; i < size(); ++i) {
    if((*m_node)(i).empty()) continue;
    Variable erri = err(i, (*m_node)(i)); // get tuple for all error symbols
    shASN(erri, Variable(ZERO.node(), Swizzle(ZERO.swizzle(), erri.size()), false));
  }
  return *this;
}


AaVariable& AaVariable::ADD(const AaVariable& other) {
  return ADD(other, other.use());
}

AaVariable& AaVariable::ADD(const AaVariable& other, const AaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());

  if(changeCenter) {
    Variable c = center();
    shADD(c, c, other.center());
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    Variable myerr = err(i, used[i]); 
    shADD(myerr, myerr, other.err(i, used[i]));
  }
  return *this;
}

AaVariable& AaVariable::ADD(const Variable& other) {
  SH_DEBUG_ASSERT(size() == other.size());

  Variable c = center();
  shADD(c, c, other);
  return *this;
}

AaVariable& AaVariable::MUL(const Variable& other) {
  SH_DEBUG_ASSERT(size() == other.size());
  Variable c = center();
  shMUL(c, c, other);
  for(int i = 0; i < size(); ++i) {
    if(use(i).empty()) continue;
    Variable myerr = err(i);
    shMUL(myerr, myerr, other(i));
  }
  return *this;
}

AaVariable& AaVariable::MAD(const AaVariable& other, const Variable& scalar) 
{
  return MAD(other, scalar, other.use());
}

AaVariable& AaVariable::MAD(const AaVariable& other, const Variable& scalar, const AaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());
  // @todo range - this doesn't work if this == other 
  
  if(changeCenter) {
    Variable c = center();
    shMAD(c, other.center(), scalar, c);
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    Variable myerr = err(i, used[i]);
    shMAD(myerr, other.err(i, used[i]), scalar(i), myerr);
  }
  return *this;
}

AaVariable& AaVariable::COND(const Variable& condvar, const Variable& other)
{
  Variable c = center();
  shCOND(c, condvar, other, c);
  ConstAttrib1f ZERO(0.0f);
  for(int i = 0; i < size(); ++i) {
    if(use(i).empty()) continue;
    Variable myerr = err(i);
    shCOND(myerr, condvar, ZERO.repeat(myerr.size()), myerr);
  }
  return *this;
}

AaVariable& AaVariable::COND(const Variable& condvar, const AaVariable& other)
{
  return COND(condvar, other, other.use());
}

AaVariable& AaVariable::COND(const Variable& condvar, const AaVariable& other, 
    const AaSyms& used, bool changeCenter)
{
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size() && size() == condvar.size());

  if(changeCenter) {
    Variable c = center();
    shCOND(c, condvar, other.center(), c); 
  }

  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    Variable myerr = err(i, used[i]);
    shCOND(myerr, condvar(i), other.err(i, used[i]), myerr);
  }
  return *this;
}

AaVariable& AaVariable::setErr(const Variable& other, const AaSyms &used) {
  SH_DEBUG_ASSERT(size() == used.size());
  for(int i = 0; i < size(); ++i) {
    Variable erri = err(i, used[i].last());
    shASN(erri, other(i)); 
  }
  return *this;
}

AaVariable& AaVariable::addErr(const Variable& other, const AaSyms &used) {
  SH_DEBUG_ASSERT(size() == used.size());
  for(int i = 0; i < size(); ++i) {
    Variable erri = err(i, used[i].last());
    shABS(erri, erri);
    shADD(erri, erri, other(i)); 
  }
  return *this;
}

// Returns radius in a variable
Variable AaVariable::radius() const {
  Variable result = m_node->radius()(m_swizzle); 
  return result;
}

Variable AaVariable::width() const {
  Variable result = radius();
  result.name(m_node->name() + "_width");
  shADD(result, result, result); // double radius
  return result; 
}

Variable AaVariable::lo() const {
  Variable result = makeTemp("_lo");
  shADD(result, center(), -radius());
  return result;
}

void AaVariable::lohi(Variable& lo, Variable& hi) const {
  Variable rad = radius(); 
  lo = makeTemp("_lo");
  hi = makeTemp("_hi");
  shADD(lo, center(), -rad);
  shADD(hi, center(), rad);
}

Variable AaVariable::hi() const {
  Variable result = makeTemp("_hi");
  shADD(result, center(), radius());
  return result;
}

Variable AaVariable::temp(string name) const {
  Variable result = makeTemp("");
  result.name(name);
  return result;
}

Record AaVariable::record() {
  Record result;
  result.append(center()); 
  for(int i = 0; i < size(); ++i) {
    if(m_used[i].empty()) continue;
    result.append(err(i));
  }
  return result;
}

ostream& operator<<(ostream& out, const AaVariable &var)
{
  int i, j;
  out << "{" << var.center() << " + (";
  for(i = 0; i < var.size(); ++i) {
    if(i > 0) out << ", ";
    const AaIndexSet usei = var.use(i); 
    if(usei.empty()) {
      out << "[null]";
      continue;
    }
    Variable erri = var.err(i);
    AaIndexSet::const_iterator U = usei.begin();
    SH_DEBUG_ASSERT(erri.size() == usei.size());
    for(j = 0; j < erri.size(); ++j, ++U) {
      if(j != 0) out << " "; 
      out << erri(j) << "_" << *U; 
    }
  }
  out << ")}";
  return out;
}

// @todo range - need to check these
Swizzle AaVariable::getSymSwiz(int i, const AaIndexSet& used) const {
  int num = used.size();
  int *s = new int[num];

  int j, uj;
  AaIndexSet::const_iterator J, U;
  const AaIndexSet& myset = (*m_node)(m_swizzle[i]);
  J = myset.begin(); 
  U = used.begin();
  for(j = uj = 0; U != used.end(); ++U, ++uj) {
    for(; *J != *U && J != myset.end(); ++J, ++j);
    s[uj] = j;
  }
  if(J == myset.end()) {
    SH_DEBUG_PRINT("Could not match used sym set to syms available on this at element " << m_swizzle[i]);
    SH_DEBUG_PRINT("  used = " << used);
    SH_DEBUG_PRINT("  myset = " << myset);
    SH_DEBUG_ASSERT(0);
  }
  Swizzle result(myset.size(), num, s);
  delete[] s;
  return result;
}

Swizzle AaVariable::getSymSwiz(int i, int idx) const {
  int j;
  AaIndexSet::const_iterator J;
  const AaIndexSet& myset = (*m_node)(m_swizzle[i]);
  J = myset.begin(); 
  for(j = 0; *J != idx && J != myset.end(); ++J, ++j);
  if(J == myset.end()) {
    SH_DEBUG_PRINT("On AaVariable " << name()); 
    SH_DEBUG_PRINT("  could not match used sym set to syms available on this at element " << m_swizzle[i]);
    SH_DEBUG_PRINT("  m_used = " << m_used);
    SH_DEBUG_PRINT("  myset = " << myset);
    SH_DEBUG_ASSERT(0);
  }
  Swizzle result(myset.size(), j);
  return result;
}

Swizzle AaVariable::getSymSwiz(int i) const {
  return getSymSwiz(i, m_used[i]);
}

Variable AaVariable::makeTemp(const string& suffix) const {
  return m_node->makeTemp(size(), suffix);
}

}
