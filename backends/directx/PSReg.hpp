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
#ifndef PSREG_HPP
#define PSREG_HPP

#include <string>
#include <iosfwd>

#include "ShRefCount.hpp"

namespace shdx {

/** Possible register types in the PS spec.
 */
enum PSRegType {
  SH_PS_REG_INPUT,
  SH_PS_REG_VSOUTPUT,
  SH_PS_REG_PSOUTPUT,
  SH_PS_REG_PSOUTPUT2,
  SH_PS_REG_ADDRESS,
  SH_PS_REG_TEMP,
  SH_PS_REG_CONST,
  SH_PS_REG_TEXTURE,
  SH_PS_REG_COLOUR
};

/** Possible bindings for a register (see ARB spec).
 */
enum PSRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PARAMLOC,
  SH_ARB_REG_PARAMENV,
  // Output
  SH_ARB_REG_RESULTCOL,

  // VERTEX
  // Input
  SH_ARB_REG_VERTEXPOS,
  SH_ARB_REG_VERTEXWGT,
  SH_ARB_REG_VERTEXNRM,
  SH_ARB_REG_VERTEXCOL,
  SH_ARB_REG_VERTEXFOG,
  SH_ARB_REG_VERTEXTEX,
  SH_ARB_REG_VERTEXMAT,
  SH_ARB_REG_VERTEXATR,
  // Output
  SH_ARB_REG_RESULTPOS,
  SH_ARB_REG_RESULTFOG,
  SH_ARB_REG_RESULTPTS, ///< Result point size
  SH_ARB_REG_RESULTTEX,

  // FRAGMENT
  // Input
  SH_ARB_REG_FRAGMENTCOL,
  SH_ARB_REG_FRAGMENTTEX,
  SH_ARB_REG_FRAGMENTFOG,
  SH_ARB_REG_FRAGMENTPOS,
  // Output
  SH_ARB_REG_RESULTDPT,

  SH_ARB_REG_NONE
};

/** A VS/PS register.
 */
struct PSReg : public SH::ShRefCountable {
  PSReg();
  PSReg(PSRegType type, int index, std::string name = "");

  PSRegType type;
  int index;
  std::string name; //< variable name (if any) associated with the register

  union {
    struct {
      PSRegBinding binding;
      int bindingIndex;
    };
    float values[4];
  };

  friend std::ostream& operator<<(std::ostream& out, const PSReg& reg);
  
  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const;
};


}

#endif
