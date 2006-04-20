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
#include "DllExport.hpp"
#include "StorageType.hpp"

namespace SH {

#define STORAGETYPE_NAME_SPEC_DEF(T, str)\
const char* __StorageTypeName<T >::name = str; 

STORAGETYPE_NAME_SPEC_DEF(Half, "h");
STORAGETYPE_NAME_SPEC_DEF(float, "f");
STORAGETYPE_NAME_SPEC_DEF(double, "d");

STORAGETYPE_NAME_SPEC_DEF(char, "b");
STORAGETYPE_NAME_SPEC_DEF(short, "s");
STORAGETYPE_NAME_SPEC_DEF(int, "i");
STORAGETYPE_NAME_SPEC_DEF(unsigned char, "ub");
STORAGETYPE_NAME_SPEC_DEF(unsigned short, "us");
STORAGETYPE_NAME_SPEC_DEF(unsigned int, "ui");

STORAGETYPE_NAME_SPEC_DEF(FracByte, "fb");
STORAGETYPE_NAME_SPEC_DEF(FracShort, "fs");
STORAGETYPE_NAME_SPEC_DEF(FracInt, "fi");
STORAGETYPE_NAME_SPEC_DEF(FracUByte, "fub");
STORAGETYPE_NAME_SPEC_DEF(FracUShort, "fus");
STORAGETYPE_NAME_SPEC_DEF(FracUInt, "fui");

}
