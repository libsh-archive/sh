// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHCHANNEL_HPP
#define SHCHANNEL_HPP

#include "ShChannelNode.hpp"
#include "ShProgram.hpp"

namespace SH {

/** The client interface to a single-channel typed data stream.
 * Really this hides an ShChannelNode which is the true representation
 * of the channel.   The template argument is the element type stored.
 */
template<typename T>
class ShChannel {
public:
  typedef typename T::ValueType ValueType;
  ShChannel();
  ShChannel(const ShMemoryPtr& memory, int count);

  // TODO: copy constructor, operator=(), etc.
  
  void memory(const ShMemoryPtr& memory, int count);

  int count() const;
  ShRefCount<const ShMemory> memory() const;
  ShMemoryPtr memory();

  // Stream element fetch operator
  T operator()();
  const T operator()() const;

  ShChannelNodePtr node();
  const ShChannelNodePtr node() const;

  // Execute fully bound single-output stream program and place result in channel.
  ShChannel& operator=(const ShProgram& program);
  
private:
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
