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
