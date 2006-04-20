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
#include "Stream.hpp"
#include "Syntax.hpp"
#include "Algebra.hpp"
#include "Context.hpp"
#include "BaseTexture.hpp"

namespace SH {

Stream::Stream()
{
}

Stream::Stream(const BaseTexture& array)
{
  append(array);
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

Stream::size_type Stream::size() const
{
  return m_nodes.size();
}

void Stream::append(const BaseTexture& array)
{
  m_nodes.push_back(array);
}

void Stream::prepend(const BaseTexture& array)
{
  m_nodes.push_front(array);
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

Stream combine(const BaseTexture& left, const BaseTexture& right)
{
  Stream stream(left);
  stream.append(right);
  return stream;
}

Stream combine(const Stream& left, const BaseTexture& right)
{
  Stream stream = left;
  stream.append(right);
  return stream;
}

Stream combine(const BaseTexture& left, const Stream& right)
{
  Stream stream = right;
  stream.prepend(left);
  return stream;
}

Stream operator&(const Stream& left, const Stream& right)
{
  return combine(left, right);
}

Stream operator&(const BaseTexture& left, const BaseTexture& right)
{
  return combine(left, right);
}

Stream operator&(const Stream& left, const BaseTexture& right)
{
  return combine(left, right);
}

Stream operator&(const BaseTexture& left, const Stream& right)
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

Program connect(const BaseTexture& array,
                  const Program& program)
{
  Program result = program;
  result.append_input(array);
  return result;
}

Program operator<<(const Program& program,
                     const BaseTexture& array)
{
  return connect(array, program);
}

Stream& Stream::operator=(const Program& program)
{
  SH_DEBUG_ASSERT(program.node());
  BackendPtr backend = Backend::get_backend(program.target());
  SH_DEBUG_ASSERT(backend);
  backend->execute(program, *this);
  return *this;
}

}
