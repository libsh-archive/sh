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

#include <climits>
#include "ShDataType.hpp"

namespace SH {
const char* dataTypeName[] = {
  "host",
  "memory",
  "unknown"
};

/** Special case data type constant values */
const unsigned short ShDataTypeConstant<SH_HALF, SH_MEM>::Zero = 0; 
const unsigned short ShDataTypeConstant<SH_HALF, SH_MEM>::One = 15 << 10; 

// @todo type fill these in 
const int ShDataTypeConstant<SH_FRAC_INT, SH_MEM>::Zero = 0; 
const short ShDataTypeConstant<SH_FRAC_SHORT, SH_MEM>::Zero = 0; 
const char ShDataTypeConstant<SH_FRAC_BYTE, SH_MEM>::Zero = 0; 
const unsigned int ShDataTypeConstant<SH_FRAC_UINT, SH_MEM>::Zero = 0; 
const unsigned short ShDataTypeConstant<SH_FRAC_USHORT, SH_MEM>::Zero = 0; 
const unsigned char ShDataTypeConstant<SH_FRAC_UBYTE, SH_MEM>::Zero = 0; 

const int ShDataTypeConstant<SH_FRAC_INT, SH_MEM>::One = INT_MAX; 
const short ShDataTypeConstant<SH_FRAC_SHORT, SH_MEM>::One = SHRT_MAX; 
const char ShDataTypeConstant<SH_FRAC_BYTE, SH_MEM>::One = SCHAR_MAX; 
const unsigned int ShDataTypeConstant<SH_FRAC_UINT, SH_MEM>::One = UINT_MAX; 
const unsigned short ShDataTypeConstant<SH_FRAC_USHORT, SH_MEM>::One = USHRT_MAX; 
const unsigned char ShDataTypeConstant<SH_FRAC_UBYTE, SH_MEM>::One = UCHAR_MAX; 

}
