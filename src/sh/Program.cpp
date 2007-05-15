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
    binding_spec(other.binding_spec),
    stream_inputs(other.stream_inputs),
    m_node(other.m_node)
{
  for (Record::const_iterator I = other.uniform_inputs.begin();
       I != other.uniform_inputs.end(); ++I) {
    uniform_inputs.append(*I);
  }
}

Program::Program(const ProgramNodePtr& node)
  : MetaForwarder(node.object()),
    m_node(node)
{
}

Program Program::clone(bool cloneVariables) const
{
  Program result(m_node ? m_node->clone(cloneVariables) : 0);
  result.clone_bindings_from(*this);
  return result; 
}

void Program::clone_bindings_from(const Program& other) 
{
  binding_spec = other.binding_spec;
  stream_inputs = other.stream_inputs;
  uniform_inputs.clear();
  for (Record::const_iterator I = other.uniform_inputs.begin();
       I != other.uniform_inputs.end(); ++I) {
    uniform_inputs.append(*I);
  }
}

void Program::append_bindings_from(const Program& other) 
{
  std::copy(other.binding_spec.begin(), other.binding_spec.end(), std::back_inserter(binding_spec));
  stream_inputs = stream_inputs & other.stream_inputs;
  uniform_inputs.append(other.uniform_inputs);
}

Program& Program::operator=(const Program& other)
{
  m_node = other.m_node;
  clone_bindings_from(other);
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

std::string Program::describe_binding_spec() const {
  Record::const_iterator R = uniform_inputs.begin();
  Stream::const_iterator S = stream_inputs.begin();
  BindingSpec::const_iterator B = binding_spec.begin();
  std::ostringstream sout;

  for(;B != binding_spec.end(); ++B) {
    switch(*B) {
      case UNIFORM: sout << "UNIFORM: " << (R->node() ? R->name().c_str() : "unknown") 
                        << " = " << *R << std::endl; ++R; break;
      case STREAM: sout << "STREAM: " << S->node() << std::endl; ++S; break;
    }
  }
  return sout.str();

}

}
