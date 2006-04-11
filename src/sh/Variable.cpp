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
#include <iostream>

#include "Variable.hpp"
#include "Debug.hpp"
#include "Record.hpp"
#include "Program.hpp"

namespace SH {

Variable::Variable()
  : MetaForwarder(0),
    m_node(0), m_swizzle(0), m_neg(false)
{
}

Variable::Variable(const VariableNodePtr& node)
  : MetaForwarder(node.object()),
    m_node(node), m_swizzle(node ? node->size() : 0), m_neg(false)
{
}

Variable::Variable(const VariableNodePtr& node,
                       const Swizzle& swizzle,
                       bool neg)
  : MetaForwarder(node.object()),
    m_node(node), m_swizzle(swizzle), m_neg(neg)
{
}

Variable& Variable::operator=(const Program &prg) {
  // @todo range
  Record rec(*this); 
  rec = prg;
  return *this;
}

void Variable::attach(const Program& prgc)
{
  Program prg(prgc);
  if (prg.node()->finished()) {
    m_node->attach(prg.node());
  } else {
    prg.node()->assign(m_node);
  }
}

bool Variable::null() const
{
  return !m_node;
}

bool Variable::uniform() const
{
  return m_node->uniform();
}

bool Variable::hasValues() const
{
  return m_node->hasValues();
}

int Variable::size() const
{
  return m_swizzle.size();
}

ValueType Variable::valueType() const 
{
  if(!m_node) return VALUETYPE_END;
  return m_node->valueType();
}

void Variable::rangeVariant(const Variant* low, const Variant* high)
{
  m_node->rangeVariant(low, high, m_neg, m_swizzle);
}

VariantPtr Variable::lowBoundVariant() const
{
  return m_node->lowBoundVariant()->get(m_neg, m_swizzle);
}

VariantPtr Variable::highBoundVariant() const
{
  return m_node->highBoundVariant()->get(m_neg, m_swizzle);
}

const Swizzle& Variable::swizzle() const
{
  return m_swizzle;
}

const VariableNodePtr& Variable::node() const
{
  return m_node;
}

bool Variable::neg() const
{
  return m_neg;
}

bool& Variable::neg()
{
  return m_neg;
}

VariantPtr Variable::getVariant() const
{
  return m_node->getVariant()->get(m_neg, m_swizzle);
}

VariantPtr Variable::getVariant(int index) const
{
  return m_node->getVariant()->get(m_neg, m_swizzle * Swizzle(size(), index)); 
}


bool Variable::loadVariant(Variant *&result) const
{
  if((!m_neg) && m_swizzle.identity()) {
    result = const_cast<Variant*>(m_node->getVariant());
    return false;
  }
  // @todo type figure out a nicer, but just as efficient way than this hackery without
  // exposing unwanted functionality in Variant or VariableNode. 
  VariantPtr temp = getVariant();
  temp->acquireRef();
  result = temp.object();
  return true;
}

void Variable::updateVariant() 
{
  m_node->update_all();
}

void Variable::setVariant(const Variant* other, bool neg, const Swizzle &writemask)
{
  m_node->setVariant(other, neg ^ m_neg, m_swizzle * writemask);
}

void Variable::setVariant(const VariantCPtr& other, bool neg, const Swizzle &writemask)
{
  setVariant(other.object(), neg, writemask); 
}

void Variable::setVariant(const Variant* other, int index)
{
  m_node->setVariant(other, m_neg, m_swizzle * Swizzle(size(), index));
}

void Variable::setVariant(const VariantCPtr& other, int index)
{
  setVariant(other.object(), index); 
}

void Variable::setVariant(const Variant* other)
{
  m_node->setVariant(other, m_neg, m_swizzle);
}

void Variable::setVariant(const VariantCPtr& other)
{
  setVariant(other.object());
}

Variable Variable::operator()() const
{
  return Variable(m_node, m_swizzle, m_neg);
}

Variable Variable::operator()(int i1) const
{
  return Variable(m_node, m_swizzle * Swizzle(size(), i1), m_neg);
}

Variable Variable::operator()(int i1, int i2) const
{
  return Variable(m_node, m_swizzle * Swizzle(size(), i1, i2), m_neg);
}

Variable Variable::operator()(int i1, int i2, int i3) const
{
  return Variable(m_node, m_swizzle * Swizzle(size(), i1, i2, i3), m_neg);
}

Variable Variable::operator()(int i1, int i2, int i3, int i4) const
{
  return Variable(m_node, m_swizzle * Swizzle(size(), i1, i2, i3, i4), m_neg);
}

Variable Variable::operator()(int n, int indices[]) const
{
  return Variable(m_node, m_swizzle * Swizzle(size(), n, indices), m_neg);
}

Variable Variable::operator()(const Swizzle &swizzle) const
{
  return Variable(m_node, m_swizzle * swizzle, m_neg);
}

std::ostream& operator<<(std::ostream& out, const Variable& v)
{
  if (!v.m_node){
    out << "[null]";
    return out;
  }
  if (!v.hasValues()){
    out << "[not uniform]";
    return out;
  }

  if(v.size() > 1) {
    out << '[';
  }
  out << v.getVariant()->encodeArray();
  if(v.size() > 1) {
    out<<']';
  }
  return out;
}


Variable Variable::operator-() const
{
  return Variable(m_node, m_swizzle, !m_neg);
}

bool Variable::operator==(const Variable& other) const
{
  return m_node == other.m_node && m_swizzle == other.m_swizzle && m_neg == other.m_neg;
}

void Variable::clone(const Variable& other)
{
  m_node = other.m_node;
  m_swizzle = other.m_swizzle;
  m_neg = other.m_neg;
}


}
