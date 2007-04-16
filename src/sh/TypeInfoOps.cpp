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
  /* not all TxTx... -> ops are implemented.  The way things work is similar to
   * C++ in spirit 
   * 
   * @see StorageTypeImpl.hpp TypeInfoCasts.cpp
   *
   * Rules (checked in order until one matches)
   * 1) If either is affine, and either has base type double, use affine double
   * 2) If either is affine, use affine float
   * 3) If either is interval, and either has base type double, use interval
   * double
   * 4) If either is interval, use interval float
   * 5) If either is double, use double
   * 6) If either is float or fractional, use float
   * 7) If both are half, use half
   * 8) Use int 
   */
  _initFloatOps<Affine<double> >();
  _initAffineOps<double, Affine<double> >();

  _initFloatOps<Affine<float> >();
  _initAffineOps<float, Affine<float> >();

  _initFloatOps<Interval<double> >();
  _initIntervalOps<double, Interval<double> >();

  _initFloatOps<Interval<float> >();
  _initIntervalOps<float, Interval<float> >();

  _initFloatOps<double>();
  _initFloatOps<float>();

  _initIntOps<int>();
}

}
