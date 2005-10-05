// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHMANIPULATORIMPL_HPP
#define SHMANIPULATORIMPL_HPP

#include <cstdarg>
#include <cassert>
#include <sstream>
#include "ShVariableNode.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShAlgebra.hpp"
#include "ShManipulator.hpp"
#include "ShInstructions.hpp"

namespace SH {

// TODO figure out a better way...
#define _FIRST -54545454
#define _SECOND -54545453
#define _LAST 54545454

template<typename T>
OffsetRange<T>::OffsetRange() {}

template<typename T>
OffsetRange<T>::OffsetRange( T start, T end )
  : start( start ), end( end ), startOffset( 0 ), endOffset( 0 ) {}

template<typename T>
OffsetRange<T>::OffsetRange( T start, int startOffset, T end, int endOffset )
  : start( start ), end( end ), startOffset( startOffset ), endOffset( endOffset ) {}

template<typename T>
int OffsetRange<T>::absStartIndex( const ShProgramNode::VarList vars ) const {
  return absIndex( start, startOffset, vars );
}

template<typename T>
int OffsetRange<T>::absEndIndex( const ShProgramNode::VarList vars ) const {
  return absIndex( end, endOffset, vars );
}

template<typename T>
std::string OffsetRange<T>::toString() const {
  std::ostringstream os;
  os << "(" << start << (startOffset >= 0 ? "+" : "" ) << startOffset
     << ", " << end << (endOffset >= 0 ? "+" : "" ) << endOffset << ")";
  return os.str();
}

template<typename T>
ShManipulator<T>::ShManipulator() {} 

template<typename T>
ShManipulator<T>::~ShManipulator() {
} 

template<typename T>
ShManipulator<T>& ShManipulator<T>::operator()(T i) {
  m_ranges.push_back(IndexRange(i, i));
  return *this;
}

template<typename T>
ShManipulator<T>& ShManipulator<T>::operator()(T start, T end) {
  m_ranges.push_back(IndexRange(start, end));
  return *this;
}

template<typename T>
ShManipulator<T>& ShManipulator<T>::operator()(const IndexRange &range) {
  m_ranges.push_back(range);
  return *this;
}

template<typename T>
typename ShManipulator<T>::IndexRangeVector ShManipulator<T>::getRanges() const { 
  return m_ranges;
}

template<typename T>
std::string ShManipulator<T>::toString() const {
  std::ostringstream os;
  for(typename IndexRangeVector::const_iterator it = m_ranges.begin(); it != m_ranges.end(); ++it) {
    if(it != m_ranges.begin()) os << ", ";
    os << it->toString();
  }
  return os.str();
}

/* input permutation */
template<typename T>
ShProgram operator<<(const ShProgram &p, const ShManipulator<T> &m) {
  typedef typename ShManipulator<T>::IndexRangeVector RangeVec;
  RangeVec mranges = m.getRanges();
  int i;

  ShProgram permuter = SH_BEGIN_PROGRAM() {
    /* Make shader outputs from p's inputs 
     * default output value is zero, so for those that have
     * no matching inputs, they become zero outputs */
    std::vector<ShVariable> outputs;
    for(ShProgramNode::VarList::const_iterator inIt = p.node()->inputs.begin();
        inIt != p.node()->inputs.end(); ++inIt) {
      ShVariable out((*inIt)->clone(SH_OUTPUT));
      outputs.push_back(out);
    }

    std::vector<ShVariable>::size_type size = outputs.size();

    /* Make shader outputs from permuted ranges of inputs */
    std::vector<bool> used(size, false); //mark used inputs
    for(typename RangeVec::const_iterator irvIt = mranges.begin();
        irvIt != mranges.end(); ++irvIt) {
      int start = irvIt->absStartIndex( p.node()->inputs );
      int end = irvIt->absEndIndex( p.node()->inputs ); 

      if(start == OFFSET_RANGE_BAD_OFFSET || end == OFFSET_RANGE_BAD_OFFSET ) continue;
      if(start == OFFSET_RANGE_BAD_INDEX || end == OFFSET_RANGE_BAD_INDEX ) {
        std::ostringstream os;
        os << "Invalid ShManipulator Range " << irvIt->toString() 
          << " for an ShProgram with output size " << static_cast<unsigned>(size);
        shError(ShAlgebraException(os.str())); 
      }

      for(i = start; i <= end; ++i) {
        if(used[i]) {
          std::ostringstream os;
          os << "Duplicate index " << i << " in range " << irvIt->toString() 
            << " not allowed for input manipulators"; 
          shError(ShAlgebraException(os.str()));
        }
        used[i] = true;

        ShVariable input(outputs[i].node()->clone(SH_INPUT));
        shASN(outputs[i], input);
      }
    }
  } SH_END;

  return connect(permuter, p); 
}

template<typename T>
ShProgram operator<<(const ShManipulator<T> &m, const ShProgram &p) {
  typedef typename ShManipulator<T>::IndexRangeVector RangeVec;
  RangeVec mranges = m.getRanges();

  ShProgram permuter = SH_BEGIN_PROGRAM() {
    /* Make shader inputs from p's outputs */
    std::vector<ShVariable> inputs;
    for(ShProgramNode::VarList::const_iterator outIt = p.node()->outputs.begin();
        outIt != p.node()->outputs.end(); ++outIt) {
      ShVariable in((*outIt)->clone(SH_INPUT));
      inputs.push_back(in);
    }

    std::vector<ShVariable>::size_type size = inputs.size();

    /* Make shader outputs from permuted ranges of inputs */
    for(typename RangeVec::const_iterator irvIt = mranges.begin();
        irvIt != mranges.end(); ++irvIt) {
      int start = irvIt->absStartIndex( p.node()->outputs );
      int end = irvIt->absEndIndex( p.node()->outputs ); 

      if(start == OFFSET_RANGE_BAD_OFFSET || end == OFFSET_RANGE_BAD_OFFSET ) continue;
      if(start == OFFSET_RANGE_BAD_INDEX || end == OFFSET_RANGE_BAD_INDEX ) {
        std::ostringstream os;
        os << "Invalid ShManipulator Range " << irvIt->toString() << 
          " for an ShProgram with output size " << static_cast<unsigned>(size);
        shError(ShAlgebraException(os.str())); 
      }

      for(int i = start; i <= end; ++i) { // handles end < start case
        ShVariable output(inputs[i].node()->clone(SH_OUTPUT));
        shASN(output, inputs[i]);
      }
    }
  } SH_END;

  return connect(p, permuter); 
}

template<typename T>
ShManipulator<T> shSwizzle(T i0) {
  ShManipulator<T> m;
  m(i0);
  return m;
}
template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1) {
  ShManipulator<T> m;
  m(i0); m(i1);
  return m;
}
template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); 
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3);
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); 
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4, T i5) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); m(i5); 
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); m(i5); m(i6); 
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); m(i5); m(i6); m(i7); 
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); m(i5); m(i6); 
  m(i7); m(i8);
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(T i0, T i1, T i2, T i3, T i4, T i5, T i6, T i7, T i8, T i9) {
  ShManipulator<T> m;
  m(i0); m(i1); m(i2); m(i3); m(i4); m(i5); m(i6); 
  m(i7); m(i8); m(i9);
  return m;
}

