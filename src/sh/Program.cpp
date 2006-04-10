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
#include "ShProgram.hpp"
#include "ShBackend.hpp"
#include "ShStream.hpp"
#include "ShRecord.hpp"
#include "ShAlgebra.hpp"

namespace SH {

ShProgram::ShProgram()
  : ShMetaForwarder(0),
    m_node(0)
{
}

ShProgram::ShProgram(const std::string& target)
  : ShMetaForwarder(0),
    m_node(new ShProgramNode(target))
{
  real_meta(m_node.object());
}

ShProgram::ShProgram(const ShProgram& other)
  : ShMetaForwarder(other.m_node.object()),
    m_node(other.m_node)
{
}

ShProgram::ShProgram(const ShProgramNodePtr& node)
  : ShMetaForwarder(node.object()),
    m_node(node)
{
}

ShProgram& ShProgram::operator=(const ShProgram& other)
{
  m_node = other.m_node;
  real_meta(m_node.object());
  return *this;
}

// Call operators for channels and streams.
// Equivalent to operator<< invocations.
ShProgram ShProgram::operator()(const ShStream& s0) const
{
  return (*this) << s0;
}

ShProgram ShProgram::operator()(const ShStream& s0,
                                 const ShStream& s1) const
{
  return (*this) << s0 << s1;
}

ShProgram ShProgram::operator()(const ShStream& s0,
                                 const ShStream& s1,
                                 const ShStream& s2) const
{
  return (*this) << s0 << s1 << s2;
}

ShProgram ShProgram::operator()(const ShStream& s0,
                                 const ShStream& s1,
                                 const ShStream& s2,
                                 const ShStream& s3) const
{
  return (*this) << s0 << s1 << s2 << s3;
}

ShProgram ShProgram::operator()(const ShStream& s0,
                                 const ShStream& s1,
                                 const ShStream& s2,
                                 const ShStream& s3,
                                 const ShStream& s4) const
{
  return (*this) << s0 << s1 << s2 << s3 << s4;
}

ShProgram ShProgram::operator()(const ShRecord &rec) const 
{
  return (*this) << rec; 
}

ShProgram ShProgram::operator()(const ShVariable &v0) const 
{
  return (*this) << v0; 
}

ShProgram ShProgram::operator()(const ShVariable &v0, 
                                const ShVariable &v1) const
{
  return operator()(v0 & v1);

}

ShProgram ShProgram::operator()(const ShVariable &v0, 
                                const ShVariable &v1, 
                                const ShVariable &v2) const
{
  return operator()(v0 & v1 & v2);
}

ShProgram ShProgram::operator()(const ShVariable &v0, 
                                const ShVariable &v1, 
                                const ShVariable &v2, 
                                const ShVariable &v3) const
{
  return operator()(v0 & v1 & v2 & v3);
}

}
