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
#ifndef SHCLOAKCAST_HPP
#define SHCLOAKCAST_HPP

#include "ShRefCount.hpp"

namespace SH {

class ShCloak;
struct ShCloakCast: public ShRefCountable {
  public:
    virtual ~ShCloakCast();

    virtual ShPointer<ShCloak> operator()(ShPointer<ShCloak> value) const = 0;
    virtual ShPointer<const ShCloak> operator()(
        ShPointer<const ShCloak> value) const = 0;
};

typedef ShPointer<ShCloakCast> ShCloakCastPtr;
typedef ShPointer<const ShCloakCast> ShCloakCastCPtr;

template<typename DEST, typename SRC>
struct ShDataCloakCast: public ShCloakCast {
  public:
    ShPointer<ShCloak> operator()(ShPointer<ShCloak> value) const;
    ShPointer<const ShCloak> operator()(ShPointer<const ShCloak> value) const;

    void doCast(DEST &dest, const SRC &src) const;
};


}

#include "ShCloakCastImpl.hpp"

#endif
