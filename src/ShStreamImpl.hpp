#ifndef SHSTREAMIMPL_HPP
#define SHSTREAMIMPL_HPP

#include "ShStream.hpp"
#include "ShDebug.hpp"
#include "ShVariable.hpp"

namespace SH {

// We may want to pass T::specialtype into the node. But there is no
// such thing yet -- specialtype is stored dynamically. I don't think
// there is a reason for this but it will take a few changes to
// ShAttrib, ShVariableNode and ShSpecialType at least to fix that.
template<typename T>
ShStream<T>::ShStream()
  : m_node(new ShStreamNode(T::typesize))
{
}

template<typename T>
ShStream<T>::ShStream(typename ShStream<T>::ValueType* data, int count)
  : m_node(new ShStreamNode(T::typesize, data, count))
{
}

template<typename T>
void ShStream<T>::attach(typename ShStream<T>::ValueType* data, int count)
{
  m_node->attach(data, count);
}

template<typename T>
int ShStream<T>::count() const
{
  return m_node->count();
}

template<typename T>
typename ShStream<T>::ValueType* ShStream<T>::data()
{
  return reinterpret_cast<typename ShStream<T>::ValueType*>(m_node->data());
}

template<typename T>
const typename ShStream<T>::ValueType* ShStream<T>::data() const
{
  return reinterpret_cast<const typename ShStream<T>::ValueType*>(m_node->data());
}

template<typename T>
T ShStream<T>::operator()()
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
const T ShStream<T>::operator()() const
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
ShProgram connect(const ShStream<T>& stream,
                  const ShProgram& program)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    // TODO: This could be improved dramatically if ShAttribs had a
    // OutputType typedef...
    // I.e.:
    // T::OutputType out = stream();
    // would be all that's needed.
    ShVariable out(new ShVariableNode(SH_VAR_OUTPUT,
                                      T::typesize, T::special_type));
    ShStatement stmt(out, SH_OP_ASN, stream());
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
  } SH_END_PROGRAM;
  return connect(nibble, program);
}

}


#endif
