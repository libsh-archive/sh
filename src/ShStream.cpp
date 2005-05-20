// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
  ShBackendPtr backend = ShBackend::get_backend(program.target());
  backend->execute(program.node(), *this);
  return *this;
}

}
