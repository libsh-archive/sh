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
#ifndef SHSTREAM_HPP
#define SHSTREAM_HPP

#include <list>
#include "DllExport.hpp"
#include "Channel.hpp"
#include "ChannelNode.hpp"

namespace SH {

/** Dynamic list of channels.
 * The stream keep track (by reference) of an ordered lists of 
 * data channels, to be used as inputs and outputs of stream operations.
 * @see Channel
 */
class
SH_DLLEXPORT Stream {
public:
  typedef std::list<ChannelNodePtr> NodeList;
  typedef NodeList::iterator iterator;
  typedef NodeList::const_iterator const_iterator;

  Stream();
  Stream(const ChannelNodePtr& node);
  
  template<typename T>
  Stream(const Channel<T>& channel);

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  int size() const;

  template<typename T>
  void append(const Channel<T>& channel);
  template<typename T>
  void prepend(const Channel<T>& channel);

  void append(const ChannelNodePtr& node);
  void prepend(const ChannelNodePtr& node);
  // Execute fully bound stream program and place results in stream.
  Stream& operator=(const Program& program);
  
private:
  NodeList m_nodes;
};

/** Combine two streams.
 * This concatenates the list of channels in the component streams.
 */
template<typename T1, typename T2>
Stream combine(const Channel<T1>& left, const Channel<T2>& right);

/** Combine a stream and a channel.
 * This concatenates the given channel to the end of the list of
 * channels in the stream.
 */
template<typename T2>
Stream combine(const Stream& left, const Channel<T2>& right);

/** Combine a channel and a stream.
 * This concatenates the given channel to the start of the list of
 * channels in the stream.
 */
template<typename T1>
Stream combine(const Channel<T1>& left, const Stream& right);

SH_DLLEXPORT
Stream combine(const Stream& left, const Stream& right);

/** An operator alias for combine between channels.
 */
template<typename T1, typename T2>
Stream operator&(const Channel<T1>& left, const Channel<T2>& right);

/** An operator alias for combine between a stream and a channel.
 */
template<typename T2>
Stream operator&(const Stream& left, const Channel<T2>& right);

/** An operator alias for combine between a channel and a stream.
 */
template<typename T1>
Stream operator&(const Channel<T1>& left, const Stream& right);

/** An operator alias for combine between two streams.
 */
SH_DLLEXPORT
Stream operator&(const Stream& left, const Stream& right);

/** Apply a program to a stream. 
 * This function connects streams onto the output of programs
 * TODO: is this right?  why is the stream argument first?
 */
SH_DLLEXPORT
Program connect(const Stream& stream, const Program& program);

/** An operator alias for connect(p,s).
 */
SH_DLLEXPORT
Program operator<<(const Program& program, const Stream& stream);


}

#include "StreamImpl.hpp"

#endif
