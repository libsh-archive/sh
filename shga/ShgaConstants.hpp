// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Bryan Chan 
//          
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
#ifndef SHGA_CONSTANTS_HPP 
#define SHGA_CONSTANTS_HPP 

namespace Shga {

/** \file ShgaConstants.hpp
 * This holds a number of constants used by all geometric algebras
 */
// TODO add Daniel's license at top

/* Use bit masks to represents grades present (a la Gaigen)*/ 
enum ShgaGrade {
  SHGA_GRADE0 = 1,
  SHGA_GRADE1 = SHGA_GRADE0 << 1,
  SHGA_GRADE2 = SHGA_GRADE1 << 1,
  SHGA_GRADE3 = SHGA_GRADE2 << 1,
  SHGA_GRADE4 = SHGA_GRADE3 << 1,
  SHGA_GRADE5 = SHGA_GRADE4 << 1,
  SHGA_GRADE6 = SHGA_GRADE5 << 1,
  SHGA_GRADE7 = SHGA_GRADE6 << 1,
};

/* maps integer grade to ShgaGrade constant */ 
extern const ShgaGrade ShgaGradeMap[]; 

/* Use similar bit masks for specifying basis elements */
enum ShgaBasisElement {
  SHGA_E1 = 1, 
  SHGA_E2 = SHGA_E1 << 1,
  SHGA_E3 = SHGA_E2 << 1,
  SHGA_E4 = SHGA_E3 << 1,
  SHGA_E5 = SHGA_E4 << 1,
  SHGA_E6 = SHGA_E5 << 1,
  SHGA_E7 = SHGA_E6 << 1,
  SHGA_E8 = SHGA_E7 << 1
};

/* maps integer grade to ShgaGrade constant */ 
extern const ShgaBasisElement ShgaBasisElementMap[]; 

/* maps integer grade to a basis string */
extern const char* ShgaBasisElementName[]; 

}

#endif
