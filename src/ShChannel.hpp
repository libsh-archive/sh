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

// The client interface to a single-channel stream.
// Really this hides an ShChannelNode which is the true representation
// of the stream.
template<typename T>
class ShChannel {
public:
  typedef typename T::ValueType ValueType;
  ShChannel();
  ShChannel(ValueType* data, int count);

  // TODO: copy constructor, operator=(), etc.
  
  void attach(ValueType* data, int count);

  int count() const;
  ValueType* data();
  const ValueType* data() const;
  
  T operator()();
  const T operator()() const;

  ShChannelNodePtr node();
  const ShChannelNodePtr node() const;
  
private:
  ShChannelNodePtr m_node;
};

// Bind a channel as an input to a program
template<typename T>
ShProgram connect(const ShChannel<T>& channel, const ShProgram& program);

template<typename T>
ShProgram operator<<(const ShProgram& program, const ShChannel<T>& channel);

}

#include "ShChannelImpl.hpp"

#endif
