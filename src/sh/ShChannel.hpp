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
#ifndef SHCHANNEL_HPP
#define SHCHANNEL_HPP

#include "ShChannelNode.hpp"
#include "ShProgram.hpp"
#include "ShGeneric.hpp"

namespace SH {

/** The client interface to a single-channel typed data stream.
 * Really this hides an ShChannelNode which is the true representation
 * of the channel.   The template argument is the element type stored.
 * @todo copy constructor, operator=(), etc.
 */
template<typename T>
class ShChannel : public ShMetaForwarder {
public:
  /// Construct a channel without any associated memory.
  ShChannel();
  /// Construct a channel with \a count elements in \a memory
  ShChannel(const ShMemoryPtr& memory, int count);

  /// Set this channel to use \a count elements in \a memory
  void memory(const ShMemoryPtr& memory, int count);

  /// Return the number of elements in this channel
  int count() const;
  /// Set the number of elements in this channel
  void count(int c);
  /// Return this channel's memory
  ShPointer<const ShMemory> memory() const;
  /// Return this channel's memory
  ShMemoryPtr memory();

  /// Set the stride on the elements of this channel
  void stride(int stride);
  /// Set the offset on the elements of this channel
  void offset(int offset);
  
  /// Fetch the stride for this stream
  int stride();
  /// Fetch the offset for this stream
  int offset();

  /// Fetch the current element from this stream.
  /// This is only useful in stream programs
  T operator()() const;

  /// Indexed lookup from the stream
  template<typename T2>
  T operator[](const ShGeneric<1, T2>& index) const;

  /// Return the node internally wrapped by this channel object
  ShChannelNodePtr node();
  /// Return the node internally wrapped by this channel object
  const ShChannelNodePtr node() const;

  /// Execute fully bound single-output stream program and place result in channel.
  ShChannel& operator=(const ShProgram& program);
  
private:
  /// The node this object is a thin wrapper for
  ShChannelNodePtr m_node;
};

/** Apply a programs to a single channel.
 * Bind a channel as an input to a program.   The implementation
 * supports currying, and returns a program with one less input.
 */
template<typename T>
ShProgram connect(const ShChannel<T>& channel, const ShProgram& program);

/** Equivalent to connect(p,c). 
 * Bind a channel as an input to a program.   The implementation
 * supports currying, and returns a program with one less input.
 */
template<typename T>
ShProgram operator<<(const ShProgram& program, const ShChannel<T>& channel);

}

#include "ShChannelImpl.hpp"

#endif
