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
#ifndef SH_MULTIARRAY_HPP
#define SH_MULTIARRAY_HPP

#include <valarray>
#include <iostream>
#include "ShDebug.hpp"

namespace SH {

// @todo range -- this is a mess partly because valarrays do some things
// well, and others not so well.  The other reason is I wrote a mess. 
//
// Clean this up (go write or include a real multidimensional array class)

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::valarray<T>& v)
{
  out << "(";
  for(size_t i = 0; i < v.size(); ++i) {
    if(i != 0) out << ", ";
    out << v[i];
  }
  out << ")";
  return out;
}

typedef std::valarray<size_t> size_t_array;
typedef std::valarray<float> float_array;

template<typename T>
T product(const std::valarray<T>& w) {
  T result = w[0];
  for(size_t i = 1; i < w.size(); result *= w[i], ++i);
  return result;
}

/* Represents a sequence of multidimensional indices 
 * (slices in each dimension at the given stride) */
struct MultiArrayIndex {
  MultiArrayIndex(const size_t_array& start, const size_t_array& lengths, 
      const size_t_array& strides) 
    : m_dims(start.size()), m_start(start), 
      m_cur(static_cast<size_t>(0), start.size()), 
      m_lengths(lengths), 
      m_strides(strides)
  {
    SH_DEBUG_ASSERT(m_lengths.size() == m_dims && m_strides.size() == m_dims);
  }

  // prefix ++ operator
  MultiArrayIndex& operator++()
  {
    for(int i = m_dims - 1; i >= 0; --i) {
      if(m_cur[i] == m_lengths[i] - 1) {
        m_cur[i] = 0;
      } else {
        m_cur[i]++; 
        break;
      }
    }
    return *this;
  }

  // tests whether this has reached the end
  bool end() 
  {
    std::valarray<bool> done = (m_cur == m_lengths - static_cast<size_t>(1));
    return product(done);
  }

  size_t_array index() const {
    return m_start + m_cur * m_strides; 
  }

  size_t length() {
    return product(m_lengths);
  }

  friend std::ostream& operator<<(std::ostream& out, const MultiArrayIndex& idx)
  {
    out << idx.index() << " cur=" << idx.m_cur;
    return out;
  }

 private:
  size_t m_dims;
  size_t_array m_start, m_cur; 
  size_t_array m_lengths, m_strides;
};


// makes a copy of a multidimensional data array stored in a 1d array and 
// allows convenient indexing. 
//
// The higher the index in widths, the faster it increments in the 1D array
// (i.e. consecutive widths[widths.size()-1] are consecutive in the 1D array) 
// This may seem backwards for typical 1D packing of x,y,z... 
// but matches the way std::gslice_array works.
template<typename T>
struct MultiArray {
  MultiArray() {}

  MultiArray(T* data, size_t_array widths) 
    : m_dims(widths.size()), 
      m_size(product(widths)),
      m_data(data, m_size),
      m_widths(widths)
  {
    init();
  }

  MultiArray(size_t_array widths)
    : m_dims(widths.size()), 
      m_size(product(widths)),
      m_data(product(widths)),
      m_widths(widths)
  {
    init();
  }

  void init() {
    m_strides.resize(m_dims);
    m_strides[m_dims - 1] = 1;
    for(int i = m_dims - 1; i > 0;  --i) {
      m_strides[i - 1] = m_strides[i] * m_widths[i];
    }
  }

  T& operator[](int idx) {
    return m_data[idx];
  }

  /* Conversions between nD <-> 1D indices 
   * @{ */
  int indexOf(size_t_array idx) {
    SH_DEBUG_ASSERT(idx.size() == m_dims);
    return (idx * m_strides).sum();
  }

  size_t size() const {
    return m_size;
  }

  size_t_array indexOf(int idx) {
    size_t_array result(m_dims);
    for(int i = m_dims - 1; i >= 0; --i) {
      result[i] = idx % m_widths[i];
      idx /= m_widths[i];
    }
    return result;
  }
  /* @} */

  T& operator[](size_t_array idx) {
    return m_data[indexOf(idx)];
  }

  MultiArray& operator=(const MultiArray& other)
  {
    /* vararrays have undefined behaviour on assignment with a different sized
     * vararray */ 
    m_data.resize(other.m_data.size());
    m_widths.resize(other.m_widths.size());
    m_strides.resize(other.m_strides.size());

    m_dims = other.m_dims;
    m_size = other.m_size;
    m_data = other.m_data;
    m_widths = other.m_widths;
    m_strides = other.m_strides;

    return *this;
  }



  MultiArray& operator=(const T& value)
  {
    m_data = value;
    return *this;
  }

  /* copies out values to data, which must be large enough to hold them */ 
  void copyTo(T* data)
  {
    for(int i = 0; i < m_size; ++i) {
      data[i] = m_data[i];
    }
  }

  /* returns a copy of the data using the given ranges */
  std::valarray<T> rangeSlice(const size_t_array& start, const size_t_array& lengths)
  {
    SH_DEBUG_ASSERT(start.size() == m_dims && lengths.size() == m_dims);
    return m_data[std::gslice(indexOf(start), lengths, m_strides)];
  }

  /* assigns other to the range slice */ 
  void rangeSliceAsn(const size_t_array& start, const size_t_array& lengths, const std::valarray<T>& other)
  {
    SH_DEBUG_ASSERT(start.size() == m_dims && lengths.size() == m_dims);
    m_data[std::gslice(indexOf(start), lengths, m_strides)] = other;
  }

  size_t m_dims;
  int m_size; // total size 
  std::valarray<T> m_data;
  size_t_array m_widths;
  size_t_array m_strides;
};


template<typename T>
std::ostream& operator<<(std::ostream& out, const MultiArray<T>& a)
{
  out << "{ widths = " << a.m_widths;
  out << ", strides = " << a.m_strides;
  out << ", data = " << a.m_data << " }";
  return out;
}

}

#endif
