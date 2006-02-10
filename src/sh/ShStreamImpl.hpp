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
#ifndef SHSTREAMLISTIMPL_HPP
#define SHSTREAMLISTIMPL_HPP

#include "ShStream.hpp"
#include "ShChannel.hpp"

namespace SH {

template<typename T>
ShStream::ShStream(const ShChannel<T>& channel)
{
  m_nodes.push_back(channel.node());
}

template<typename T>
void ShStream::append(const ShChannel<T>& channel)
{
  m_nodes.push_back(channel.node());
}

template<typename T>
void ShStream::prepend(const ShChannel<T>& channel)
{
  m_nodes.push_front(channel.node());
}

// Ways to form a combined stream
template<typename T1, typename T2>
ShStream combine(const ShChannel<T1>& left, const ShChannel<T2>& right)
{
  ShStream stream(left);
  stream.append(right);
  return stream;
}

template<typename T2>
ShStream combine(const ShStream& left, const ShChannel<T2>& right)
{
  ShStream stream = left;
  stream.append(right);
  return stream;
}

template<typename T1>
ShStream combine(const ShChannel<T1>& left, const ShStream& right)
{
  ShStream stream = right;
  stream.prepend(left);
  return stream;
}

// Aliases for combine
template<typename T1, typename T2>
ShStream operator&(const ShChannel<T1>& left, const ShChannel<T2>& right)
{
  return combine(left, right);
}

template<typename T2>
ShStream operator&(const ShStream& left, const ShChannel<T2>& right)
{
  return combine(left, right);
}

template<typename T1>
ShStream operator&(const ShChannel<T1>& left, const ShStream& right)
{
  return combine(left, right);
}

}
#endif