template<typename T>
ShManipulator<T> shSwizzle(std::vector<T> indices) {
  ShManipulator<T> m;
  for(typename std::vector<T>::iterator it = indices.begin();
      it != indices.end(); ++it) {
    m(*it);
  }
  return m;
}

template<typename T>
ShManipulator<T> shRange(T i) {
  ShManipulator<T> m;
  m(i);
  return m;
}

template<typename T>
ShManipulator<T> shRange(T start, T end) {
  ShManipulator<T> m;
  m(start, end);
  return m;
}

template<typename T>
ShManipulator<T> shExtract(T k) {
  ShManipulator<T> m;
  m(k);
  typedef typename ShManipulator<T>::IndexRange Range;
  m(Range(k,_FIRST,k,-1));
  m(Range(k,1,k,_LAST));
  return m;
}

template<typename T>
ShManipulator<T> shInsert(T k) {
  ShManipulator<T> m;
  typedef typename ShManipulator<T>::IndexRange Range;
  
  m(Range(k,_SECOND,k,0));
  m(Range(k,_FIRST,k,_FIRST));
  m(Range(k,1,k,_LAST));
  std::cout << m.toString() << std::endl;
  return m;
}

template<typename T>
ShManipulator<T> shDrop(T k) {
  ShManipulator<T> m;
  typedef typename ShManipulator<T>::IndexRange Range; 
  m(Range(k,_FIRST,k,-1));
  m(Range(k,1,k,_LAST));
  return m;
}

}

#endif
