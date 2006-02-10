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
#include "ShStream.hpp"
#include "ShProgram.hpp"
#include "ShChannelNode.hpp"
#include "ShSyntax.hpp"
#include "ShAlgebra.hpp"
#include "ShContext.hpp"

namespace SH {

// May want to move this elsewhere
ShProgram connect(const ShChannelNodePtr& node, const ShProgram& program)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShVariable out(node->clone(SH_OUTPUT));

    ShVariable streamVar(node);
    ShStatement stmt(out, SH_OP_FETCH, streamVar);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } SH_END_PROGRAM;
  return connect(nibble, program);
}

ShStream::ShStream()
{
}
ShStream::ShStream(const ShChannelNodePtr& channel)
{
  append(channel);
}

ShStream::const_iterator ShStream::begin() const
{
  return m_nodes.begin();
}

ShStream::const_iterator ShStream::end() const
{
  return m_nodes.end();
}

ShStream::iterator ShStream::begin() 
{
  return m_nodes.begin();
}

ShStream::iterator ShStream::end() 
{
  return m_nodes.end();
}

int ShStream::size() const
{
  return m_nodes.size();
}

void ShStream::append(const ShChannelNodePtr& node)
{
  m_nodes.push_back(node);
}

void ShStream::prepend(const ShChannelNodePtr& node)
{
  m_nodes.push_front(node);
}

ShStream combine(const ShStream& left, const ShStream& right)
{
  ShStream stream = left;
  for (ShStream::NodeList::const_iterator I = right.begin(); I != right.end();
       ++I) {
    stream.append(*I);
  }
  return stream;
}

ShStream operator&(const ShStream& left, const ShStream& right)
{
  return combine(left, right);
}

// Connecting (currying) streams onto programs
ShProgram connect(const ShStream& stream, const ShProgram& program)
{
  ShProgram p;
  p = program;
  for (ShStream::NodeList::const_iterator I = stream.begin();
       I != stream.end(); ++I) {
    p = connect(*I, p);
  }
  return p;
}

ShProgram operator<<(const ShProgram& program, const ShStream& stream)
{
  return connect(stream, program);
}

ShStream& ShStream::operator=(const ShProgram& program)
{
  SH_DEBUG_ASSERT(program.node());
  ShBackendPtr backend = ShBackend::get_backend(program.target());
  SH_DEBUG_ASSERT(backend);
  backend->execute(program.node(), *this);
  return *this;
}

}
