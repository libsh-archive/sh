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
#ifndef SHSTREAM_HPP
#define SHSTREAM_HPP

#include "ShChannel.hpp"
#include "ShChannelNode.hpp"
#include <list>

namespace SH {

// Dynamic list of channels
class ShStream {
public:
  template<typename T>
  ShStream(const ShChannel<T>& channel);

  typedef std::list<ShChannelNodePtr> NodeList;

  NodeList::const_iterator begin() const;
  NodeList::const_iterator end() const;
  int size() const;

  template<typename T>
  void append(const ShChannel<T>& channel);
  template<typename T>
  void prepend(const ShChannel<T>& channel);

  void append(const ShChannelNodePtr& node);
  void prepend(const ShChannelNodePtr& node);
  
private:
  std::list<ShChannelNodePtr> m_nodes;
};

// Ways to form a combined stream
template<typename T1, typename T2>
ShStream combine(const ShChannel<T1>& left, const ShChannel<T2>& right);

template<typename T2>
ShStream combine(const ShStream& left, const ShChannel<T2>& right);

template<typename T1>
ShStream combine(const ShChannel<T1>& left, const ShStream& right);

ShStream combine(const ShStream& left, const ShStream& right);

// Aliases for combine
template<typename T1, typename T2>
ShStream operator&(const ShChannel<T1>& left, const ShChannel<T2>& right);

template<typename T2>
ShStream operator&(const ShStream& left, const ShChannel<T2>& right);

template<typename T1>
ShStream operator&(const ShChannel<T1>& left, const ShStream& right);

ShStream operator&(const ShStream& left, const ShStream& right);

// Connecting (currying) streams onto programs
ShProgram connect(const ShStream& stream, const ShProgram& program);
ShProgram operator<<(const ShProgram& program, const ShStream& stream);

}

#include "ShStreamImpl.hpp"

#endif
