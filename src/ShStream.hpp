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

#include "ShStreamNode.hpp"

namespace SH {

// The client interface to a stream.
// Really this hides an ShStreamNode which is the true representation
// of the stream.
template<typename T>
class ShStream {
public:
  typedef typename T::ValueType ValueType;
  ShStream();
  ShStream(ValueType* data, int count);

  // TODO: copy constructor, operator=(), etc.
  
  void attach(ValueType* data, int count);

  int count() const;
  ValueType* data();
  const ValueType* data() const;
  
  T operator()();
  const T operator()() const;
  
private:
  ShStreamNodePtr m_node;
};

template<typename T>
ShProgram connect(const ShStream<T>& stream,
                  const ShProgram& program);

/* TODO

template<typename T1, typename T2>
class ShCombinedStream {
public:
  typedef T1 LeftType;
  typedef T2 RightType;

private:
  LeftType left;
  RightType right;
};

template<typename T1, typename T2>
ShCombinedStream< ShStream<T1>, ShStream<T2> > operator&(const ShStream<T1>& left,
                                                         const ShStream<T2>& right);

template<typename T1, typename T2>
ShCombinedStream< ShCombinedStream<T1>, ShStream<T2> > operator&(const ShCombinedStream<T1>& left,
                                                                 const ShStream<T2>& right);

template<typename T1, typename T2>
ShCombinedStream< ShStream<T1>, ShCombinedStream<T2> > operator&(const ShStream<T1>& left,
                                                                 const ShCombinedStream<T2>& right);

template<typename T1, typename T2>
ShCombinedStream< ShCombinedStream<T1>, ShCombinedStream<T2> > operator&(const ShCombinedStream<T1>& left,
                                                                         const ShCombinedStream<T2>& right);
*/

}

#include "ShStreamImpl.hpp"

#endif
