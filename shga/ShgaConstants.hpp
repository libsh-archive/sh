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
const ShgaGrade ShgaGradeMap[] = { SHGA_GRADE0, SHGA_GRADE1, SHGA_GRADE2, 
  SHGA_GRADE3, SHGA_GRADE4, SHGA_GRADE5, SHGA_GRADE6, SHGA_GRADE7 }; 

enum ShgaParity {
  SHGA_EVEN = SHGA_GRADE0 | SHGA_GRADE2 | SHGA_GRADE4 | SHGA_GRADE6;
  SHGA_ODD = SHGA_GRADE1 | SHGA_GRADE3 | SHGA_GRADE5 | SHGA_GRADE7;
}

/* Use similar bit masks for specifying basis elements */
enum ShgaBasisElement {
  SHGA_E0 = 1,
  SHGA_E1 = SHGA_E0 << 1,
  SHGA_E2 = SHGA_E1 << 1,
  SHGA_E3 = SHGA_E2 << 1,
  SHGA_E4 = SHGA_E3 << 1,
  SHGA_E5 = SHGA_E4 << 1,
  SHGA_E6 = SHGA_E5 << 1,
  SHGA_E7 = SHGA_E6 << 1,
};

const int SHGA_NEGATIVE_BIT = SHGA_E7 << 1; 


/* maps integer grade to ShgaGrade constant */ 
const ShgaBasisElement ShgaBasisElementMap[] = { SHGA_E0, SHGA_E1, SHGA_E2, 
  SHGA_E3, SHGA_E4, SHGA_E5, SHGA_E6, SHGA_E7 }; 

const string ShgaBasisElementName[] = { "e0", "e1", "e2", "e3", "e4",
  "e5", "e6", "e7" };

}

#endif
