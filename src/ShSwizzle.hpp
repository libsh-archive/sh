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
#ifndef SHSWIZZLE_HPP
#define SHSWIZZLE_HPP

#include <iosfwd>
//#include <vector>
#include "ShDllExport.hpp"
#include "ShException.hpp"

// This uses char indices instead of ints and uses a union of char[4] instead
// of a dynamically allocated pointer for small swizzles
#define SH_SWIZZLE_CHAR 

// This uses a bool to indicate identity if SH_SWIZZLE_CHAR
// or an unallocated m_indices = 0 if not SH_SWIZZLE_CHAR
//
// Prevents having to go through the array to check for identities and 
// removes a memory lookup when getting indices from identity swizzles 
// @todo not implemented yet
//#define SH_SWIZZLE_ID

namespace SH {

/** Represents swizzling of a variable.
 * Swizzling takes at least one element from an n-tuple and in
 * essence makes a new n-tuple with those elements in it. To actually
 * perform a swizzle using Sh, you should use the operator() on the
 * variable you are swizzling. This class is internal to Sh.
 *
 * Swizzles can be combined ("swizzle algebra") using the
 * operator*(). This class currently only supports host-time constant
 * swizzles, ie. you cannot use shader variables to specify swizzling
 * order.
 *
 * This class is also used for write masks, at least at the
 * intermediate level.
 *
 * Note that, at the moment, when combining swizzles indices are
 * checked to be sane, but original indices are not checked for
 * sanity, since currently swizzles don't know anything about (in
 * particular the size of) the tuple which they are swizzling.
 */
class
SH_DLLEXPORT
ShSwizzle {
public:
#ifdef SH_SWIZZLE_CHAR
    typedef char T;
#else 
    typedef int T;
#endif

  // Null swizzle
  ShSwizzle();
  /// Identity swizzle: does nothing at all.
  ShSwizzle(T srcSize);
  /// Use one element from the original tuple.
  ShSwizzle(T srcSize, T i0);
  /// Use two elements from the original tuple.
  ShSwizzle(T srcSize, T i0, T i1);
  /// Use three elements from the original tuple.
  ShSwizzle(T srcSize, T i0, T i1, T i2);
  /// Use four elements from the original tuple.
  ShSwizzle(T srcSize, T i0, T i1, T i2, T i3);
  /// Use an arbitrary number of elements from the original tuple.
  ShSwizzle(T srcSize, T size, T* indices);

#ifdef SH_SWIZZLE_CHAR
  /// Use an arbitrary number of elements from the original tuple.
  /// This is defined in addition to the char version in case
  /// you happen to have data in an integer array.
  ShSwizzle(T srcSize, T size, int* indices);
#endif

  ShSwizzle(const ShSwizzle& other);
  ~ShSwizzle();

  ShSwizzle& operator=(const ShSwizzle& other);
  
  /// Combine a swizzle with this one, as if it occured after this
  /// swizzle occured.
  ShSwizzle& operator*=(const ShSwizzle& other);

  /// Combine two swizzles with left-to-right precedence.
  ShSwizzle operator*(const ShSwizzle& other) const;

  /// Determine how many elements this swizzle results in.
  T size() const { return m_size; }
  /// Obtain the index of the \a i'th element. 0 <= i < size().
  T operator[](T i) const;

  /// Determine whether this is an identity swizzle.
  bool identity() const;

  /// Determine whether two swizzles are identical
  bool operator==(const ShSwizzle& other) const;
  
private:

  // deletes indices and throws an exception if index < 0 or index >= m_srcSize
  void checkSrcSize(T index); 

  // deletes indices and throws an exception if index < 0 or index >= m_size 
  void checkSize(T index); 

  // deletes and reallocates m_indices if newsize doesn't match m_size 
  void realloc(T newsize); 

  // allocates the m_indices array to current m_size;
  void  alloc(); 

  // deallocates the m_indices array;
  void dealloc();

  T get(T index) const;
  T& get(T index);
  T* getptr();
  const T* getptr() const;

  // Declare these two first so alignment problems don't make the ShSwizzle struct larger
  T m_srcSize;
  T m_size;

#ifdef SH_SWIZZLE_CHAR
  union {
    char local[4]; 
    char* ptr;
  } m_indices; // if m_size <= 4, no need to dynamically allocate array.  
#else
  T *m_indices;
#endif

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const ShSwizzle& swizzle);
};

/// Thrown when an invalid swizzle is specified (e.g. an index in the
/// swizzle is out of range).
class
SH_DLLEXPORT ShSwizzleException : public ShException 
{
public:
  ShSwizzleException(const ShSwizzle& s, int idx, int size);
};
  
}

#include "ShSwizzleImpl.hpp"
  
#endif
