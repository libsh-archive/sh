#ifndef SHREFCOUNT_HPP
#define SHREFCOUNT_HPP

#include <utility>

namespace SH {

/** A class which can be reference-counted.
    These are classes you can wrap in an ShRefCount. Inherit from this
    if you want to reference-count your class */
class ShRefCountable 
{
public:
  ShRefCountable()
    : m_refCount(0)
  {
  }
  
  int acquireRef() 
  {
    return ++m_refCount;
  }
  
  int releaseRef()
  {
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
}

template<typename T>
ShRefCount<T>::ShRefCount(T* object)
  : m_object(object)
{
  if (m_object) m_object->acquireRef();
}

template<typename T>
ShRefCount<T>::ShRefCount(const ShRefCount<T>& other)
  : m_object(other.m_object)
{
  if (m_object) m_object->acquireRef();
}


template<typename T>
template<typename T1>
ShRefCount<T>::ShRefCount(const ShRefCount<T1>& other)
  : m_object(0)
{
  if (other.object()) {
    m_object = dynamic_cast<T*>(other.object());
    if (m_object) m_object->acquireRef();
  }
}

template<typename T>
ShRefCount<T>::~ShRefCount()
{
  releaseRef();
}

template<typename T>
ShRefCount<T>& ShRefCount<T>::operator=(const ShRefCount<T>& other)
{
  if (m_object == other.m_object) return *this;
  releaseRef();

  m_object = other.m_object;
  if (m_object) m_object->acquireRef();
  
  return *this;
}

template<typename T>
template<typename T1>
ShRefCount<T>& ShRefCount<T>::operator=(const ShRefCount<T1>& other)
{
  if (m_object == other.object()) return *this;
  releaseRef();

  if (!other.object()) {
    m_object = 0;
  } else {
    m_object = dynamic_cast<T*>(other.object());
    if (m_object) m_object->acquireRef();
  }
  
  return *this;
}

template<typename T>
ShRefCount<T>& ShRefCount<T>::operator=(T* object)
{
  if (m_object == object) return *this;
  releaseRef();

  m_object = object;
  if (m_object) m_object->acquireRef();
  
  return *this;
}

template<typename T>
bool ShRefCount<T>::operator==(const ShRefCount<T>& other) const
{
  return m_object == other.m_object;
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
  if (m_object && m_object->releaseRef() == 0) delete m_object;
}

} // namespace SH

#endif
