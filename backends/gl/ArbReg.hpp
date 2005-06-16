// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef ARBREG_HPP
#define ARBREG_HPP

#include <string>
#include <iosfwd>

#include "ShRefCount.hpp"

namespace shgl {

/** Possible register types in the ARB spec.
 */
enum ArbRegType {
  SH_ARB_REG_ATTRIB,
  SH_ARB_REG_PARAM,
  SH_ARB_REG_TEMP,
  SH_ARB_REG_HALF_TEMP, // @todo type may want to rethink this
  SH_ARB_REG_ADDRESS,
  SH_ARB_REG_OUTPUT,
  SH_ARB_REG_CONST,
  SH_ARB_REG_TEXTURE
};

/** Possible bindings for a register (see ARB spec).
 */
enum ArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PROGRAMLOC,
  SH_ARB_REG_PROGRAMENV,
  SH_ARB_REG_STATE,
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
  SH_ARB_REG_RESULTCOL_ATI,

  SH_ARB_REG_NONE
};

/** An ARB register.
 */
struct ArbReg : public SH::ShRefCountable {
  ArbReg();
  ArbReg(ArbRegType type, int index, std::string name = "");

  ArbRegType type;
  int index;
  bool preset;
  std::string name; //< variable name (if any) associated with the register

  struct BindingInfo
    {
    ArbRegBinding type;
    int index;
    int count;
    std::string name;
    float values[4];
    };
  BindingInfo binding;

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);
  
  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const;
};


}

#endif
