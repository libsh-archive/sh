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
#ifndef SHSTREAMIMPL_HPP
#define SHSTREAMIMPL_HPP

#include "Stream.hpp"
#include "Debug.hpp"
#include "Variable.hpp"
#include "Context.hpp"
#include "Program.hpp"
#include "Syntax.hpp"
#include "Statement.hpp"
#include "Algebra.hpp"
#include "Error.hpp"
#include "Exception.hpp"

namespace SH {

template<typename T>
Channel<T>::Channel()
  : MetaForwarder(0),
    m_node(new ChannelNode(T::semantic_type, T::typesize, T::value_type))
{
  real_meta(m_node.object());
}

template<typename T>
Channel<T>::Channel(int count)
  : MetaForwarder(0),
    m_node(new ChannelNode(T::semantic_type, T::typesize, T::value_type))
{
  real_meta(m_node.object());
  HostMemoryPtr mem = new HostMemory(count * T::typesize * sizeof(typename T::mem_type), T::value_type);
  m_node->memory(mem, count);
}

template<typename T>
Channel<T>::Channel(const MemoryPtr& memory, int count)
  : MetaForwarder(0),
    m_node(new ChannelNode(T::semantic_type, T::typesize, T::value_type, memory, count))
{
  real_meta(m_node.object());
}

template<typename T>
void Channel<T>::memory(const MemoryPtr& memory, int count)
{
  m_node->memory(memory, count);
}

template<typename T>
int Channel<T>::count() const
{
  return m_node->count();
}

template<typename T>
void Channel<T>::count(int count)
{
  m_node->count(count);
}

template<typename T>
void Channel<T>::stride(int stride)
{
  m_node->stride(stride);
}

template<typename T>
void Channel<T>::offset(int offset)
{
  m_node->offset(offset);
}

template<typename T>
int Channel<T>::stride()
{
  return m_node->stride();
}

template<typename T>
int Channel<T>::offset()
{
  return m_node->offset();
}

template<typename T>
MemoryPtr Channel<T>::memory()
{
  return m_node->memory();
}

template<typename T>
Pointer<const Memory> Channel<T>::memory() const
{
  return m_node->memory();
}

template<typename T>
ChannelNodePtr Channel<T>::node()
{
  return m_node;
}

template<typename T>
const ChannelNodePtr Channel<T>::node() const
{
  return m_node;
}

template<typename T>
typename T::mem_type* Channel<T>::read_data()
{
  StoragePtr storage = memory()->findStorage("host");
  if (!storage) error(Exception("No host storage found"));
  HostStoragePtr host_storage = shref_dynamic_cast<HostStorage>(storage);
  DEBUG_ASSERT(host_storage);
  host_storage->sync();
  return static_cast<typename T::mem_type*>(host_storage->data());
}

template<typename T>
typename T::mem_type* Channel<T>::write_data()
{
  StoragePtr storage = memory()->findStorage("host");
  if (!storage) error(Exception("No host storage found"));
  HostStoragePtr host_storage = shref_dynamic_cast<HostStorage>(storage);
  DEBUG_ASSERT(host_storage);
  host_storage->dirty();
  return static_cast<typename T::mem_type*>(host_storage->data());
}

template<typename T>
T Channel<T>::operator()() const
{
  // TODO: error() maybe instead.
  if (!Context::current()->parsing()) error(ScopeException("Stream fetch outside program"));
  
  T t;
  Variable streamVar(m_node);
  Statement stmt(t, OP_FETCH, streamVar);

  Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  
  return t;
}

template<typename T>
template<typename T2>
T Channel<T>::operator[](const Generic<1, T2>& index) const
{
  // TODO: error() maybe instead.
  if (!Context::current()->parsing()) error(ScopeException("Indexed stream fetch outside program"));
  
  T t;
  Variable streamVar(m_node);
  Statement stmt(t, streamVar, OP_LOOKUP, index);

  Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  
  return t;
}

template<typename T>
Program connect(const Channel<T>& stream,
                  const Program& program)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::OutputType out = stream();
  } SH_END_PROGRAM;
  return connect(nibble, program);
}

template<typename T>
Program operator<<(const Program& program,
                     const Channel<T>& stream)
{
  return connect(stream, program);
}

template<typename T>
Channel<T>& Channel<T>::operator=(const Program& program)
{
  Stream stream(*this);
  stream = program;
  return *this;
}

// Put these here for dependency reasons, even though they are member
// functions of Program
template<typename T0>
Program Program::operator()(const Channel<T0>& t0) const
{
  return (*this) << t0;
}

template<typename T0, typename T1>
Program Program::operator()(const Channel<T0>& t0,
                                 const Channel<T1>& t1) const
{
  return (*this) << t0 << t1;
}

template<typename T0, typename T1, typename T2>
Program Program::operator()(const Channel<T0>& t0,
                                 const Channel<T1>& t1,
                                 const Channel<T2>& t2) const
{
  return (*this) << t0 << t1 << t2;
}

template<typename T0, typename T1, typename T2, typename T3>
Program Program::operator()(const Channel<T0>& t0,
                                 const Channel<T1>& t1,
                                 const Channel<T2>& t2,
                                 const Channel<T3>& t3) const
{
  return (*this) << t0 << t1 << t2 << t3;
}

template<typename T0, typename T1, typename T2, typename T3,
         typename T4>
Program Program::operator()(const Channel<T0>& t0,
                                 const Channel<T1>& t1,
                                 const Channel<T2>& t2,
                                 const Channel<T3>& t3,
                                 const Channel<T4>& t4) const
{
  return (*this) << t0 << t1 << t2 << t3 << t4;
}

}


#endif
