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
#include "ShCastManager.hpp"
#include "ShVariantCast.hpp"

namespace {
using namespace SH;

template<ShValueType Dest, ShDataType DestDT, ShValueType Src, ShDataType SrcDT>
void addCast(bool automatic)
{
  ShCastManager::instance()->addCast(ShDataVariantCast<Dest, DestDT, Src, SrcDT>::instance(), automatic);
}
// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction 
template<ShValueType Dest, ShValueType Src>
void addPromotion()
{
  addCast<Dest, SH_HOST, Src, SH_HOST>(true);
  addCast<Src, SH_HOST, Dest, SH_HOST>(false);
}

// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction
template<ShValueType V>
void addMemoryCast()
{
  addCast<V, SH_HOST, V, SH_MEM>(false);
  addCast<V, SH_MEM, V, SH_HOST>(false);
}

}

namespace SH {

// This adds the available automatic and explicit casts 
void ShTypeInfo::addCasts()
{
  
  // automatic promotion DAG edges 
  // the inverse edges are added as automatic conversions, but not promotions
  // i_d <- i_f, d 
  // i_f <- f
  // d <- f
  // f <- h, i, ui, fi, fs, fb, fui, fus, fub
  // i <- s, b, us, ub
  //
  // This should give a connected graph with maximum diameter (longest shortest
  // path between all pairs) of 3 (i.e. convering fi to b goes fi -> f -> i ->
  // b)
  //
  // We can add in a few more automatic conversions to turn this down to 2
  // (by making f the direct "supertype" of everything fractional or int)
  
  addPromotion<SH_INTERVAL_DOUBLE, SH_INTERVAL_FLOAT>();
  addCast<SH_INTERVAL_DOUBLE, SH_HOST, SH_DOUBLE, SH_HOST>(true);

  addCast<SH_INTERVAL_FLOAT, SH_HOST, SH_FLOAT, SH_HOST>(true);

  addPromotion<SH_DOUBLE, SH_FLOAT>();

  addPromotion<SH_FLOAT, SH_HALF>();
  addPromotion<SH_FLOAT, SH_INT>();
  addPromotion<SH_FLOAT, SH_UINT>();
  addPromotion<SH_FLOAT, SH_FRAC_INT>();
  addPromotion<SH_FLOAT, SH_FRAC_SHORT>();
  addPromotion<SH_FLOAT, SH_FRAC_BYTE>();
  addPromotion<SH_FLOAT, SH_FRAC_UINT>();
  addPromotion<SH_FLOAT, SH_FRAC_USHORT>();
  addPromotion<SH_FLOAT, SH_FRAC_UBYTE>();

  addPromotion<SH_INT, SH_SHORT>();
  addPromotion<SH_INT, SH_BYTE>();
  addPromotion<SH_INT, SH_USHORT>();
  addPromotion<SH_INT, SH_UBYTE>();

  // these are the extra conversions to make the diameter = 2
  addCast<SH_FLOAT, SH_HOST, SH_SHORT, SH_HOST>(false);
  addCast<SH_FLOAT, SH_HOST, SH_BYTE, SH_HOST>(false);
  addCast<SH_FLOAT, SH_HOST, SH_USHORT, SH_HOST>(false);
  addCast<SH_FLOAT, SH_HOST, SH_UBYTE, SH_HOST>(false);

  // these are the memory->host, host->memory casts
  addMemoryCast<SH_INTERVAL_DOUBLE>();
  addMemoryCast<SH_INTERVAL_FLOAT>();

  addMemoryCast<SH_DOUBLE>();
  addMemoryCast<SH_FLOAT>();
  addMemoryCast<SH_HALF>();

  addMemoryCast<SH_INT>();
  addMemoryCast<SH_SHORT>();
  addMemoryCast<SH_BYTE>();
  addMemoryCast<SH_UINT>();
  addMemoryCast<SH_USHORT>();
  addMemoryCast<SH_UBYTE>();

  addMemoryCast<SH_FRAC_INT>();
  addMemoryCast<SH_FRAC_SHORT>();
  addMemoryCast<SH_FRAC_BYTE>();
  addMemoryCast<SH_FRAC_UINT>();
  addMemoryCast<SH_FRAC_USHORT>();
  addMemoryCast<SH_FRAC_UBYTE>();

  ShCastManager::instance()->init();

  /* DEBUG */ //ShCastManager::instance()->graphvizDump(std::cout);
}

}
