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
  /// not all TxTx... -> ops are implemented.  The way things work is similar to
  /// C++ in spirit (except for the way the i x ui case is handled.  C++
  /// has a special case there which uses ui, which to me sounds like a serious
  /// loss of information. I think prefering loss of precision by going to float 
  /// over losing sign information sounds better) 
  ///
  /// 1) if either operand is i_d or operands are d x i_f, use i_d
  /// 2) else if one operand is i_f, use i_f
  /// 3) else if either operand is d, use d 
  /// 4) else if either operand is f or we have i x ui, use f 
  /// 5) else if operands are integers/unsigned integers use i
  /// 6) else use float (this includes all fractionals)
  _shInitFloatOps<SH_INTERVAL_DOUBLE>();
  _shInitIntervalOps<SH_DOUBLE, SH_INTERVAL_DOUBLE>();

  _shInitFloatOps<SH_INTERVAL_FLOAT>();
  _shInitIntervalOps<SH_FLOAT, SH_INTERVAL_FLOAT>();

  _shInitFloatOps<SH_DOUBLE>();
  _shInitFloatOps<SH_FLOAT>();

  _shInitIntOps<SH_INT>();
}

}
