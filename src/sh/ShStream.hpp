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
#include "ShDllExport.hpp"
#include "ShChannel.hpp"
#include "ShChannelNode.hpp"

namespace SH {

/** Dynamic list of channels.
 * The stream keep track (by reference) of an ordered lists of 
 * data channels, to be used as inputs and outputs of stream operations.
 * @see ShChannel
 */
class
SH_DLLEXPORT ShStream {
public:
  typedef std::list<ShChannelNodePtr> NodeList;
  typedef NodeList::iterator iterator;
  typedef NodeList::const_iterator const_iterator;

  ShStream();
  ShStream(const ShChannelNodePtr& node);
  
  template<typename T>
  ShStream(const ShChannel<T>& channel);

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();
  int size() const;

  template<typename T>
  void append(const ShChannel<T>& channel);
  template<typename T>
  void prepend(const ShChannel<T>& channel);

  void append(const ShChannelNodePtr& node);
  void prepend(const ShChannelNodePtr& node);
  // Execute fully bound stream program and place results in stream.
  ShStream& operator=(const ShProgram& program);
  
private:
  NodeList m_nodes;
};

/** Combine two streams.
 * This concatenates the list of channels in the component streams.
 */
template<typename T1, typename T2>
ShStream combine(const ShChannel<T1>& left, const ShChannel<T2>& right);

/** Combine a stream and a channel.
 * This concatenates the given channel to the end of the list of
 * channels in the stream.
 */
template<typename T2>
ShStream combine(const ShStream& left, const ShChannel<T2>& right);

/** Combine a channel and a stream.
 * This concatenates the given channel to the start of the list of
 * channels in the stream.
 */
template<typename T1>
ShStream combine(const ShChannel<T1>& left, const ShStream& right);

SH_DLLEXPORT
ShStream combine(const ShStream& left, const ShStream& right);

/** An operator alias for combine between channels.
 */
template<typename T1, typename T2>
ShStream operator&(const ShChannel<T1>& left, const ShChannel<T2>& right);

/** An operator alias for combine between a stream and a channel.
 */
template<typename T2>
ShStream operator&(const ShStream& left, const ShChannel<T2>& right);

/** An operator alias for combine between a channel and a stream.
 */
template<typename T1>
ShStream operator&(const ShChannel<T1>& left, const ShStream& right);

/** An operator alias for combine between two streams.
 */
SH_DLLEXPORT
ShStream operator&(const ShStream& left, const ShStream& right);

/** Apply a program to a stream. 
 * This function connects streams onto the output of programs
 * TODO: is this right?  why is the stream argument first?
 */
SH_DLLEXPORT
ShProgram connect(const ShStream& stream, const ShProgram& program);

/** An operator alias for connect(p,s).
 */
SH_DLLEXPORT
ShProgram operator<<(const ShProgram& program, const ShStream& stream);


}

#include "ShStreamImpl.hpp"

#endif
