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
