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
