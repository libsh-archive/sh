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

#include "BaseTexture.hpp"

namespace SH {

template<typename T>
class Channel
  : public BaseTexture1D<T> {
public:
  /// Construct a channel without any associated memory
  Channel()
    : BaseTexture1D<T>(ArrayTraits())
  {}
  /// Construct a channel with a mamory containing \a count elements
  Channel(int count);
  /// Construct a channel with \a count elements in \a memory
  Channel(const MemoryPtr& memory, int count);

  Channel<T>& operator=(const Program& program);

  /// Get the offset for this stream  
  int offset() const;
  /// Get the stride for this stream
  int stride() const;
  /// Get the number of elements in this stream
  int count() const;
  /// Set the offset for this stream
  void offset(int offset);
  /// Set the stride for this stream
  void stride(int stride);
  /// Set the number of elements in this stream
  void count(int count);

  /// Return this channel's memory
  MemoryCPtr memory() const;
  /// Return this channel's memory
  MemoryPtr memory();
  
  /// Sync and return a pointer to the channel's host storage
  typename T::mem_type* read_data()
  { return static_cast<typename T::mem_type*>(BaseTexture::read_data(0)); }
  
  /// Dirty and return a pointer to the channel's host storage
  typename T::mem_type* write_data()
  { return static_cast<typename T::mem_type*>(BaseTexture::write_data(0)); }
};


}

#include "ChannelImpl.hpp"

#endif
