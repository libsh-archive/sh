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
#include "ShDebug.hpp"
#include "ShInstructions.hpp"
#include "ShVariableNode.hpp"
#include "ShAttrib.hpp"
#include "ShAaVariable.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_AAVARIABLE
#endif
// #define SH_DBG_AAVARIABLE
  
#ifdef SH_DBG_AAVARIABLE                                                               
#define SH_DEBUG_PRINT_AV(x) { SH_DEBUG_PRINT(x); }                                    
#else                                                                                   
#define SH_DEBUG_PRINT_AV(x) {}
#endif  

//@todo range
// There's a number of checks in place here for zero-length noise symbol
// assignments.  verify correctness and improve "elegantness".

namespace SH {

//@todo range refactor code in these three constructors
ShAaVariableNode::ShAaVariableNode(ShVariableNodeCPtr node, const ShAaSyms& syms,
    ShBindingType kind, const char* suffix)
  : m_packed(syms.isSingles()),
    m_syms(syms),
    m_center(node->clone(kind, syms.size(), shRegularValueType(node->valueType()), 
                       SH_SEMANTICTYPE_END, false, false)),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(node->name() + suffix)
{
  SH_DEBUG_PRINT_AV("  creating AaVariableNode for ShVariable " << m_name); 
  init();
}

ShAaVariableNode::ShAaVariableNode(const std::string& name, ShBindingType kind, 
      ShValueType valueType, ShSemanticType type,  const ShAaSyms& syms)
  : m_packed(syms.isSingles()), 
    m_syms(syms),
    m_center(ShVariable(new ShVariableNode(kind, syms.size(), 
            shRegularValueType(valueType), type))),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(name)
{
  SH_DEBUG_PRINT_AV("  creating AaVariableNode from scratch named " << m_name); 
  init();
}


ShAaVariableNode::ShAaVariableNode(const ShAaVariableNode& other, const ShAaSyms &syms) 
  : m_packed(syms.isSingles()), 
    m_syms(syms),
    m_center(other.m_center.node()->clone(SH_TEMP, syms.size(), 
          SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false)),
    m_symvar(syms.isSingles() ? 1 : syms.size()),
    m_name(other.m_name + "_saClone")
{
  SH_DEBUG_PRINT_AV("  cloning AaVariableNode from " << other.name() << " syms=" << syms); 
  init();
}

void ShAaVariableNode::varlistInsert(ShProgramNode::VarList& varlist, ShProgramNode::VarList::iterator pos)
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

ShVariable ShAaVariableNode::makeTemp(int size, const std::string& suffix) {
  ShVariable result(m_center.node()->clone(SH_TEMP, size,
        SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false));
  result.name(m_name + suffix);
  return result;
}

ShRecord ShAaVariableNode::record() {
  ShRecord result;
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

void ShAaVariableNode::init()
{
  m_center.name(m_name + "_center");

  if(m_packed) {
    m_symvar[0] = ShVariable(m_center.node()->clone(SH_BINDINGTYPE_END, 
          0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false));
    std::ostringstream out;
    out << m_name << "_pk_" << m_syms;
    m_symvar[0].name(out.str());
  } else {
    for(int i = 0; i < size(); ++i) {
      if(m_syms[i].empty()) continue;

      m_symvar[i] = ShVariable(m_center.node()->clone(SH_BINDINGTYPE_END, 
            m_syms[i].size(), SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false));
      std::ostringstream out;
      out << m_name << "[" << i << "]" << m_syms[i];
      m_symvar[i].name(out.str());
    }
  }
}


ShAaVariable::ShAaVariable()
  : m_node(0)
{}

ShAaVariable::ShAaVariable(ShAaVariableNodePtr node)
  : m_node(node),
    m_swizzle(ShSwizzle(node->size())),
    m_neg(false),
    m_used(node->syms())
{}
 
ShAaVariable::ShAaVariable(ShAaVariableNodePtr node, const ShSwizzle& swizzle, bool neg, const ShAaSyms& use)
  : m_node(node), m_swizzle(swizzle), m_neg(neg), m_used(use) 
{}

ShAaVariable ShAaVariable::clone() const
{
  ShAaVariableNodePtr cloneNode(new ShAaVariableNode(*m_node, m_used));
  ShAaVariable result(cloneNode); 
  return result.ASN(*this);
}

ShAaVariable ShAaVariable::operator()(int i0) const
{
  return operator()(ShSwizzle(size(), i0)); 
}

ShAaVariable ShAaVariable::operator()(int i0, int i1) const
{
  return operator()(ShSwizzle(size(), i0, i1)); 
}

ShAaVariable ShAaVariable::operator()(int i0, int i1, int i2) const
{
  return operator()(ShSwizzle(size(), i0, i1, i2)); 
}

ShAaVariable ShAaVariable::operator()(int i0, int i1, int i2, int i3) const
{
  return operator()(ShSwizzle(size(), i0, i1, i2, i3)); 
}

ShAaVariable ShAaVariable::operator()(int swizSize, int indices[]) const
{
  return operator()(ShSwizzle(size(), swizSize, indices)); 
}

ShAaVariable ShAaVariable::operator()(const ShSwizzle& swizzle) const
{
  // Construct a new set of syms
  return ShAaVariable(m_node, m_swizzle * swizzle, m_neg, m_used.swizSyms(swizzle));
}

ShVariable ShAaVariable::center() const {
  ShVariable result = m_node->center()(m_swizzle);
  if(m_neg) return -result;
  return result;
}

ShVariable ShAaVariable::err(int i) const {
  ShVariable result = (*m_node)[m_swizzle[i]](getSymSwiz(i)); 
  if(m_neg) return -result;
  return result;
}

ShVariable ShAaVariable::err(int i, const ShAaIndexSet& use) const {
  ShVariable result = (*m_node)[m_swizzle[i]](getSymSwiz(i, use));
  if(m_neg) return -result;
  return result;
}

ShVariable ShAaVariable::err(int i, int idx) const {
  ShVariable result;

  if(!m_used[i].contains(idx)) {
    result = ShConstAttrib1f(0.0f);
  } else {
    result = (*m_node)[m_swizzle[i]](getSymSwiz(i, idx));
    if(m_neg) return -result;
  }
  return result;
}

const ShAaSyms& ShAaVariable::use() const {
  return m_used;
}

const ShAaIndexSet& ShAaVariable::use(int i) const {
  return m_used[i];
}

ShAaVariable ShAaVariable::NEG() const {
  return ShAaVariable(m_node, m_swizzle, !m_neg, m_used);
}

// @todo range Also make sure that if one of the normal ShVariable args
// is say the center of this that things don't screw up
ShAaVariable& ShAaVariable::ASN(const ShAaVariable &other) {
  return ASN(other, other.use());
}

ShAaVariable& ShAaVariable::ASN(const ShAaVariable &other, const ShAaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());

  if(changeCenter) {
    shASN(center(), other.center());
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    shASN(err(i, used[i]), other.err(i, used[i]));
  }
  return *this;
}

ShAaVariable& ShAaVariable::ASN(const ShVariable& other, const ShAaSyms& used) {
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    shASN(err(i, used[i]), other(i));
  }
  return *this;
}

