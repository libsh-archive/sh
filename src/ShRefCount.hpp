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
#ifndef SHREFCOUNT_HPP
#define SHREFCOUNT_HPP

#include <utility>

//#define SH_REFCOUNT_DEBUGGING

#ifdef SH_REFCOUNT_DEBUGGING
#include <iostream>
#include <iomanip>
#include "ShDebug.hpp"

#define SH_RCDEBUG_GREEN std::cerr << "[32m"
#define SH_RCDEBUG_RED std::cerr << "[31m"
#define SH_RCDEBUG_BLUE std::cerr << "[34m"
#define SH_RCDEBUG_NORMAL std::cerr << "[0m"

#endif

namespace SH {

/** A class which can be reference-counted.
    These are classes you can wrap in an ShRefCount. Inherit from this
    if you want to reference-count your class.
*/
class ShRefCountable 
{
public:
  ShRefCountable()
    : m_refCount(0)
  {
  }

  ShRefCountable(const ShRefCountable&)
    : m_refCount(0)
    // any new RefCountable should have a zero refcount, even if it's
    // made as a copy
  {
  }

  ShRefCountable& operator=(const ShRefCountable&)
  {
    // we don't actually change refCount here
    // this is indeed the intended behaviour
    return *this;
  }

#ifdef SH_REFCOUNT_DEBUGGING
  // Just to make this polymorphic, so typeid() works as expected
  virtual ~ShRefCountable() {}   
#endif
  
  int acquireRef() 
  {
#ifdef SH_REFCOUNT_DEBUGGING
    SH_RCDEBUG_GREEN;
    std::cerr << "   [+] " << std::setw(10) << this << " <" << typeid(*this).name() << ">"
              << ": " << m_refCount << "->" << (m_refCount + 1) << std::endl;
    SH_RCDEBUG_NORMAL;
#endif
    return ++m_refCount;
  }
  
  int releaseRef()
  {
#ifdef SH_REFCOUNT_DEBUGGING
    SH_RCDEBUG_RED;
    std::cerr << "   [-] " << std::setw(10) << this << " <" << typeid(*this).name() << ">"
              << ": " << m_refCount << "->" << (m_refCount - 1) << std::endl;
    SH_RCDEBUG_NORMAL;
#endif
    return --m_refCount;
  }

  int refCount() const
  {
    return m_refCount;
  }

private:
  int m_refCount;
};

/** A reference-counting smart pointer. */
template<typename T>
class ShRefCount 
{
public:
  ShRefCount();
  ShRefCount(T* object);
  ShRefCount(const ShRefCount<T>& other);
  /// Attempt to dynamically cast another object to an object of this type.
  template<typename T1>
  ShRefCount(const ShRefCount<T1>& other);
  
  ~ShRefCount();

  ShRefCount<T>& operator=(const ShRefCount<T>& other);
  ShRefCount<T>& operator=(T* other);

  /// Attempt to dynamically cast another object to an object of this type.
  template<typename T1>
  ShRefCount<T>& operator=(const ShRefCount<T1>& other);

  /// Two references are equal if they point to the same object.
  bool operator==(const ShRefCount<T>& other) const;

  /// Two references are equal if they point to the same object.
  bool operator!=(const ShRefCount<T>& other) const;

  /// Actually compares the pointers.
  bool operator<(const ShRefCount<T>& other) const;

  T& operator*();
  const T& operator*() const;
  T* operator->();
  const T* operator->() const;

  /// Return true iff this is not a reference to a null pointer
  operator bool() const;

  /// Obtain the total amount of references to the referenced object.
  int refCount() const;

  /// Provided only for backwards compatibility
  /// Try not to call the function below if possible.
  T* object() const;

private:
  void releaseRef();
  
  T* m_object;
};

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
template<typename T1>
ShRefCount<T>::ShRefCount(const ShRefCount<T1>& other)
  : m_object(0)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cast] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">"
            << "(to <" << typeid(*this).name() << ">)" << std::endl;
  SH_RCDEBUG_NORMAL;
#endif

  if (other.object()) {
    m_object = dynamic_cast<T*>(other.object());
    if (m_object) m_object->acquireRef();
  }
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
template<typename T1>
ShRefCount<T>& ShRefCount<T>::operator=(const ShRefCount<T1>& other)
{
#ifdef SH_REFCOUNT_DEBUGGING
  SH_RCDEBUG_BLUE;
  std::cerr << "[cst=] " << std::setw(10) << other.object() << " <" << (other.object() ? typeid(*(other.object())).name() : "n/a") << ">" << " (was " << " <" << (m_object ? typeid(*(m_object)).name() : "n/a") << ">" << ")"
            << "(to <" << typeid(*this).name() << ">)" << std::endl;

  SH_RCDEBUG_NORMAL;
#endif
  
  if (m_object == other.object()) return *this;

  // Bug fix - moved releaseRef down to prevent same problem as above
  if (!other.object()) {
    releaseRef();
    m_object = 0;
  } else {
    T* temp = dynamic_cast<T*>(other.object());
    if (temp) temp->acquireRef();
    releaseRef();
    m_object = temp;
  }
  
  return *this;
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
T& ShRefCount<T>::operator*()
{
  return *m_object;
}

template<typename T>
const T& ShRefCount<T>::operator*() const
{
  return *m_object;
}

template<typename T>
T* ShRefCount<T>::operator->()
{
  return m_object;
}

template<typename T>
const T* ShRefCount<T>::operator->() const
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
  if (m_object && m_object->releaseRef() <= 0) {
    delete m_object;
#ifdef SH_REFCOUNT_DEBUGGING
    m_object = 0;
#endif
  }
}

} // namespace SH

#endif
