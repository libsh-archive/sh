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
#ifndef SHSTREAMIMPL_HPP
#define SHSTREAMIMPL_HPP

#include "ShStream.hpp"
#include "ShDebug.hpp"
#include "ShVariable.hpp"
#include "ShEnvironment.hpp"
#include "ShProgram.hpp"
#include "ShSyntax.hpp"
#include "ShStatement.hpp"

namespace SH {

template<typename T>
ShChannel<T>::ShChannel()
  : m_node(new ShChannelNode(T::special_type, T::typesize))
{
}

template<typename T>
ShChannel<T>::ShChannel(const ShMemoryPtr& memory, int count)
  : m_node(new ShChannelNode(T::special_type, T::typesize, memory, count))
{
}

template<typename T>
void ShChannel<T>::memory(const ShMemoryPtr& memory, int count)
{
  m_node->memory(memory, count);
}

template<typename T>
int ShChannel<T>::count() const
{
  return m_node->count();
}

template<typename T>
ShMemoryPtr ShChannel<T>::memory()
{
  return m_node->memory();
}

template<typename T>
ShRefCount<const ShMemory> ShChannel<T>::memory() const
{
  return m_node->memory();
}

template<typename T>
ShChannelNodePtr ShChannel<T>::node()
{
  return m_node;
}

template<typename T>
const ShChannelNodePtr ShChannel<T>::node() const
{
  return m_node;
}

template<typename T>
T ShChannel<T>::operator()()
{
  // TODO: shError() maybe instead.
  SH_DEBUG_ASSERT(ShEnvironment::insideShader);
  
  T t;
  ShVariable streamVar(m_node);
  ShStatement stmt(t, SH_OP_FETCH, streamVar);

  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  
  return t;
}
template<typename T>
const T ShChannel<T>::operator()() const
{
  // TODO: shError() maybe instead.
  SH_DEBUG_ASSERT(ShEnvironment::insideShader);
  
  T t;
  ShVariable streamVar(m_node);
  ShStatement stmt(t, SH_OP_FETCH, streamVar);

  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  
  return t;
}

template<typename T>
ShProgram connect(const ShChannel<T>& stream,
                  const ShProgram& program)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::OutputType out = stream();
  } SH_END_PROGRAM;
  return connect(nibble, program);
}

template<typename T>
ShProgram operator<<(const ShProgram& program,
                     const ShChannel<T>& stream)
{
  return connect(stream, program);
}

template<typename T>
ShChannel<T>& ShChannel<T>::operator=(const ShProgram& program)
{
  ShStream stream(*this);
  stream = program;
  return *this;
}

}


#endif
