// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShTypeInfo.hpp"
#include "ShEval.hpp"

// @file ShTypeInfoOps.cpp
// Implements the addOps method in ShTypeInfo.  This is separated out into 
// a different file so that distributed compilers can split up  
// ShTypeInfo. 
//
// @see ShTypeInfo
namespace SH {

void ShTypeInfo::addOps()
{
  /* not all TxTx... -> ops are implemented.  The way things work is similar to
   * C++ in spirit 
   * 
   * @see ShStorageTypeImpl.hpp ShTypeInfoCasts.cpp
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
  _shInitFloatOps<ShAffine<double> >();
  _shInitAffineOps<double, ShAffine<double> >();

  _shInitFloatOps<ShAffine<float> >();
  _shInitAffineOps<float, ShAffine<float> >();

  _shInitFloatOps<ShInterval<double> >();
  _shInitIntervalOps<double, ShInterval<double> >();

  _shInitFloatOps<ShInterval<float> >();
  _shInitIntervalOps<float, ShInterval<float> >();

  _shInitFloatOps<double>();
  _shInitFloatOps<float>();
  // @todo range - this should be okay for now
  // Because of ShCommonType, library functions will still use half temporaries when recorded in shaders,
  // but in immediate mode halfs will be converted to float for computation.
  //_shInitFloatOps<ShHalf>();

  _shInitIntOps<int>();

#if 0
  std::cout << "Available Ops: " << std::endl;
  std::cout << ShEval::instance()->availableOps();
#endif
}

}
