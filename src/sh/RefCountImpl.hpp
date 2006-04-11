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
#ifndef SHREFCOUNTIMPL_HPP
#define SHREFCOUNTIMPL_HPP

#include "RefCount.hpp"

namespace SH {

template<typename T>
inline
Pointer<T>::Pointer()
  : m_object(0)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[cons] " << std::flush << std::setw(10) << "0" << " (default)" << std::endl;
  RCDEBUG_NORMAL;
#endif
}

template<typename T>
inline
Pointer<T>::Pointer(T* object)
  : m_object(object)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[cons] " << std::flush << std::setw(10) << object << " <" << (object ? typeid(*(object)).name() : "n/a") << ">" << std::endl;
  RCDEBUG_NORMAL;
#endif
  if (m_object) m_object->acquireRef();
}

template<typename T>
inline
Pointer<T>::Pointer(const Pointer<T>& other)
  : m_object(other.m_object)
{
#ifdef REFCOUNT_DEBUGGING
  DEBUG_ASSERT((unsigned long)m_object < 0xb0000000L);
  RCDEBUG_BLUE;
  std::cerr << "[copy] " << std::flush << std::setw(10) << other.m_object << " <" << (other.m_object ? typeid(*(other.m_object)).name() : "n/a") << ">" << std::endl;
  RCDEBUG_NORMAL;
#endif


  if (m_object) m_object->acquireRef();
}

template<typename T>
template<typename S>
inline
Pointer<T>::Pointer(const Pointer<S>& other)
  : m_object(other.object())
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[cpct] " << std::flush << std::setw(10) << other.object()
            << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
            << " -> " << typeid(T).name()
            << std::endl;
  RCDEBUG_NORMAL;
#endif

  if (m_object) m_object->acquireRef();
}

template<typename T>
inline
void Pointer<T>::releaseRef()
{
  if (m_object && m_object->releaseRef() == 0) {
#ifdef REFCOUNT_DEBUGGING
    RCDEBUG_RED;
    std::cerr << "[dstr] " << std::flush << std::setw(10) << m_object << " <" << typeid(*m_object).name() << ">" << std::endl;
    RCDEBUG_NORMAL;
#endif
    delete m_object;
#ifdef REFCOUNT_DEBUGGING
    m_object = 0;
#endif
  }
}

template<typename T>
inline
Pointer<T>::~Pointer()
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[dest] " << std::flush << std::setw(10) << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << std::endl;
  RCDEBUG_NORMAL;
#endif

  releaseRef();
}

template<typename T>
inline
void Pointer<T>::swap(Pointer<T>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[swap] " << std::flush << std::setw(10) << other.m_object << " <" << (other.m_object ? typeid(*(other.m_object)).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  RCDEBUG_NORMAL;
#endif
  T* t = m_object;
  m_object = other.m_object;
  other.m_object = t;
}

template<typename T>
inline
Pointer<T>& Pointer<T>::operator=(T* object)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[asn*] " << std::flush << std::setw(10) << object << " <" << (object ? typeid(*(object)).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  RCDEBUG_NORMAL;
#endif

  Pointer<T> t(object);
  t.swap(*this);
  
  return *this;
}

template<typename T>
inline
Pointer<T>& Pointer<T>::operator=(const Pointer<T>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[assn] " << std::flush << std::setw(10) << other.m_object << " <" << (other.m_object ? typeid(*(other.m_object)).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  RCDEBUG_NORMAL;
#endif

  Pointer<T> t(other);
  t.swap(*this);
  
  return *this;
}

template<typename T>
template<typename S>
inline
Pointer<T>& Pointer<T>::operator=(const Pointer<S>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[assn] " << std::flush << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">" << " (was " << m_object << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")" << std::endl;
  RCDEBUG_NORMAL;
#endif

  Pointer<T> t(other);
  t.swap(*this);

  return *this;
}

template<typename T>
inline
bool Pointer<T>::operator==(const Pointer<T>& other) const
{
  return m_object == other.m_object;
}

template<typename T>
inline
bool Pointer<T>::operator!=(const Pointer<T>& other) const
{
  return m_object != other.m_object;
}

template<typename T>
inline
bool Pointer<T>::operator<(const Pointer<T>& other) const
{
  return m_object < other.m_object; // TODO: Make sure this is portable...
}

template<typename T>
inline
T& Pointer<T>::operator*() const
{
  return *m_object;
}

template<typename T>
inline
T* Pointer<T>::operator->() const
{
  return m_object;
}

template<typename T>
inline
int Pointer<T>::refCount() const
{
  if (!m_object)
    return 0; // TODO: Maybe -1?
  else
    return m_object->refCount();
}

template<typename T>
inline
T* Pointer<T>::object() const
{
  return m_object;
}


template<typename T, typename S>
Pointer<T> shref_static_cast(const Pointer<S>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[csts] " << std::flush << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  RCDEBUG_NORMAL;
#endif

  return Pointer<T>(static_cast<T*>(other.object()));
}

template<typename T, typename S>
Pointer<T> shref_dynamic_cast(const Pointer<S>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[cstd] " << std::flush << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  RCDEBUG_NORMAL;
#endif
  return Pointer<T>(dynamic_cast<T*>(other.object()));
}

template<typename T, typename S>
Pointer<T> shref_const_cast(const Pointer<S>& other)
{
#ifdef REFCOUNT_DEBUGGING
  RCDEBUG_BLUE;
  std::cerr << "[cstc] " << std::flush << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
	    << " -> " << typeid(T).name() << std::endl;
  RCDEBUG_NORMAL;
#endif
  return Pointer<T>(const_cast<T*>(other.object()));
}

}

#endif
