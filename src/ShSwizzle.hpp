#ifndef SHSWIZZLE_HPP
#define SHSWIZZLE_HPP

#include <iosfwd>
#include <vector>
#include "ShException.hpp"

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
class ShSwizzle {
public:
  /// Identity swizzle: does nothing at all.
  ShSwizzle(int srcSize);
  /// Use one element from the original tuple.
  ShSwizzle(int srcSize, int i0);
  /// Use two elements from the original tuple.
  ShSwizzle(int srcSize, int i0, int i1);
  /// Use three elements from the original tuple.
  ShSwizzle(int srcSize, int i0, int i1, int i2);
  /// Use four elements from the original tuple.
  ShSwizzle(int srcSize, int i0, int i1, int i2, int i3);
  /// Use an arbitrary number of elements from the original tuple.
  ShSwizzle(int srcSize, int size, int* indices);

  ShSwizzle(const ShSwizzle& other);
  ~ShSwizzle();

  ShSwizzle& operator=(const ShSwizzle& other);
  
  /// Combine a swizzle with this one, as if it occured after this
  /// swizzle occured.
  ShSwizzle& operator*=(const ShSwizzle& other);

  /// Combine two swizzles with left-to-right precedence.
  ShSwizzle operator*(const ShSwizzle& other) const;

  /// Determine how many elements this swizzle results in.
  int size() const;
  /// Obtain the index of the \a i'th element. 0 <= i < size().
  int operator[](int i) const;
  
private:
  std::vector<int> m_indices;

  friend std::ostream& operator<<(std::ostream& out, const ShSwizzle& swizzle);
};

/// Thrown when an invalid swizzle is specified (e.g. an index in the
/// swizzle is out of range).
class ShSwizzleException : public ShException 
{
public:
  ShSwizzleException(int idx, int size);
};
  
}
  
#endif
