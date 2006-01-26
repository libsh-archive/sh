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
#include <fstream>
#include "ShTypeInfo.hpp"
#include "ShCastManager.hpp"
#include "ShVariantCast.hpp"

namespace {
using namespace SH;

template<typename Dest, ShDataType DestDT, typename Src, ShDataType SrcDT>
void addCast(bool automatic)
{
  ShCastManager::instance()->addCast(ShDataVariantCast<Dest, DestDT, Src, SrcDT>::instance(), automatic);
}
// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction 
template<typename Dest, typename Src>
void addPromotion()
{
  addCast<Dest, SH_HOST, Src, SH_HOST>(true);
  addCast<Src, SH_HOST, Dest, SH_HOST>(false);
}

// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction
template<typename T>
void addMemoryCast()
{
  addCast<T, SH_HOST, T, SH_MEM>(false);
  addCast<T, SH_MEM, T, SH_HOST>(false);
}

}

namespace SH {

// This adds the available automatic and explicit casts 
void ShTypeInfo::addCasts()
{
  
  // automatic promotion DAG edges 
  // the inverse edges are added as automatic conversions, but not promotions
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
  
  addPromotion<double, float>();

  addPromotion<float, ShHalf>();
  addPromotion<float, int>();
  addPromotion<float, unsigned int>();
  addPromotion<float, ShFracInt> ();
  addPromotion<float, ShFracShort> ();
  addPromotion<float, ShFracByte> ();
  addPromotion<float, ShFracUInt> ();
  addPromotion<float, ShFracUShort> ();
  addPromotion<float, ShFracUByte> ();

  addPromotion<int, short>();
  addPromotion<int, char>();
  addPromotion<int, unsigned short>();
  addPromotion<int, unsigned char>();

  // these are the extra conversions to make the diameter = 2
  addCast<float, SH_HOST, short, SH_HOST>(false);
  addCast<float, SH_HOST, char, SH_HOST>(false);
  addCast<float, SH_HOST, unsigned short, SH_HOST>(false);
  addCast<float, SH_HOST, unsigned char, SH_HOST>(false);

  addMemoryCast<double>();
  addMemoryCast<float>();
  addMemoryCast<ShHalf>();

  addMemoryCast<int>();
  addMemoryCast<short>();
  addMemoryCast<char>();
  addMemoryCast<unsigned int>();
  addMemoryCast<unsigned short>();
  addMemoryCast<unsigned char>();

  addMemoryCast<ShFraction<int> >();
  addMemoryCast<ShFraction<short> >();
  addMemoryCast<ShFraction<char> >();
  addMemoryCast<ShFraction<unsigned int> >();
  addMemoryCast<ShFraction<unsigned short> >();
  addMemoryCast<ShFraction<unsigned char> >();

  ShCastManager::instance()->init();

#if 0
  std::ofstream fout("castgraph.dot");
  ShCastManager::instance()->graphvizDump(fout);
  system("dot -Tps < castgraph.dot > castgraph.ps");
#endif
}

}
