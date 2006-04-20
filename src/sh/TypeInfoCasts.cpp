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
#include <fstream>
#include "TypeInfo.hpp"
#include "CastManager.hpp"
#include "VariantCast.hpp"

namespace {
using namespace SH;

template<typename Dest, DataType DestDT, typename Src, DataType SrcDT>
void addCast(bool automatic)
{
  CastManager::instance()->addCast(DataVariantCast<Dest, DestDT, Src, SrcDT>::instance(), automatic);
}
// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction 
template<typename Dest, typename Src>
void addPromotion()
{
  addCast<Dest, HOST, Src, HOST>(true);
  addCast<Src, HOST, Dest, HOST>(false);
}

// adds automatic promotion from src to dest
// and a type conversion edge in the opposite direction
template<typename T>
void addMemoryCast()
{
  addCast<T, HOST, T, MEM>(false);
  addCast<T, MEM, T, HOST>(false);
}

}

namespace SH {

// This adds the available automatic and explicit casts 
void TypeInfo::addCasts()
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

  addPromotion<float, Half>();
  addPromotion<float, int>();
  addPromotion<float, unsigned int>();
  addPromotion<float, FracInt> ();
  addPromotion<float, FracShort> ();
  addPromotion<float, FracByte> ();
  addPromotion<float, FracUInt> ();
  addPromotion<float, FracUShort> ();
  addPromotion<float, FracUByte> ();

  addPromotion<int, short>();
  addPromotion<int, char>();
  addPromotion<int, unsigned short>();
  addPromotion<int, unsigned char>();

  // these are the extra conversions to make the diameter = 2
  addCast<float, HOST, short, HOST>(false);
  addCast<float, HOST, char, HOST>(false);
  addCast<float, HOST, unsigned short, HOST>(false);
  addCast<float, HOST, unsigned char, HOST>(false);

  addMemoryCast<double>();
  addMemoryCast<float>();
  addMemoryCast<Half>();

  addMemoryCast<int>();
  addMemoryCast<short>();
  addMemoryCast<char>();
  addMemoryCast<unsigned int>();
  addMemoryCast<unsigned short>();
  addMemoryCast<unsigned char>();

  addMemoryCast<Fraction<int> >();
  addMemoryCast<Fraction<short> >();
  addMemoryCast<Fraction<char> >();
  addMemoryCast<Fraction<unsigned int> >();
  addMemoryCast<Fraction<unsigned short> >();
  addMemoryCast<Fraction<unsigned char> >();

  CastManager::instance()->init();

#if 0
  std::ofstream fout("castgraph.dot");
  CastManager::instance()->graphvizDump(fout);
  system("dot -Tps < castgraph.dot > castgraph.ps");
#endif
}

}
