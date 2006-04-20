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
#include "Program.hpp"
#include "Backend.hpp"
#include "Stream.hpp"
#include "Record.hpp"
#include "Algebra.hpp"
#include "BaseTexture.hpp"

namespace SH {

Program::Program()
  : MetaForwarder(0),
    m_node(0)
{
}

Program::Program(const std::string& target)
  : MetaForwarder(0),
    m_node(new ProgramNode(target))
{
  real_meta(m_node.object());
}

Program::Program(const Program& other)
  : MetaForwarder(other.m_node.object()),
    m_stream_inputs(other.m_stream_inputs),
    m_node(other.m_node)
{
}

Program::Program(const ProgramNodePtr& node)
  : MetaForwarder(node.object()),
    m_node(node)
{
}

Program& Program::operator=(const Program& other)
{
  m_node = other.m_node;
  m_stream_inputs = other.m_stream_inputs;
  real_meta(m_node.object());
  return *this;
}

// Call operators for channels and streams.
// Equivalent to operator<< invocations.
Program Program::operator()(const Stream& s0) const
{
  return (*this) << s0;
}

Program Program::operator()(const Stream& s0,
                                 const Stream& s1) const
{
  return (*this) << s0 << s1;
}

Program Program::operator()(const Stream& s0,
                                 const Stream& s1,
                                 const Stream& s2) const
{
  return (*this) << s0 << s1 << s2;
}

Program Program::operator()(const Stream& s0,
                                 const Stream& s1,
                                 const Stream& s2,
                                 const Stream& s3) const
{
  return (*this) << s0 << s1 << s2 << s3;
}

Program Program::operator()(const Stream& s0,
                                 const Stream& s1,
                                 const Stream& s2,
                                 const Stream& s3,
                                 const Stream& s4) const
{
  return (*this) << s0 << s1 << s2 << s3 << s4;
}


Program Program::operator()(const BaseTexture& s0) const
{
  return (*this) << s0;
}

Program Program::operator()(const BaseTexture& s0,
                                 const BaseTexture& s1) const
{
  return (*this) << s0 << s1;
}

Program Program::operator()(const BaseTexture& s0,
                                 const BaseTexture& s1,
                                 const BaseTexture& s2) const
{
  return (*this) << s0 << s1 << s2;
}

Program Program::operator()(const BaseTexture& s0,
                                 const BaseTexture& s1,
                                 const BaseTexture& s2,
                                 const BaseTexture& s3) const
{
  return (*this) << s0 << s1 << s2 << s3;
}

Program Program::operator()(const BaseTexture& s0,
                                 const BaseTexture& s1,
                                 const BaseTexture& s2,
                                 const BaseTexture& s3,
                                 const BaseTexture& s4) const
{
  return (*this) << s0 << s1 << s2 << s3 << s4;
}


Program Program::operator()(const Record &rec) const 
{
  return (*this) << rec; 
}

Program Program::operator()(const Variable &v0) const 
{
  return (*this) << v0; 
}

Program Program::operator()(const Variable &v0, 
                                const Variable &v1) const
{
  return operator()(v0 & v1);

}

Program Program::operator()(const Variable &v0, 
                                const Variable &v1, 
                                const Variable &v2) const
{
  return operator()(v0 & v1 & v2);
}

Program Program::operator()(const Variable &v0, 
                                const Variable &v1, 
                                const Variable &v2, 
                                const Variable &v3) const
{
  return operator()(v0 & v1 & v2 & v3);
}

}
