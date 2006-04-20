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
#include "TypeInfo.hpp"
#include "Eval.hpp"

// @file TypeInfoOps.cpp
// Implements the addOps method in TypeInfo.  This is separated out into 
// a different file so that distributed compilers can split up  
// TypeInfo. 
//
// @see TypeInfo
namespace SH {

void TypeInfo::addOps()
{
  /// not all TxTx... -> ops are implemented.  The way things work is similar to
  /// C++ in spirit (except for the way the i x ui case is handled.  C++
  /// has a special case there which uses ui, which to me sounds like a serious
  /// loss of information. I think prefering loss of precision by going to float 
  /// over losing sign information sounds better) 
  ///
  /// 1) else if either operand is d, use d 
  /// 2) else if either operand is f, h, or fractional, use f
  /// 3) otherwise, use int
  _initFloatOps<double>();
  _initFloatOps<float>();

  _initIntOps<int>();
}

}
