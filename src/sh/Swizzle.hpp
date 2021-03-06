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
#ifndef SHSWIZZLE_HPP
#define SHSWIZZLE_HPP

#include <iosfwd>
//#include <vector>
#include "DllExport.hpp"
#include "Exception.hpp"

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
Swizzle {
public:

  // Null swizzle
  Swizzle();

  /// Identity swizzle: does nothing at all.
  Swizzle(int srcSize);
  /// Use one element from the original tuple.
  Swizzle(int srcSize, int i0);
  /// Use two elements from the original tuple.
  Swizzle(int srcSize, int i0, int i1);
  /// Use three elements from the original tuple.
  Swizzle(int srcSize, int i0, int i1, int i2);
  /// Use four elements from the original tuple.
  Swizzle(int srcSize, int i0, int i1, int i2, int i3);
  /// Use an arbitrary number of elements from the original tuple.
  Swizzle(int srcSize, int size, int* indices);

  Swizzle(const Swizzle& other);

  /* Construct swizzle from repeating other n times */ 
  Swizzle(const Swizzle& other, int n);
  ~Swizzle();

  Swizzle& operator=(const Swizzle& other);
  
  /// Combine a swizzle with this one, as if it occured after this
  /// swizzle occured.
  Swizzle& operator*=(const Swizzle& other);

  /// Combine two swizzles with left-to-right precedence.
  Swizzle operator*(const Swizzle& other) const;

  /// Determine how many elements this swizzle results in.
  int size() const { return m_size; }

  /// Obtain the index of the \a i'th element. 0 <= i < size().
  /// This is int so that printing out the result won't give something
  /// weird
  int operator[](int i) const;

  /// Determine whether this is an identity swizzle.
  bool identity() const;

  /// Determine whether two swizzles are identical
  bool operator==(const Swizzle& other) const;

  /// Determine whether a swizzle is "less" than another swizzle.
  /// This is only useful to provide strong ordering.
  bool operator<(const Swizzle& other) const;
  
private:
  // copies the other swizzle's elements 
  void copy(const Swizzle &other, bool islocal);

  // throws an exception if index < 0 or index >= m_srcSize
  void checkSrcSize(int index); 

  // allocates the m_indices array to current m_size
  // returns true 
  bool alloc(); 

  // deallocates the m_indices array 
  void dealloc();

  // returns whether we're using local 
  bool local() const;

  // returns the identity swiz value on this machine
  int idswiz() const;

  // Declare these two first so alignment problems don't make the Swizzle struct larger
  int m_srcSize;
  int m_size;

  // when srcSize <= 255 and size <= 4, use local.
  // local is always initialized to 0x03020101, so identity comparison is
  // just an integer comparison using intval
  union {
    unsigned char local[4];
    int intval;
    int* ptr;
  } m_index;

  friend SH_DLLEXPORT std::ostream& operator<<(std::ostream& out, const Swizzle& swizzle);
};

/// Thrown when an invalid swizzle is specified (e.g. an index in the
/// swizzle is out of range).
class
SH_DLLEXPORT SwizzleException : public Exception 
{
public:
  SwizzleException(const Swizzle& s, int idx, int size);
};
  
}

#include "SwizzleImpl.hpp"
  
#endif
