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
#ifndef SHREFCOUNTIMPL_HPP
#define SHREFCOUNTIMPL_HPP

#include "ShRefCount.hpp"

namespace SH {

template<typename T>
ShRefCount<T>::ShRefCount()
  : m_object(0)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cons] " << std::setw(10) << "0" << " (default)" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
}

template<typename T>
ShRefCount<T>::ShRefCount(T* object)
  : m_object(object)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cons] " << std::setw(10) << object << " <" << (object ? typeid(*(object)).name() : "n/a") << ">" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  if (m_object) m_object->acquireRef();
}

template<typename T>
ShRefCount<T>::ShRefCount(const ShRefCount<T>& other)
  : m_object(other.m_object)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_DEBUG_ASSERT((unsigned long)m_object < 0xb0000000L);
  SH_RCDEBUG_BLUE;
  std::cerr << "[copy] " << std::setw(10) << other.m_object << " <" << (other.m_object ? typeid(*(other.m_object)).name() : "n/a") << ">" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif


  if (m_object) m_object->acquireRef();
}

template<typename T>
template<typename S>
ShRefCount<T>::ShRefCount(const ShRefCount<S>& other)
  : m_object(other.object())
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cpct] " << std::setw(10) << other.object()
            << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
            << " -> " << typeid(T).name()
            << std::endl;
  SH_RCDEBUG_NORMAL;
#endif

  if (m_object) m_object->acquireRef();
}

template<typename T>
ShRefCount<T>::~ShRefCount()
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[dest] " << std::setw(10) << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif

  releaseRef();
}

template<typename T>
ShRefCount<T>& ShRefCount<T>::operator=(T* object)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[asn*] " << std::setw(10) << object << " <" << (object ? typeid(*(object)).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  
  if (m_object == object) return *this;
  if( object ) object->acquireRef();
  releaseRef();

  m_object = object;
  
  return *this;
}

template<typename T>
ShRefCount<T>& ShRefCount<T>::operator=(const ShRefCount<T>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[assn] " << std::setw(10) << other.m_object << " <" << (other.m_object ? typeid(*(other.m_object)).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  if (m_object == other.m_object) return *this;

  // Bug fix - must aquireRef to other.m_object first in case
  // m_object has the only current pointer to other.m_object
  if(other.m_object) other.m_object->acquireRef();
  releaseRef();

  m_object = other.m_object;

  return *this;
}

template<typename T>
template<typename S>
ShRefCount<T>& ShRefCount<T>::operator=(const ShRefCount<S>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[assn] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  if (m_object == other.object()) return *this;

  // Bug fix - must aquireRef to other.object() first in case
  // m_object has the only current pointer to other.object()
  if(other.object()) other.object()->acquireRef();
  releaseRef();

  m_object = other.object();

  return *this;
}

template<typename T>
bool ShRefCount<T>::operator==(const ShRefCount<T>& other) const
{
  return m_object == other.m_object;
}

template<typename T>
bool ShRefCount<T>::operator!=(const ShRefCount<T>& other) const
{
  return m_object != other.m_object;
}

template<typename T>
bool ShRefCount<T>::operator<(const ShRefCount<T>& other) const
{
  return m_object < other.m_object; // TODO: Make sure this is portable...
}


template<typename T>
T& ShRefCount<T>::operator*() const
{
  return *m_object;
}

template<typename T>
T* ShRefCount<T>::operator->() const
{
  return m_object;
}

template<typename T>
ShRefCount<T>::operator bool() const
{
  return m_object != 0;
}

template<typename T>
int ShRefCount<T>::refCount() const
{
  if (!m_object)
    return 0; // TODO: Maybe -1?
  else
    return m_object->refCount();
}


template<typename T>
T* ShRefCount<T>::object() const
{
  return m_object;
}

template<typename T>
void ShRefCount<T>::releaseRef()
{
  if (m_object && m_object->releaseRef() == 0) {
#ifdef SH_REFCOUNT_DEBUGGING
    SH_RCDEBUG_RED;
    std::cerr << "[dstr] " << std::setw(10) << m_object << " <" << typeid(*m_object).name() << ">" << std::endl;
    SH_RCDEBUG_NORMAL;
#endif
    delete m_object;
#ifdef SH_REFCOUNT_DEBUGGING
    m_object = 0;
#endif
  }
}

template<typename T, typename S>
ShRefCount<T> shref_static_cast(const ShRefCount<S>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[csts] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  SH_RCDEBUG_NORMAL;
#endif

  return ShRefCount<T>(static_cast<T*>(other.object()));
}

template<typename T, typename S>
ShRefCount<T> shref_dynamic_cast(const ShRefCount<S>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cstd] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  return ShRefCount<T>(dynamic_cast<T*>(other.object()));
}

template<typename T, typename S>
ShRefCount<T> shref_const_cast(const ShRefCount<S>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cstc] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  SH_RCDEBUG_NORMAL;
#endif
  return ShRefCount<T>(const_cast<T*>(other.object()));
}

}

#endif
