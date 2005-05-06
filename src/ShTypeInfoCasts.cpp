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
// and a type conversion edge in the opposite direction of reverse = true 
template<typename Dest, typename Src>
void addPromotion()
{
  addCast<Dest, SH_HOST, Src, SH_HOST>(true);
  addCast<Src, SH_HOST, Dest, SH_HOST>(false);
}

template<typename Dest, typename Src>
void addPromotionOnly()
{
  addCast<Dest, SH_HOST, Src, SH_HOST>(true);
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
  addPromotion<ShAffine<double>, ShAffine<float> >();

  // @todo range do we really want to allow conversions from affine -> interval?
  addPromotion<ShAffine<double>, ShInterval<double> >(); 

  addPromotion<ShAffine<float>, ShInterval<float> >();

  
  addPromotion<ShInterval<double>, ShInterval<float> >();
  addPromotionOnly<ShInterval<double>, double>();

  addPromotionOnly<ShInterval<float>, float>();

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

  // these are the memory->SH_HOST, SH_HOST->memory casts
  addMemoryCast<ShAffine<double> >();
  addMemoryCast<ShAffine<float> >();

  addMemoryCast<ShInterval<double> >();
  addMemoryCast<ShInterval<float> >();

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
