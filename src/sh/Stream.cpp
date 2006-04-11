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
#include "Stream.hpp"
#include "Program.hpp"
#include "ChannelNode.hpp"
#include "Syntax.hpp"
#include "Algebra.hpp"
#include "Context.hpp"

namespace SH {

// May want to move this elsewhere
Program connect(const ChannelNodePtr& node, const Program& program)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    Variable out(node->clone(OUTPUT));

    Variable streamVar(node);
    Statement stmt(out, OP_FETCH, streamVar);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } SH_END_PROGRAM;
  return connect(nibble, program);
}

Stream::Stream()
{
}
Stream::Stream(const ChannelNodePtr& channel)
{
  append(channel);
}

Stream::const_iterator Stream::begin() const
{
  return m_nodes.begin();
}

Stream::const_iterator Stream::end() const
{
  return m_nodes.end();
}

Stream::iterator Stream::begin() 
{
  return m_nodes.begin();
}

Stream::iterator Stream::end() 
{
  return m_nodes.end();
}

int Stream::size() const
{
  return m_nodes.size();
}

void Stream::append(const ChannelNodePtr& node)
{
  m_nodes.push_back(node);
}

void Stream::prepend(const ChannelNodePtr& node)
{
  m_nodes.push_front(node);
}

Stream combine(const Stream& left, const Stream& right)
{
  Stream stream = left;
  for (Stream::NodeList::const_iterator I = right.begin(); I != right.end();
       ++I) {
    stream.append(*I);
  }
  return stream;
}

Stream operator&(const Stream& left, const Stream& right)
{
  return combine(left, right);
}

// Connecting (currying) streams onto programs
Program connect(const Stream& stream, const Program& program)
{
  Program p;
  p = program;
  for (Stream::NodeList::const_iterator I = stream.begin();
       I != stream.end(); ++I) {
    p = connect(*I, p);
  }
  return p;
}

Program operator<<(const Program& program, const Stream& stream)
{
  return connect(stream, program);
}

Stream& Stream::operator=(const Program& program)
{
  DEBUG_ASSERT(program.node());
  BackendPtr backend = Backend::get_backend(program.target());
  DEBUG_ASSERT(backend);
  backend->execute(program.node(), *this);
  return *this;
}

}