ShAaVariable& ShAaVariable::ZERO() {
  // @todo range make this ASN
  ShConstAttrib1f ZERO(0.0f);
  shASN(center(), ShVariable(ZERO.node(), ShSwizzle(ZERO.swizzle(), size()), false));
  for(int i = 0; i < size(); ++i) {
    if((*m_node)(i).empty()) continue;
    ShVariable erri = err(i, (*m_node)(i)); // get tuple for all error symbols
    shASN(erri, ShVariable(ZERO.node(), ShSwizzle(ZERO.swizzle(), erri.size()), false));
  }
  return *this;
}


ShAaVariable& ShAaVariable::ADD(const ShAaVariable& other) {
  return ADD(other, other.use());
}

ShAaVariable& ShAaVariable::ADD(const ShAaVariable& other, const ShAaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());

  if(changeCenter) {
    shADD(center(), center(), other.center());
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    ShVariable myerr = err(i, used[i]); 
    shADD(myerr, myerr, other.err(i, used[i]));
  }
  return *this;
}

ShAaVariable& ShAaVariable::ADD(const ShVariable& other) {
  SH_DEBUG_ASSERT(size() == other.size());

  shADD(center(), center(), other);
  return *this;
}

ShAaVariable& ShAaVariable::MUL(const ShVariable& other) {
  SH_DEBUG_ASSERT(size() == other.size());
  shMUL(center(), center(), other);
  for(int i = 0; i < size(); ++i) {
    if(use(i).empty()) continue;
    ShVariable myerr = err(i);
    shMUL(myerr, myerr, other(i));
  }
  return *this;
}

ShAaVariable& ShAaVariable::MAD(const ShAaVariable& other, const ShVariable& scalar) 
{
  return MAD(other, scalar, other.use());
}

