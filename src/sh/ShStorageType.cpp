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
#include "ShDllExport.hpp"
#include "ShStorageType.hpp"

namespace SH {

#define SH_STORAGETYPE_NAME_SPEC_DEF(T, str)\
const char* __ShStorageTypeName<T >::name = str; 

SH_STORAGETYPE_NAME_SPEC_DEF(ShHalf, "h");
SH_STORAGETYPE_NAME_SPEC_DEF(float, "f");
SH_STORAGETYPE_NAME_SPEC_DEF(double, "d");

SH_STORAGETYPE_NAME_SPEC_DEF(char, "b");
SH_STORAGETYPE_NAME_SPEC_DEF(short, "s");
SH_STORAGETYPE_NAME_SPEC_DEF(int, "i");
SH_STORAGETYPE_NAME_SPEC_DEF(unsigned char, "ub");
SH_STORAGETYPE_NAME_SPEC_DEF(unsigned short, "us");
SH_STORAGETYPE_NAME_SPEC_DEF(unsigned int, "ui");

SH_STORAGETYPE_NAME_SPEC_DEF(ShFracByte, "fb");
SH_STORAGETYPE_NAME_SPEC_DEF(ShFracShort, "fs");
SH_STORAGETYPE_NAME_SPEC_DEF(ShFracInt, "fi");
SH_STORAGETYPE_NAME_SPEC_DEF(ShFracUByte, "fub");
SH_STORAGETYPE_NAME_SPEC_DEF(ShFracUShort, "fus");
SH_STORAGETYPE_NAME_SPEC_DEF(ShFracUInt, "fui");

}
