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
#ifndef SHDATATYPEIMPL_HPP
#define SHDATATYPEIMPL_HPP

#include <climits>
#include <cmath>
#include <algorithm>
#include "ShDataType.hpp"

namespace SH {

/** Returns the boolean cond in the requested data type */
template<typename T, ShDataType DT>
inline
typename ShDataTypeCppType<T, DT>::type shDataTypeCond(bool cond) 
{
  return cond ? ShDataTypeConstant<T, DT>::One : ShDataTypeConstant<T, DT>::Zero;
}

/** Returns a whether the two values are exactly the same.
 * This is is useful for the range types.
 * @{
 */
template<typename T>
inline
bool shDataTypeEqual(const T &a, 
                     const T &b) 
{
  return a == b;
}

// @}

/** Returns whether the value is always greater than zero (i.e. true) 
 * @{
 */
template<typename T>
inline
bool shDataTypeIsPositive(const T &a)
{
  return a > 0; 
}

//@}

/** Casts one data type to another data type 
 * All the built-in types can use C++ casts
 * for all the casts required by Sh internally.
 */
template<typename T1, ShDataType DT1, typename T2, ShDataType DT2>
void shDataTypeCast(typename ShDataTypeCppType<T1, DT1>::type &dest,
                    const typename ShDataTypeCppType<T2, DT2>::type &src)
{
  typedef typename ShDataTypeCppType<T1, DT1>::type desttype; 
  dest = static_cast<desttype>(src);
}


}

#endif