ShAaVariable& ShAaVariable::MAD(const ShAaVariable& other, const ShVariable& scalar, const ShAaSyms& used, bool changeCenter) {
  SH_DEBUG_ASSERT(other.node() != node());
  SH_DEBUG_ASSERT(size() == other.size());
  // @todo range - this doesn't work if this == other 
  
  if(changeCenter) {
    shMAD(center(), other.center(), scalar, center());
  }
  for(int i = 0; i < size(); ++i) {
    if(used[i].empty()) continue;
    ShVariable myerr = err(i, used[i]);
    shMAD(myerr, other.err(i, used[i]), scalar(i), myerr);
  }
  return *this;
}

ShAaVariable& ShAaVariable::setErr(const ShVariable& other, const ShAaSyms &used) {
  SH_DEBUG_ASSERT(size() == used.size());
  for(int i = 0; i < size(); ++i) {
    shASN(err(i, used[i].last()), other(i)); 
  }
  return *this;
}

ShAaVariable& ShAaVariable::addErr(const ShVariable& other, const ShAaSyms &used) {
  SH_DEBUG_ASSERT(size() == used.size());
  for(int i = 0; i < size(); ++i) {
    ShVariable erri = err(i, used[i].last());
    shABS(erri, erri);
    shADD(erri, erri, other(i)); 
  }
  return *this;
}

// Returns radius in a variable
ShVariable ShAaVariable::radius() const {
  ShVariable result = makeTemp("_radius");
  for(int i = 0; i < size(); ++i) {
    if(use(i).empty()) {
      shASN(result(i), ShConstAttrib1f(0.0));
    } else {
      ShVariable erri = err(i);
      ShVariable abs_err(erri.node()->clone(SH_TEMP, erri.size()));
      abs_err.name(erri.name() + "_abs");
      shABS(abs_err, erri);
      shCSUM(result(i), abs_err); 
    }
  }
  return result;
}

ShVariable ShAaVariable::width() const {
  ShVariable result = radius();
  result.name(m_node->name() + "_width");
  shADD(result, result, result); // double radius
  return result; 
}

ShVariable ShAaVariable::lo() const {
  ShVariable result = makeTemp("_lo");
  shADD(result, center(), -radius());
  return result;
}

ShVariable ShAaVariable::hi() const {
  ShVariable result = makeTemp("_hi");
  shADD(result, center(), radius());
  return result;
}

ShVariable ShAaVariable::temp(std::string name) const {
  ShVariable result = makeTemp("");
  result.name(name);
  return result;
}

ShRecord ShAaVariable::record() {
  ShRecord result;
  result.append(center()); 
  for(int i = 0; i < size(); ++i) {
    if(m_used[i].empty()) continue;
    result.append(err(i));
  }
  return result;
}

std::ostream& operator<<(std::ostream& out, const ShAaVariable &var)
{
  int i, j;
  out << "{" << var.center() << " + (";
  for(i = 0; i < var.size(); ++i) {
    if(i > 0) out << ", ";
    const ShAaIndexSet usei = var.use(i); 
    if(usei.empty()) {
      out << "[null]";
      continue;
    }
    ShVariable erri = var.err(i);
    ShAaIndexSet::const_iterator U = usei.begin();
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
ShSwizzle ShAaVariable::getSymSwiz(int i, const ShAaIndexSet& used) const {
  int num = used.size();
  int *s = new int[num];

  int j, uj;
  ShAaIndexSet::const_iterator J, U;
  const ShAaIndexSet& myset = (*m_node)(m_swizzle[i]);
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
  ShSwizzle result(myset.size(), num, s);
  delete[] s;
  return result;
}

ShSwizzle ShAaVariable::getSymSwiz(int i, int idx) const {
  int j;
  ShAaIndexSet::const_iterator J;
  const ShAaIndexSet& myset = (*m_node)(m_swizzle[i]);
  J = myset.begin(); 
  for(j = 0; *J != idx && J != myset.end(); ++J, ++j);
  if(J == myset.end()) {
    SH_DEBUG_PRINT("On AaVariable " << name()); 
    SH_DEBUG_PRINT("  could not match used sym set to syms available on this at element " << m_swizzle[i]);
    SH_DEBUG_PRINT("  m_used = " << m_used);
    SH_DEBUG_PRINT("  myset = " << myset);
    SH_DEBUG_ASSERT(0);
  }
  ShSwizzle result(myset.size(), j);
  return result;
}

ShSwizzle ShAaVariable::getSymSwiz(int i) const {
  return getSymSwiz(i, m_used[i]);
}

ShVariable ShAaVariable::makeTemp(const std::string& suffix) const {
  return m_node->makeTemp(size(), suffix);
}

}
