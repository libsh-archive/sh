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
#include "ShGeneric.hpp"
#include "ShGenericImpl.hpp"

namespace SH {

template class ShGeneric<1, float>;
template class ShGeneric<2, float>;
template class ShGeneric<3, float>;
template class ShGeneric<4, float>;

template ShGeneric<1, float> ShGeneric<1, float>::swiz<1>(int[]) const;
template ShGeneric<2, float> ShGeneric<1, float>::swiz<2>(int[]) const;
template ShGeneric<3, float> ShGeneric<1, float>::swiz<3>(int[]) const;
template ShGeneric<4, float> ShGeneric<1, float>::swiz<4>(int[]) const;
template ShGeneric<1, float> ShGeneric<2, float>::swiz<1>(int[]) const;
template ShGeneric<2, float> ShGeneric<2, float>::swiz<2>(int[]) const;
template ShGeneric<3, float> ShGeneric<2, float>::swiz<3>(int[]) const;
template ShGeneric<4, float> ShGeneric<2, float>::swiz<4>(int[]) const;
template ShGeneric<1, float> ShGeneric<3, float>::swiz<1>(int[]) const;
template ShGeneric<2, float> ShGeneric<3, float>::swiz<2>(int[]) const;
template ShGeneric<3, float> ShGeneric<3, float>::swiz<3>(int[]) const;
template ShGeneric<4, float> ShGeneric<3, float>::swiz<4>(int[]) const;
template ShGeneric<1, float> ShGeneric<4, float>::swiz<1>(int[]) const;
template ShGeneric<2, float> ShGeneric<4, float>::swiz<2>(int[]) const;
template ShGeneric<3, float> ShGeneric<4, float>::swiz<3>(int[]) const;
template ShGeneric<4, float> ShGeneric<4, float>::swiz<4>(int[]) const;

}
