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
  std::cerr << "[copy] " << std::setw(10) << other.object()
            << " <" << (other.object() ? typeid(*(other.m_object)).name() : "n/a") << ">"
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
    delete m_object;
#ifdef SH_REFCOUNT_DEBUGGING
    m_object = 0;
#endif
  }
}

template<typename T, typename S>
ShRefCount<T> shref_static_cast(const ShRefCount<S>& other)
{
  return ShRefCount<T>(static_cast<T*>(other.object()));
}

template<typename T, typename S>
ShRefCount<T> shref_dynamic_cast(const ShRefCount<S>& other)
{
  return ShRefCount<T>(dynamic_cast<T*>(other.object()));
}

template<typename T, typename S>
ShRefCount<T> shref_const_cast(const ShRefCount<S>& other)
{
  return ShRefCount<T>(const_cast<T*>(other.object()));
}

}

#endif
