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
#ifndef SHMANIPULATOR_HPP
#define SHMANIPULATOR_HPP

#include "Program.hpp"
#include <vector>
#include <string>
#include <sstream>

namespace SH {

/** \brief A type trait class that determines storage type used for T in 
 * a Manipulator.  There must be an automatic conversion from T
 * to StorageType or an applicable copy constructor. The storage type also 
 * must be responsible for its own memory management as the code often
 * uses shallow copies of ranges/range vectors.
 *
 * The only function that needs to be added after adding a new storage type
 * is the OffsetRange::absIndex function in Manipulator.cpp.
 * This converts the storage type index into an absolute integer index.
 */
template<typename T> 
struct storage_trait {
  typedef T StorageType;
};

template<>
struct storage_trait<const char*> {
  typedef std::string StorageType; 
};

enum OffsetRangeErrors {
  OFFSET_RANGE_BAD_OFFSET = -1,
  OFFSET_RANGE_BAD_INDEX = -2
};

template<typename T>
class OffsetRange {
  public:
    OffsetRange(); 
    OffsetRange( T start, T end );
    OffsetRange( T start, int startOffset, T end, int endOffset );

    /** \brief returns an absolute position for start/end
     * where in the given sequence, the elements are assigned positions [0, vsize]
     *
     * returns BAD_OFFSET/BAD_INDEX if no corresponding index is found
     * The offset is only for internal use (for special indices like first/last)
     * If the use provided index is invalid, then there should be an error message.
     */
    int absStartIndex( const ProgramNode::VarIt& V, size_t vsize) const; 
    int absStartIndex( const ProgramNode::VarList& vlist ) const { return absStartIndex(vlist.begin(), vlist.size()); }
    int absEndIndex( const ProgramNode::VarIt& V, size_t vsize) const; 
    int absEndIndex( const ProgramNode::VarList& vlist ) const { return absEndIndex(vlist.begin(), vlist.size()); }

    std::string toString() const; 

  private:
    T start, end;
    int startOffset, endOffset;
    int absIndex( T index, int offset, const ProgramNode::VarIt& V, size_t vsize) const;
};


/** \brief A Manipulator is a Program output manipulator. 
 * This kind of manipulator permutes the outputs of 
 * a Program based on given integer indices. 
 *
 * Currently, two types are supported - T = int and T = char*.
 * T = int references channels by position (negative numbers are position from end)
 * T = char* references channels by name.
 * (The Manipulator<char*> operator()(??) methods store copies of the string) 
 * (negative indices -k mean k^th channel from the end, with -1
 * being the last output channel) 
 */
template<typename T>
class Manipulator {
  public:
    typedef typename storage_trait<T>::StorageType StorageType;
    typedef OffsetRange<StorageType> IndexRange;
    typedef std::vector<IndexRange> IndexRangeVector;

    /** \brief Creates empty manipulator of given size
     */
    Manipulator();
    ~Manipulator();

    Manipulator<T>& operator()(T i);
    Manipulator<T>& operator()(T start, T end);
    Manipulator<T>& operator()(const IndexRange &range); 

    // converts ranges to a sequence of integer ranges using given var list 
    IndexRangeVector getRanges() const; 

    // converts to string for debugging/error messages
    std::string toString() const;
    
  protected:
    IndexRangeVector m_ranges; 

    // converts indices to positive integer indices. 
    // If it cannot be found, raises AlgebraException.
    // if the index has an offset that makes it invalid, then valid is set to false
    //OffsetRange<int> convertRange(IndexRange range, ProgramNode::VarIt& V, size_t vsize) const;
};

/** \brief Applies a manipulator to inputs of a Program
 *
 * The permutation ranges are more restrictive than output manipulation
 * since inputs cannot be repeated, and should not be discarded.
 *
 * This means that ranges in the manipulator must not overlap, and any inputs not 
 * in a range are given a default value of 0. 
 */
template<typename T>
Program operator<<(const Program &p, const Manipulator<T> &m); 

/** \brief Applies a manipulator to the outputs of a Program
 *
 * This makes sense since >> is left associative, so 
 *    p >> m >> q
 * looks like manipulating p's output channels to use as q's inputs.
 */
template<typename T>
Program operator<<(const Manipulator<T> &m, const Program &p);


/// permute(a1, ...) is a manipulator that permutes 
// shader outputs based on given indices
//
/** \brief creates a permutation manipulator which
 * gives outputSize outputs when applied to a Program
 *
 * Empty permutes are not allowed because the compiler would not
 * be able to resolve ambiguity. 
 *
 * if an index >= 0, then uses index'th output
 * if index < 0, then uses program size + index'th output
 */
template<typename T>
Manipulator<T> swizzle(T i0);

template<typename T>
Manipulator<T> swizzle(T i0, T i1);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4, T i5);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8);

template<typename T>
Manipulator<T> swizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8, T i9);

template<typename T>
Manipulator<T> swizzle(std::vector<T> indices);

/// range manipulator that permutes ranges of shader
// outputs based on given indices
template<typename T>
Manipulator<T> range(T i);

template<typename T>
Manipulator<T> range(T start, T end);

/** extract is a manipulator that removes the kth output
 * and appends it before all other outputs. 
 *
 * int version:
 * if k >= 0, then take element k (indices start at 0) 
 * if k < 0, take element outputs.size + k
 *
 * string version:
 * extracts given name to beginning
 */
template<typename T>
Manipulator<T> extract(T k); 

/** insert is a manipulator that does the opposite of extract.
 * It moves the first output to the kth output and shifts
 * the rest of the outputs accordingly.
 *
 * int version:
 * if k >= 0, then move to element k (indices start at 0) 
 * if k < 0, move to element outputs.size + k
 *
 * string version:
 * inserts first output to the position of the given name 
 */
template<typename T>
Manipulator<T> insert(T k); 

/** drop is a manipulator that discards the k outputs.
 *
 * int version:
 * discards k'th output
 *
 * string version:
 * drops given name to beginning
 */
template<typename T>
Manipulator<T> drop(T k);

typedef Manipulator<int> PositionManipulator;
typedef Manipulator<char*> NameManipulator;

}

#include "ManipulatorImpl.hpp"

#endif
