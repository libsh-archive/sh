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
ShChannel<T>::ShChannel(typename ShChannel<T>::ValueType* data, int count)
  : m_node(new ShChannelNode(T::special_type, T::typesize, data, count))
{
}

template<typename T>
void ShChannel<T>::attach(typename ShChannel<T>::ValueType* data, int count)
{
  m_node->attach(data, count);
}

template<typename T>
int ShChannel<T>::count() const
{
  return m_node->count();
}

template<typename T>
typename ShChannel<T>::ValueType* ShChannel<T>::data()
{
  return reinterpret_cast<typename ShChannel<T>::ValueType*>(m_node->data());
}

template<typename T>
const typename ShChannel<T>::ValueType* ShChannel<T>::data() const
{
  return reinterpret_cast<const typename ShChannel<T>::ValueType*>(m_node->data());
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

template<typename T>
ShProgram operator<<(const ShProgram& program,
                     const ShChannel<T>& stream)
{
  return connect(stream, program);
}

}


#endif
