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
#ifndef SHREFCOUNT_HPP
#define SHREFCOUNT_HPP

#include <utility>
#include "DllExport.hpp"

// #define REFCOUNT_DEBUGGING

#ifdef REFCOUNT_DEBUGGING
#include <iostream>
#include <iomanip>
#include "Debug.hpp"

#define RCDEBUG_GREEN std::cerr << "[32m"
#define RCDEBUG_RED std::cerr << "[31m"
#define RCDEBUG_BLUE std::cerr << "[34m"
#define RCDEBUG_NORMAL std::cerr << "[0m"

#endif

namespace SH {

/** A class which can be reference-counted.
 * These are classes you can wrap in an RefCount. Inherit from this
 * if you want to reference-count your class.
 */
class 
DLLEXPORT
RefCountable 
{
public:
  RefCountable()
    : m_refCount(0)
  {
  }

  RefCountable(const RefCountable&)
    : m_refCount(0)
    // any new RefCountable should have a zero refcount, even if it's
    // made as a copy
  {
  }

  RefCountable& operator=(const RefCountable&)
  {
    // we don't actually change refCount here
    // this is indeed the intended behaviour
    return *this;
  }

#ifdef REFCOUNT_DEBUGGING
  // Just to make this polymorphic, so typeid() works as expected
  virtual ~RefCountable() {}   
#endif
  
  int acquireRef() const
  {
#ifdef REFCOUNT_DEBUGGING
    RCDEBUG_GREEN;
    std::cerr << "   [+] " << std::setw(10) << this << " <" << typeid(*this).name() << ">"
              << ": " << m_refCount << "->" << (m_refCount + 1) << std::endl;
    RCDEBUG_NORMAL;
#endif
    return ++m_refCount;
  }
  
  int releaseRef() const
  {
#ifdef REFCOUNT_DEBUGGING
    RCDEBUG_RED;
    std::cerr << "   [-] " << std::setw(10) << this << " <" << typeid(*this).name() << ">"
              << ": " << m_refCount << "->" << (m_refCount - 1) << std::endl;
    RCDEBUG_NORMAL;
#endif
    return --m_refCount;
  }

  int refCount() const
  {
    return m_refCount;
  }

private:
  mutable int m_refCount;
};

/** A reference-counting smart pointer. 
 */
template<typename T>
class Pointer 
{
public:
  Pointer();
  Pointer(T* object);
  Pointer(const Pointer& other);
  template<typename S>
  Pointer(const Pointer<S>& other);
  
  ~Pointer();

  Pointer& operator=(T* other);
  Pointer& operator=(const Pointer& other);
  template<typename S>
  Pointer& operator=(const Pointer<S>& other);

  /// Two references are equal if they point to the same object.
  bool operator==(const Pointer& other) const;

  /// Two references are equal if they point to the same object.
  bool operator!=(const Pointer& other) const;

  /// Actually compares the pointers.
  bool operator<(const Pointer& other) const;

  T& operator*() const;
  T* operator->() const;

  // Idea taken from the Boost shared_ptr implementation.
  typedef T * (Pointer<T>::*unspecified_bool_type)() const;
    
  /// Return true iff this is not a reference to a null pointer
  operator unspecified_bool_type() const
  {
    return m_object == 0 ? 0 : &Pointer<T>::object;
  }  

  /// Obtain the total amount of references to the referenced object.
  int refCount() const;

  /// Obtain a pointer to the object we reference count
  T* object() const;

  void swap(Pointer& other);

private:
  void releaseRef();
  
  T* m_object;
};

template<typename T, typename S>
Pointer<T> shref_static_cast(const Pointer<S>& other);

template<typename T, typename S>
Pointer<T> shref_dynamic_cast(const Pointer<S>& other);

template<typename T, typename S>
Pointer<T> shref_const_cast(const Pointer<S>& other);

} // namespace SH

#include "RefCountImpl.hpp"

#endif
