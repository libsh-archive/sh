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
#ifndef SHVARIANTCASTIMPL_HPP
#define SHVARIANTCASTIMPL_HPP

#include "ShVariant.hpp"
#include "ShVariantCast.hpp"

namespace SH {

template<typename DEST, typename SRC>
ShVariantPtr ShDataVariantCast<DEST, SRC>::operator()(ShVariantPtr value) const
{
  if (MatchType<DEST, SRC>::matches) return value;

  ShPointer<ShDataVariant<SRC> > srcVariant = 
    shref_dynamic_cast<ShDataVariant<SRC> >(value);
  ShDataVariant<DEST> *result = new ShDataVariant<DEST>(srcVariant->size());

  typename ShDataVariant<SRC>::const_iterator S = srcVariant->begin();
  typename ShDataVariant<DEST>::iterator D = result->begin();
  for(;S != srcVariant->end(); ++S, ++D) doCast(*D, *S);

  return result;
}

template<typename DEST, typename SRC>
ShVariantCPtr ShDataVariantCast<DEST, SRC>::operator()(ShVariantCPtr value) const
{
  if (MatchType<DEST, SRC>::matches) return value;

  ShPointer<const ShDataVariant<SRC> > srcVariant = 
    shref_dynamic_cast<const ShDataVariant<SRC> >(value);
  ShDataVariant<DEST> *result = new ShDataVariant<DEST>(srcVariant->size());

  typename ShDataVariant<SRC>::const_iterator S = srcVariant->begin();
  typename ShDataVariant<DEST>::iterator D = result->begin();
  for(;S != srcVariant->end(); ++S, ++D) doCast(*D, *S);

  return result;
}

template<typename DEST, typename SRC>
void ShDataVariantCast<DEST, SRC>::doCast(DEST &dest, const SRC &src) const
{
  // default C++ cast for now.  Users can add their own special weird
  // casts later on.
  // (This allows people to use a different cast operation than an existing
  // one defined in C++ in case they either (a) do not have access to the 
  // original source code of the type or (b) want to use a different
  // cast for the special cases.)
  dest = (DEST)(src);
}

}

#endif
