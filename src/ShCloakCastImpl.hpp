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
#ifndef SHCLOAKCASTIMPL_HPP
#define SHCLOAKCASTIMPL_HPP

#include "ShCloak.hpp"
#include "ShCloakCast.hpp"

namespace SH {

template<typename DEST, typename SRC>
ShCloakPtr ShDataCloakCast<DEST, SRC>::operator()(ShCloakPtr value) const
{
  if (MatchType<DEST, SRC>::matches) return value;

  ShPointer<ShDataCloak<SRC> > srcCloak = 
    shref_dynamic_cast<ShDataCloak<SRC> >(value);
  ShDataCloak<DEST> *result = new ShDataCloak<DEST>(srcCloak->size());

  typename ShDataCloak<SRC>::const_iterator S = srcCloak->begin();
  typename ShDataCloak<DEST>::iterator D = result->begin();
  for(;S != srcCloak->end(); ++S, ++D) doCast(*D, *S);

  return result;
}

template<typename DEST, typename SRC>
ShCloakCPtr ShDataCloakCast<DEST, SRC>::operator()(ShCloakCPtr value) const
{
  if (MatchType<DEST, SRC>::matches) return value;

  ShPointer<const ShDataCloak<SRC> > srcCloak = 
    shref_dynamic_cast<const ShDataCloak<SRC> >(value);
  ShDataCloak<DEST> *result = new ShDataCloak<DEST>(srcCloak->size());

  typename ShDataCloak<SRC>::const_iterator S = srcCloak->begin();
  typename ShDataCloak<DEST>::iterator D = result->begin();
  for(;S != srcCloak->end(); ++S, ++D) doCast(*D, *S);

  return result;
}

template<typename DEST, typename SRC>
void ShDataCloakCast<DEST, SRC>::doCast(DEST &dest, const SRC &src) const
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
